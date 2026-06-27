#include "DaydreamPCH.h"
#include "D3D12Swapchain.h"

#include "D3D12Utility.h"

#include "Daydream/Graphics/Core/Renderer2D.h"

#include "Daydream/Core/Window.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

namespace Daydream
{
	D3D12Swapchain::D3D12Swapchain(D3D12RenderDevice* _device, const DaydreamWindow& _window, const SwapchainDesc& _desc)
		:Swapchain(_desc)
	{
		device = _device;
		desc = _desc;
		bufferCount = _desc.imageCount;
		format = GraphicsUtility::DirectX::ConvertToDXGIFormat(_desc.format);;
		DXGI_SAMPLE_DESC sampleDesc = {};
		sampleDesc.Count = 1;    // 샘플 수 (1이면 MSAA 비활성화)
		sampleDesc.Quality = 0;  // 품질 레벨 (0이면 기본값)

		DXGI_SWAP_CHAIN_DESC1 swapchainDesc;
		ZeroMemory(&swapchainDesc, sizeof(swapchainDesc));
		swapchainDesc.Width = _desc.width;
		swapchainDesc.Height = _desc.height;
		swapchainDesc.Format = format;
		swapchainDesc.Stereo = 0;
		swapchainDesc.SampleDesc = sampleDesc;
		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.BufferCount = _desc.imageCount;
		//swapchainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

		windowHandle = glfwGetWin32Window((GLFWwindow*)_window.GetNativeWindow());

		ComPtr<IDXGISwapChain1> swapChain1;
		HRESULT hr = device->GetFactory()->CreateSwapChainForHwnd(
			device->GetCommandQueue(),
			windowHandle,
			&swapchainDesc,
			nullptr, nullptr,
			swapChain1.GetAddressOf()
		);

		swapChain1->QueryInterface(swapchain.GetAddressOf());
		DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create swapChain!");

		commandLists.resize(_desc.imageCount);
		for (UInt32 i = 0; i < _desc.imageCount; i++)
		{
			commandLists[i] = MakeShared<D3D12RenderCommandList>(device);
		}

		// 여기서부터 스왑체인이 사용할 백버퍼로 렌더타겟 뷰 생성
		CreateBackBufferView();

		//RenderPassAttachmentDesc colorDesc;
		//colorDesc.format = _desc.format;
		//colorDesc.isSwapchain = true;

		//RenderPassDesc rpDesc{};
		//rpDesc.colorAttachments.push_back(colorDesc);

		//mainRenderPass = MakeShared<D3D12RenderPass>(device, rpDesc);

		//framebuffers.resize(desc.imageCount);
		//for (UInt32 i = 0; i < desc.imageCount; i++)
		//{

		//	framebuffers[i] = MakeShared<D3D12Framebuffer>(device, mainRenderPass.get(), this, backBuffer.Get());
		//	d3d12Backbuffers.push_back(backBuffer);
		//}

		frameIndex = swapchain->GetCurrentBackBufferIndex();

		fenceValues.resize(_desc.imageCount);
		//fence가 0까지는 완료된 상태로 생성
		hr = device->GetDevice()->CreateFence(fenceValues[frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
		//fenceValues[frameIndex]++;
		//fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		fenceEvent.Attach(CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE));
		if (fenceEvent == nullptr)
		{
			DAYDREAM_CORE_ERROR("Failed to Create FenceEvent!");
		}

		currentFenceValue = 0;
	}
	
	D3D12Swapchain::~D3D12Swapchain()
	{
		WaitForGPU();
		device = nullptr;
	}
	void D3D12Swapchain::SetVSync(bool _enabled)
	{
	}

	void D3D12Swapchain::Present()
	{
		swapchain->Present(desc.isVSync, 0);
		//frameIndex = 0->1
		frameIndex = swapchain->GetCurrentBackBufferIndex();
	}


	void D3D12Swapchain::BeginFrame()
	{
		if (isSwapchainResized)
		{
			WaitForGPU();

			//이전 백버퍼를 붙잡고 있던 렌더타겟뷰, 텍스쳐배열을 놓아줘야 리사이즈 할 수 있음
			backBufferRTVs.clear();
			backBufferTextures.clear();

			swapchain->ResizeBuffers(desc.imageCount, desc.width, desc.height, format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING);

			CreateBackBufferView();

			frameIndex = swapchain->GetCurrentBackBufferIndex();
			for (UInt32 i = 0; i < desc.imageCount; i++)
			{
				fenceValues[i] = currentFenceValue;
			}

			isSwapchainResized = false;
		}

		// frameIndex의 작업이 끝난상태인지 확인
		if (fence->GetCompletedValue() < fenceValues[frameIndex])
		{
			fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent.Get());
			WaitForSingleObjectEx(fenceEvent.Get(), INFINITE, FALSE);
		}

		currentCommandList = commandLists[frameIndex]->GetID3D12GraphicsCommandList();

		commandLists[frameIndex]->Begin();

		ID3D12DescriptorHeap* heaps[] = { device->GetCBVSRVUAVHeap(), device->GetSamplerHeap() };
		currentCommandList->SetDescriptorHeaps(2, heaps);

		D3D12_RESOURCE_BARRIER barr{};
		barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barr.Transition.pResource = backBufferTextures[frameIndex]->GetID3D12Resource();
		barr.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barr.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barr.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		currentCommandList->ResourceBarrier(1, &barr);
	}

	void D3D12Swapchain::EndFrame()
	{
		D3D12_RESOURCE_BARRIER barr{};

		barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barr.Transition.pResource = backBufferTextures[frameIndex]->GetID3D12Resource();
		barr.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barr.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barr.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		currentCommandList->ResourceBarrier(1, &barr);

		commandLists[frameIndex]->End();

		Array<ID3D12CommandList*> execCommandLists = { currentCommandList };
		//이제 0번 이미지에 그려
		device->GetCommandQueue()->ExecuteCommandLists((UInt32)execCommandLists.size(), execCommandLists.data());

		//여기서 FenceValue = 0 -> 1
		currentFenceValue++;
		//다그리면 Fence에 1 전달해
		device->GetCommandQueue()->Signal(fence.Get(), currentFenceValue);

		//FenceValue[0] = 1; // 이제 0번 이미지 다시 쓰려면 Fence가 1보다 커야된다
		fenceValues[frameIndex] = currentFenceValue;
	}

	//모든 GPU작업이 끝날때까지 대기
	void D3D12Swapchain::WaitForGPU()
	{
		currentFenceValue++;
		//현재 gpu에 있는 작업들 끝나면 currentFenceValue로 fence값 설정해
		device->GetCommandQueue()->Signal(fence.Get(), currentFenceValue);

		//currentFenceValue값 받을때까지 기다려
		fence->SetEventOnCompletion(currentFenceValue, fenceEvent.Get());
		WaitForSingleObjectEx(fenceEvent.Get(), INFINITE, FALSE);
	}

	// GPU가 이전 프레임 작업을 끝낼 때까지 기다림
	void D3D12Swapchain::MoveToNextFrame()
	{
		// 현재 프레임의 커맨드 제출이 완료되면 fenceValues[frameIndex]를 펜스에 signal 해라
		const UINT64 currentFenceValue = fenceValues[frameIndex]; // 현재 프레임의 고유 펜스 값
		HRESULT hr = device->GetCommandQueue()->Signal(fence.Get(), currentFenceValue);
		DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to signal!");

		// 다음 백 버퍼 인덱스 획득 (GPU가 렌더링을 마친 버퍼) Present이후 호출이므로 바뀜
		frameIndex = swapchain->GetCurrentBackBufferIndex();

		// 다음 프레임에 사용될 백 버퍼의 펜스 값 확인 및 대기
		// 새 프레임의 fenceValue보다 완료된 value가 높으면 넘어감
		if (fence->GetCompletedValue() < fenceValues[frameIndex])
		{
			// 완료되지 않았다면, 해당 펜스 값이 시그널될 때까지 CPU 대기
			fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent.Get());
			WaitForSingleObjectEx(fenceEvent.Get(), INFINITE, FALSE);
		}
		// 현재 프레임이 다음에 signal 할 값 = 이전 프레임의 펜스값 + 1
		fenceValues[frameIndex] = currentFenceValue + 1;
	}

	void D3D12Swapchain::CreateBackBufferView()
	{
		TextureDesc textureDesc{};
		textureDesc.width = desc.width;
		textureDesc.height = desc.height;
		textureDesc.mipLevels = 1;
		textureDesc.sampleCount = 1;
		textureDesc.format = desc.format;
		textureDesc.textureUsage = TextureUsage::RenderTarget;
		textureDesc.type = TextureType::Texture2D;

		TextureViewDesc viewDesc;
		viewDesc.format = desc.format;
		viewDesc.type = TextureViewType::RenderTarget;

		backBufferTextures.resize(desc.imageCount);
		backBufferRTVs.resize(desc.imageCount);
		for (UInt32 i = 0; i < desc.imageCount; i++)
		{
			ComPtr<ID3D12Resource> backBuffer;
			swapchain->GetBuffer(i, IID_PPV_ARGS(backBuffer.GetAddressOf()));
			backBuffer->SetName(L"Swapchain Buffer");
			backBufferTextures[i] = MakeShared<D3D12GPUTexture>(device, textureDesc, backBuffer);
			backBufferRTVs[i] = MakeShared<D3D12TextureView>(device, backBufferTextures[i].get(), viewDesc);
		}
	}
}
