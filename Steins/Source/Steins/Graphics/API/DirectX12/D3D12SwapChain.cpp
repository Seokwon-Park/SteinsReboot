#include "SteinsPCH.h"
#include "D3D12Swapchain.h"

#include "Steins/Graphics/Utility/GraphicsUtil.h"

#include "Steins/Graphics/Core/Renderer2D.h"

#include "Steins/Core/Window.h"
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

namespace Steins
{
	D3D12Swapchain::D3D12Swapchain(RenderDevice* _device, SteinsWindow* _window, const SwapchainDesc& _desc)
	{
		device = Cast<D3D12RenderDevice>(_device);
		desc = _desc;
		DXGI_SAMPLE_DESC sampleDesc = {};
		sampleDesc.Count = 1;    // 샘플 수 (1이면 MSAA 비활성화)
		sampleDesc.Quality = 0;  // 품질 레벨 (0이면 기본값)

		DXGI_SWAP_CHAIN_DESC1 swapchainDesc;
		ZeroMemory(&swapchainDesc, sizeof(swapchainDesc));
		swapchainDesc.Width = _desc.width;
		swapchainDesc.Height = _desc.height;
		swapchainDesc.Format = GraphicsUtil::RenderFormatToDXGIFormat(_desc.format);
		swapchainDesc.Stereo = 0;
		swapchainDesc.SampleDesc = sampleDesc;
		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.BufferCount = _desc.bufferCount;
		swapchainDesc.Scaling = DXGI_SCALING_NONE;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapchainDesc.Flags = 0;

		fenceValues.resize(_desc.bufferCount);

		ComPtr<IDXGISwapChain1> swapChain1;
		HRESULT hr = device->GetFactory()->CreateSwapChainForHwnd(
			device->GetCommandQueue(),
			glfwGetWin32Window((GLFWwindow*)_window->GetNativeWindow()),
			&swapchainDesc,
			nullptr, nullptr,
			swapChain1.GetAddressOf()
		);

		swapChain1->QueryInterface(swapChain.GetAddressOf());

		framebuffers.resize(desc.bufferCount);
		for (UInt32 i = 0; i < desc.bufferCount; i++)
		{
			frameIndex = i;
			framebuffers[i] = MakeShared<D3D12Framebuffer>(device, this);
		}
		frameIndex = swapChain->GetCurrentBackBufferIndex();

		hr = device->GetDevice()->CreateFence(fenceValues[frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
		fenceValues[frameIndex]++; // 초기 값 증가
		fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (fenceEvent == nullptr)
		{
			STEINS_CORE_ERROR("Failed to Create FenceEvent!");
		}

		STEINS_CORE_ASSERT(SUCCEEDED(hr), "Failed to create swapchain!");
		device->GetCommandAllocator(frameIndex)->Reset();
		device->GetCommandList()->Reset(device->GetCommandAllocator(frameIndex), nullptr);
		ID3D12DescriptorHeap* heaps[] = { device->GetCBVSRVUAVHeap() };
		device->GetCommandList()->SetDescriptorHeaps(1, heaps);
		framebuffers[frameIndex]->Begin();
		framebuffers[frameIndex]->Clear(Color(1.0f, 1.0f, 1.0f, 1.0f));

		D3D12_RESOURCE_BARRIER barrier{};

		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = framebuffers[frameIndex]->GetRenderTarget().Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		device->GetCommandList()->ResourceBarrier(1, &barrier);
	}
	D3D12Swapchain::~D3D12Swapchain()
	{
		WaitForGPU();
		device = nullptr;
	}
	void D3D12Swapchain::SetVSync(bool _enabled)
	{
	}

	void D3D12Swapchain::SwapBuffers()
	{
		D3D12_RESOURCE_BARRIER barr{};

		barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barr.Transition.pResource = framebuffers[frameIndex]->GetRenderTarget().Get();
		barr.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barr.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barr.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		device->GetCommandList()->ResourceBarrier(1, &barr);

		device->GetCommandList()->Close();
		commandLists.push_back(device->GetCommandList());

		device->GetCommandQueue()->ExecuteCommandLists((UInt32)commandLists.size(), commandLists.data());
		commandLists.clear();
		swapChain->Present(desc.isVSync, 0);

		MoveToNextFrame();

		device->GetCommandAllocator(frameIndex)->Reset();
		device->GetCommandList()->Reset(device->GetCommandAllocator(frameIndex), nullptr);

		D3D12_VIEWPORT viewport = {};
		viewport.Width = static_cast<float>(desc.width);
		viewport.Height = static_cast<float>(desc.height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		device->GetCommandList()->RSSetViewports(1, &viewport);

		D3D12_RECT scissorRect = {};
		scissorRect.left = 0;
		scissorRect.top = 0;
		scissorRect.right = static_cast<LONG>(desc.width);
		scissorRect.bottom = static_cast<LONG>(desc.height);
		device->GetCommandList()->RSSetScissorRects(1, &scissorRect);

		ID3D12DescriptorHeap* srvHeap = device->GetCBVSRVUAVHeap();
		device->GetCommandList()->SetDescriptorHeaps(1, &srvHeap);

		barr.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barr.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barr.Transition.pResource = framebuffers[frameIndex]->GetRenderTarget().Get();
		barr.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barr.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barr.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		device->GetCommandList()->ResourceBarrier(1, &barr);

		framebuffers[frameIndex]->Begin();
		framebuffers[frameIndex]->Clear(Color(1.0f, 1.0f, 1.0f, 1.0f));

	}
	void D3D12Swapchain::ResizeSwapchain(UInt32 _width, UInt32 height)
	{
	}

	void D3D12Swapchain::BeginFrame()
	{
	}

	void D3D12Swapchain::EndFrame()
	{
	}

	void D3D12Swapchain::WaitForGPU()
	{
		// Schedule a Signal command in the queue.
		device->GetCommandQueue()->Signal(fence.Get(), fenceValues[frameIndex]);

		// Wait until the fence has been processed.
		fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
		WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);

		// Increment the fence value for the current frame.
		fenceValues[frameIndex]++;
	}

	// GPU가 이전 프레임 작업을 끝낼 때까지 기다림
	void D3D12Swapchain::MoveToNextFrame()
	{
		const UINT64 currentFenceValue = fenceValues[frameIndex];
		HRESULT hr = device->GetCommandQueue()->Signal(fence.Get(), currentFenceValue);
		STEINS_CORE_ASSERT(SUCCEEDED(hr), "Failed to signal!");

		frameIndex = swapChain->GetCurrentBackBufferIndex();

		if (fence->GetCompletedValue() < fenceValues[frameIndex])
		{
			fence->SetEventOnCompletion(fenceValues[frameIndex], fenceEvent);
			WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
		}
		fenceValues[frameIndex] = currentFenceValue + 1;
	}
}
