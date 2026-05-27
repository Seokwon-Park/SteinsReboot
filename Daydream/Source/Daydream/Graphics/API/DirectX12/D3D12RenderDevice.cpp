#include "DaydreamPCH.h"
#include "D3D12RenderDevice.h"
#include "D3D12Buffer.h"
#include "D3D12RenderContext.h"
#include "D3D12Shader.h"
#include "D3D12Swapchain.h"
#include "D3D12GraphicsPipelineState.h"
#include "D3D12ImGuiRenderer.h"
#include "D3D12Texture.h"
#include "D3D12TextureView.h"
#include "D3D12TextureCube.h"
#include "D3D12Sampler.h"
#include "Daydream/Graphics/Utility/GraphicsUtility.h"

namespace Daydream
{
	D3D12RenderDevice::D3D12RenderDevice()
	{
		API = RendererAPIType::DirectX12;
	}

	D3D12RenderDevice::~D3D12RenderDevice()
	{
	}

	void D3D12RenderDevice::Init()
	{
		UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
		// Enable the D3D12 debug layer.
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugLayer.GetAddressOf()))))
		{
			debugLayer->EnableDebugLayer();
			// Enable additional debug layers.
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
#endif
		// DXGI 팩토리 생성
		HRESULT hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
		if (FAILED(hr))
		{
			DAYDREAM_CORE_ERROR("Failed to create dxgiFactory!");
		}

		// 디바이스 생성
		int adapterIndex = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0
		bool isAdapterFound = false; // set this to true when a good one was found
		for (UINT adapterIndex = 0;
			dxgiFactory->EnumAdapterByGpuPreference(
				adapterIndex,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
				IID_PPV_ARGS(&dxgiAdapter)
			) != DXGI_ERROR_NOT_FOUND;
			++adapterIndex)
		{
			DXGI_ADAPTER_DESC3 desc;
			dxgiAdapter->GetDesc3(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}

			//이 어댑터로 Device를 만들 수 있는지 테스트한다.
			hr = D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr);
			if (SUCCEEDED(hr))
			{
				//성공하면 빠져나온다.
				isAdapterFound = true;
				break;
			}
		}

		DAYDREAM_CORE_ASSERT(isAdapterFound, "Failed to find Adapter!");

		hr = D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(device.GetAddressOf()));
		DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to Create Device!");

		D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
		allocatorDesc.pDevice = device.Get();
		allocatorDesc.pAdapter = dxgiAdapter.Get();
		allocatorDesc.Flags = Cast<D3D12MA::ALLOCATOR_FLAGS>(D3D12MA_RECOMMENDED_ALLOCATOR_FLAGS);

		hr = D3D12MA::CreateAllocator(&allocatorDesc, memoryAllocator.GetAddressOf());
		DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to Create Memory Allocator!");


#if defined(DEBUG) || defined(_DEBUG)
		Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
		if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
		{
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, FALSE); // 원하면 TRUE로

			// 필요 없는 메시지는 필터링도 가능
			D3D12_MESSAGE_ID denyIds[] = {
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
				D3D12_MESSAGE_ID_CLEARDEPTHSTENCILVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE
			};
			D3D12_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = _countof(denyIds);
			filter.DenyList.pIDList = denyIds;
			infoQueue->AddStorageFilterEntries(&filter);
		}
#endif

		// 커맨드 큐 생성
		{
			D3D12_COMMAND_QUEUE_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			device->CreateCommandQueue(&desc, IID_PPV_ARGS(commandQueue.GetAddressOf()));
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc{};
			desc.NumDescriptors = 512;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(rtvHeap.GetAddressOf()));
			DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create RTV descriptor heap");
			rtvHeapAlloc.Create(device.Get(), rtvHeap.Get());
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc{};
			desc.NumDescriptors = 512;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(dsvHeap.GetAddressOf()));
			DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create DSV descriptor heap");
			dsvHeapAlloc.Create(device.Get(), dsvHeap.Get());

		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc{};
			desc.NumDescriptors = 512;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(samplerHeap.GetAddressOf()));
			DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create sampler descriptor heap");
			samplerHeapAlloc.Create(device.Get(), samplerHeap.Get());

		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc{};
			desc.NumDescriptors = 1024;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(cbvSrvUavHeap.GetAddressOf()));
			DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create cbvSrvUav descriptor heap");
			cbvSrvUavHeapAlloc.Create(device.Get(), cbvSrvUavHeap.Get());
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc{};
			desc.NumDescriptors = 512;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(dynamicSamplerHeap.GetAddressOf()));
			DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create sampler descriptor heap");
			dynamicSamplerHeapAlloc.Create(device.Get(), dynamicSamplerHeap.Get());

		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc{};
			desc.NumDescriptors = 1024;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(dynamicCbvSrvUavHeap.GetAddressOf()));
			DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create cbvSrvUav descriptor heap");
			dynamicCbvSrvUavHeapAlloc.Create(device.Get(), dynamicCbvSrvUavHeap.Get());
		}

		hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(uploadCommandAllocator.GetAddressOf()));
		DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create command allocator");

		hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, uploadCommandAllocator.Get(), nullptr, IID_PPV_ARGS(uploadCommandList.GetAddressOf()));
		DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create commandlist");

		uploadCommandList->Close();

		hr = device->CreateFence(uploadFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(uploadFence.GetAddressOf()));
		//다음 펜스값은 1을 보내야함
		uploadFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (uploadFenceEvent == nullptr)
		{
			DAYDREAM_CORE_ERROR("Failed to Create FenceEvent!");
		}

		DXGI_ADAPTER_DESC3 adapterDescription; // Vendor
		SecureZeroMemory(&adapterDescription, sizeof(DXGI_ADAPTER_DESC));
		char videoCardDescription[128]; // Renderer
		LARGE_INTEGER driverVersion; // Version

		dxgiAdapter->GetDesc3(&adapterDescription);
		dxgiAdapter->CheckInterfaceSupport(__uuidof(IDXGIDevice), &driverVersion);

		// Renderer
		wcstombs_s(NULL, videoCardDescription, 128, adapterDescription.Description, 128);

		// Version
		std::string major, minor, release, build;
		major = std::to_string(HIWORD(driverVersion.HighPart));
		minor = std::to_string(LOWORD(driverVersion.HighPart));
		release = std::to_string(HIWORD(driverVersion.LowPart));
		build = std::to_string(LOWORD(driverVersion.LowPart));

		std::string version;
		version = major + "." + minor + "." + release + "." + build;

		info.APIName = "DirectX12 Info:";
		info.vendor = GraphicsUtility::GetVendor(adapterDescription.VendorId);
		info.physicalDeviceInfo = videoCardDescription, std::round((double)adapterDescription.DedicatedVideoMemory / (1 << 30));
		info.version = version;

		//device->CreateDescriptorHeap()
		//// 스왑 체인 생성
		//DXGI_SWAP_CHAIN_DESC swapDesc = {};
		//swapDesc.BufferCount = 2;
		//swapDesc.BufferDesc.Width = 800;
		//swapDesc.BufferDesc.Height = 600;
		//swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		//swapDesc.OutputWindow = hwnd;
		//swapDesc.Windowed = TRUE;
		//swapDesc.SampleDesc.Count = 1;
		//dxgiFactory->CreateSwapchain(commandQueue.Get(), &swapDesc, &swapChain);

	}

	void D3D12RenderDevice::Shutdown()
	{
	}


	Unique<RenderContext> D3D12RenderDevice::CreateContext()
	{
		return MakeUnique<D3D12RenderContext>(this);
	}

	Shared<RenderCommandList> D3D12RenderDevice::CreateRenderCommandList()
	{
		return MakeShared<D3D12RenderCommandList>(this);
	}

	Shared<GPUBuffer> D3D12RenderDevice::CreateGPUBuffer(const BufferDesc& _desc)
	{
		return MakeShared<D3D12GPUBuffer>(this, _desc);
	}

	Shared<GPUTexture> D3D12RenderDevice::CreateGPUTexture(const TextureDesc& _desc)
	{
		return MakeShared<D3D12GPUTexture>(this, _desc);
	}

	Shared<TextureView> D3D12RenderDevice::CreateTextureView(Texture* _texture, const TextureViewDesc& _desc)
	{
		D3D12GPUTexture* texture = Cast<D3D12GPUTexture*>(_texture->GetGPUTexture());
		return MakeShared<D3D12TextureView>(this, texture, _desc);
	}

	//Shared<VertexBuffer> D3D12RenderDevice::CreateDynamicVertexBuffer(UInt32 _size, UInt32 _stride, UInt32 _initialDataSize, const void* _initialData)
	//{
	//	BufferDesc desc{};
	//	desc.bufferUsage = BufferUsage::Vertex;
	//	desc.memoryUsage = MemoryUsage::Dynamic;
	//	desc.size = _size;
	//	desc.initialData = _initialData;

	//	Shared<D3D12GPUBuffer> gpuBuffer = MakeShared<D3D12GPUBuffer>(this, desc);

	//	Shared<VertexBuffer> vertexBuffer = MakeShared<VertexBuffer>(gpuBuffer, _stride);
	//	if (_initialData != nullptr)
	//	{
	//		vertexBuffer->UpdateData(_initialData, _initialDataSize);
	//	}
	//	return vertexBuffer;
	//}

	//Shared<VertexBuffer> D3D12RenderDevice::CreateStaticVertexBuffer(UInt32 _size, UInt32 _stride, const void* _initialData)
	//{
	//	BufferDesc desc{};
	//	desc.bufferUsage = BufferUsage::Vertex;
	//	desc.memoryUsage = MemoryUsage::Static;
	//	desc.size = _size;

	//	Shared<D3D12GPUBuffer> vertexBuffer = MakeShared<D3D12GPUBuffer>(this, MemoryUsage::Static, _size, _stride);

	//	auto uploadBuffer = CreateBuffer(_size, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);

	//	void* bufferData;
	//	HRESULT hr = uploadBuffer->Map(0, nullptr, &bufferData);
	//	DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to map uploadBuffer");
	//	memcpy(bufferData, _initialData, _size);
	//	uploadBuffer->Unmap(0, nullptr);

	//	CopyBuffer(uploadBuffer.Get(), vertexBuffer->GetID3D12Resource(), _size);

	//	D3D12_RESOURCE_BARRIER barrier = {};
	//	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//	barrier.Transition.pResource = vertexBuffer->GetID3D12Resource();
	//	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	//	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	//	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	//	TransitionResourceStateImmediate(barrier);

	//	return vertexBuffer;
	//}


	//Shared<IndexBuffer> Daydream::D3D12RenderDevice::CreateIndexBuffer(const UInt32* _indices, UInt32 _count)
	//{
	//	auto indexBuffer = MakeShared<D3D12IndexBuffer>(this, _count);

	//	UInt32 bufferSize = sizeof(UInt32) * _count;
	//	auto uploadBuffer = CreateBuffer(bufferSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);

	//	void* bufferData;
	//	HRESULT hr = uploadBuffer->Map(0, nullptr, &bufferData);
	//	DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to map uploadBuffer");
	//	memcpy(bufferData, _indices, bufferSize);
	//	uploadBuffer->Unmap(0, nullptr);

	//	CopyBuffer(uploadBuffer.Get(), indexBuffer->GetID3D12Resource(), bufferSize);

	//	D3D12_RESOURCE_BARRIER barrier = {};
	//	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//	barrier.Transition.pResource = indexBuffer->GetID3D12Resource();
	//	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	//	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	//	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;
	//	TransitionResourceStateImmediate(barrier);

	//	return indexBuffer;
	//}

	//Shared<ConstantBuffer> D3D12RenderDevice::CreateConstantBuffer(UInt32 _size)
	//{
	//	return MakeShared<D3D12ConstantBuffer>(this, _size);
	//}

	//Shared<RenderPass> D3D12RenderDevice::CreateRenderPass(const RenderPassDesc& _desc)
	//{
	//	return MakeShared<D3D12RenderPass>(this, _desc);
	//}

	//Shared<Framebuffer> D3D12RenderDevice::CreateFramebuffer(Shared<RenderPass> _renderPass, const FramebufferDesc& _desc)
	//{
	//	return _renderPass->CreateFramebuffer(_desc);
	//}

	Shared<GraphicsPipelineState> D3D12RenderDevice::CreatePipelineState(const GraphicsPipelineStateDesc& _desc)
	{
		return MakeShared<D3D12GraphicsPipelineState>(this, _desc);
	}

	Shared<Shader> D3D12RenderDevice::CreateShader(const std::string& _src, const ShaderType& _type, ShaderLoadMode _mode)
	{
		return MakeShared<D3D12Shader>(this, _src, _type, _mode);
	}

	Shared<Swapchain> Daydream::D3D12RenderDevice::CreateSwapchain(const DaydreamWindow& _window, const SwapchainDesc& _desc)
	{
		return MakeShared<D3D12Swapchain>(this, _window, _desc);
	}

	//Shared<Texture2D> D3D12RenderDevice::CreateTexture2D(const void* _imageData, const TextureDesc& _desc)
	//{
	//	Shared<D3D12Texture2D> texture = MakeShared<D3D12Texture2D>(this, _desc);

	//	if (_imageData != nullptr)
	//	{
	//		D3D12_RESOURCE_DESC dstDesc = texture->GetID3D12Resource()->GetDesc();

	//		D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedFootprint;
	//		UINT64 totalBytes;
	//		device->GetCopyableFootp rints(&dstDesc, 0, 1, 0, &placedFootprint, nullptr, nullptr, &totalBytes);

	//		auto uploadBuffer = CreateBuffer(totalBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);

	//		UInt32 imageSize = _desc.width * _desc.height * GraphicsUtility::GetRenderFormatSize(_desc.format);

	//		void* pixelData;
	//		D3D12_RANGE range = { 0, totalBytes };
	//		uploadBuffer->Map(0, &range, &pixelData);
	//		BYTE* pDest = static_cast<BYTE*>(pixelData);
	//		BYTE* pSrc = const_cast<BYTE*>(static_cast<const BYTE*>(_imageData)); // 원본 이미지 데이터
	//		UInt32 rowCount = dstDesc.Height;
	//		UInt32 rowSizeInBytes = (UInt32)dstDesc.Width * GraphicsUtility::GetRenderFormatSize(_desc.format); // 픽셀당 4바이트 가정

	//		for (UINT y = 0; y < rowCount; ++y)
	//		{
	//			// 한 줄(RowSizeInBytes) 만큼만 복사
	//			memcpy(pDest, pSrc, rowSizeInBytes);

	//			// 다음 줄로 포인터 이동
	//			pDest += placedFootprint.Footprint.RowPitch; // 목적지는 RowPitch만큼 이동
	//			pSrc += rowSizeInBytes;            // 소스는 실제 데이터 크기만큼 이동
	//		}

	//		uploadBuffer->Unmap(0, &range);
	//		uploadBuffer->SetName(L"Check");

	//		CopyBufferToImage(uploadBuffer.Get(), texture->GetID3D12Resource(), { placedFootprint });

	//		D3D12_RESOURCE_BARRIER barrier = {};
	//		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//		barrier.Transition.pResource = texture->GetID3D12Resource();
	//		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	//		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	//		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//		TransitionResourceStateImmediate(barrier);

	//	}


	//	return texture;
	//}

	//Shared<Texture2D> D3D12RenderDevice::CreateEmptyTexture2D(const TextureDesc& _desc)
	//{
	//	Shared<D3D12Texture2D> texture = MakeShared<D3D12Texture2D>(this, _desc);

	//	D3D12_RESOURCE_BARRIER barrier = {};
	//	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//	barrier.Transition.pResource = texture->GetID3D12Resource();
	//	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	//	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	//	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//	TransitionResourceStateImmediate(barrier);
	//	return texture;
	//}

	//Shared<TextureCube> D3D12RenderDevice::CreateTextureCube(Array<const void*>& _imagePixels, const TextureDesc& _desc)
	//{
	//	auto texture = MakeShared<D3D12TextureCube>(this, _desc);

	//	UInt32 imageSize = _desc.width * _desc.height * 4 * 6;
	//	auto uploadBuffer = CreateBuffer(imageSize, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);

	//	D3D12_RESOURCE_DESC textureDesc = texture->GetID3D12Resource()->GetDesc();
	//	std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> layouts(6);
	//	UINT64 totalBytes;
	//	device->GetCopyableFootprints(&textureDesc, 0, 6, 0, layouts.data(), nullptr, nullptr, &totalBytes);

	//	UInt8* pixelData;
	//	uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pixelData));
	//	for (int i = 0; i < 6; ++i)
	//	{
	//		const void* srcPixels = _imagePixels[i]; // i번째 이미지 데이터 포인터
	//		const auto& destLayout = layouts[i];
	//		UINT rowPitch = _desc.width * 4; // 픽셀당 4바이트(R8G8B8A8)라고 가정

	//		// 메모리 레이아웃(row pitch)을 고려하여 한 줄씩 복사
	//		for (UINT y = 0; y < _desc.height; ++y)
	//		{
	//			// 목적지 주소 = 버퍼 시작 주소 + 현재 면의 오프셋 + 현재 y 위치 오프셋
	//			UInt8* dest = pixelData + destLayout.Offset + y * destLayout.Footprint.RowPitch;
	//			// 소스 주소 = i번째 이미지 데이터 시작 주소 + 현재 y 위치 오프셋
	//			const UINT8* src = static_cast<const UINT8*>(srcPixels) + y * rowPitch;

	//			memcpy(dest, src, rowPitch);
	//		}
	//	}
	//	uploadBuffer->Unmap(0, nullptr);

	//	CopyBufferToImage(uploadBuffer.Get(), texture->GetID3D12Resource(), layouts);

	//	D3D12_RESOURCE_BARRIER barrier = {};
	//	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//	barrier.Transition.pResource = texture->GetID3D12Resource();
	//	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	//	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	//	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//	TransitionResourceStateImmediate(barrier);

	//	return texture;
	//}

	//Shared<TextureCube> D3D12RenderDevice::CreateEmptyTextureCube(const TextureDesc& _desc)
	//{
	//	auto textureCube = MakeShared<D3D12TextureCube>(this, _desc);

	//	D3D12_RESOURCE_BARRIER barrier = {};
	//	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//	barrier.Transition.pResource = textureCube->GetID3D12Resource();
	//	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	//	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	//	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//	TransitionResourceStateImmediate(barrier);

	//	return textureCube;
	//}

	Shared<Sampler> D3D12RenderDevice::CreateSampler(const SamplerDesc& _desc)
	{
		return MakeShared<D3D12Sampler>(this, _desc);
	}

	Unique<ImGuiRenderer> D3D12RenderDevice::CreateImGuiRenderer()
	{
		return MakeUnique<D3D12ImGuiRenderer>(this);
	}


	//void D3D12RenderDevice::CopyTexture2D(Shared<Texture2D> _src, Shared<Texture2D> _dst)
	//{
	//	//D3D12Texture2D* src = (D3D12Texture2D*)_src.get();
	//	//D3D12Texture2D* dst = (D3D12Texture2D*)_dst.get();
	//	//TransitionResourceState(commandList.Get(), src->GetID3D12Resource(), src->GetCurrentState(),
	//	//	D3D12_RESOURCE_STATE_COPY_SOURCE);
	//	//src->SetCurrentState(D3D12_RESOURCE_STATE_COPY_SOURCE);
	//	//commandList->CopyResource(dst->GetID3D12Resource(), src->GetID3D12Resource());
	//	//dst->SetCurrentState(D3D12_RESOURCE_STATE_COPY_DEST);
	//}

	//void Daydream::D3D12RenderDevice::CopyTextureToCubemapFace(TextureCube* _dstCubemap, UInt32 _faceIndex, Texture2D* _srcTexture2D, UInt32 _mipLevel)
	//{
	//	//TransitionResourceState(commandList.Get(), _srcTexture2D, D3D12_RESOURCE_STATE_COPY_DEST,
	//	//	D3D12_RESOURCE_STATE_COPY_SOURCE);
	//	//TransitionResourceState(commandList.Get(), _dstCubemap, D3D12_RESOURCE_STATE_COPY_SOURCE,
	//	//	D3D12_RESOURCE_STATE_COPY_DEST);

	//	//D3D12Texture2D* src = (D3D12Texture2D*)_srcTexture2D;

	//	//D3D12TextureCube* dst = (D3D12TextureCube*)_dstCubemap;


	//	//TransitionResourceState(commandList.Get(), src->GetID3D12Resource(), src->GetCurrentState(),
	//	//	D3D12_RESOURCE_STATE_COPY_SOURCE);
	//	//src->SetCurrentState(D3D12_RESOURCE_STATE_COPY_SOURCE);
	//	//TransitionResourceState(commandList.Get(), dst->GetID3D12Resource(), dst->GetCurrentState(),
	//	//	D3D12_RESOURCE_STATE_COPY_DEST);
	//	//dst->SetCurrentState(D3D12_RESOURCE_STATE_COPY_DEST);

	//	//D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
	//	//srcLocation.pResource = src->GetID3D12Resource();
	//	//srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	//	//srcLocation.SubresourceIndex = 0;

	//	//D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
	//	//dstLocation.pResource = dst->GetID3D12Resource();
	//	//dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	//	//dstLocation.SubresourceIndex = _mipLevel + _faceIndex * dst->GetMipLevels();

	//	//// 3. 복사 명령을 기록합니다.
	//	//commandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);

	//	//TransitionResourceState(commandList.Get(), src->GetID3D12Resource(), src->GetCurrentState(),
	//	//	D3D12_RESOURCE_STATE_COMMON);
	//	//src->SetCurrentState(D3D12_RESOURCE_STATE_COMMON);
	//	//TransitionResourceState(commandList.Get(), dst->GetID3D12Resource(), dst->GetCurrentState(),
	//	//	D3D12_RESOURCE_STATE_COMMON);
	//	//dst->SetCurrentState(D3D12_RESOURCE_STATE_COMMON);
	//}


	void D3D12RenderDevice::ExecuteSingleTimeCommands(std::function<void(ID3D12GraphicsCommandList*)> commands)
	{
		// 1. Allocator와 Command List를 재사용하기 위해 리셋
		uploadCommandAllocator->Reset();
		uploadCommandList->Reset(uploadCommandAllocator.Get(), nullptr);

		// 2. 외부에서 전달받은 커맨드 기록 (람다 함수 사용)
		commands(uploadCommandList.Get());

		// 3. 커맨드 리스트 기록 종료
		uploadCommandList->Close();

		// 4. 커맨드 리스트를 큐에 제출하여 실행
		ID3D12CommandList* ppCommandLists[] = { uploadCommandList.Get() };
		commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		// 5. Fence를 사용하여 작업이 완료될 때까지 대기
		uploadFenceValue++;
		commandQueue->Signal(uploadFence.Get(), uploadFenceValue);

		if (uploadFence->GetCompletedValue() < uploadFenceValue)
		{
			uploadFence->SetEventOnCompletion(uploadFenceValue, uploadFenceEvent);
			WaitForSingleObject(uploadFenceEvent, INFINITE);
		}
	}
	ComPtr<ID3D12Resource> D3D12RenderDevice::CreateBuffer(UINT64 _size, D3D12_HEAP_TYPE _heapType, D3D12_RESOURCE_STATES _initialState, D3D12_RESOURCE_FLAGS _flags)
	{
		ComPtr<ID3D12Resource> buffer;

		D3D12_HEAP_PROPERTIES heapProperties{};
		heapProperties.Type = _heapType;
		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = _size;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = _flags;

		HRESULT hr = device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			_initialState,
			nullptr, // Clear value (버퍼에는 필요 없음)
			IID_PPV_ARGS(buffer.GetAddressOf()));

		return buffer;
	}

	void D3D12RenderDevice::CopyBuffer(ID3D12Resource* _src, ID3D12Resource* _dst, UInt32 _dataSize)
	{
		ExecuteSingleTimeCommands([&](ID3D12GraphicsCommandList* _commandList)
			{
				_commandList->CopyBufferRegion(
					_dst,
					0,
					_src,
					0,
					_dataSize
				);
			});
	}

	ComPtr<ID3D12Resource> D3D12RenderDevice::CreateTexture(const D3D12_RESOURCE_DESC& _desc, D3D12_RESOURCE_STATES _initialState)
	{
		ComPtr<ID3D12Resource> texture;

		D3D12_CLEAR_VALUE clearValue{};
		bool isUseClearValue = _desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET || _desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		if (isUseClearValue)
		{
			clearValue.Format = _desc.Format;
			if (_desc.Format == DXGI_FORMAT_D24_UNORM_S8_UINT)
			{
				clearValue.DepthStencil.Depth = 1.0f;
				clearValue.DepthStencil.Stencil = 0;
			}
			else
			{
				clearValue.Color[0] = 0.0f;
				clearValue.Color[1] = 0.0f;
				clearValue.Color[2] = 1.0f;
				clearValue.Color[3] = 1.0f;
			}
		}

		D3D12_HEAP_PROPERTIES heapProperties = {};
		heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

		device->CreateCommittedResource(
			&heapProperties,
			D3D12_HEAP_FLAG_NONE,
			&_desc,
			_initialState,
			nullptr,
			IID_PPV_ARGS(texture.GetAddressOf())
		);
		return texture;
	}


	void D3D12RenderDevice::CopyBufferToImage(ID3D12Resource* _src, ID3D12Resource* _dst, Array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> _subresourceFootprint)
	{
		ExecuteSingleTimeCommands([&](ID3D12GraphicsCommandList* _commandList)
			{
				for (UInt32 i = 0; i < _subresourceFootprint.size(); ++i)
				{
					D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
					srcLocation.pResource = _src;
					srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
					srcLocation.PlacedFootprint = _subresourceFootprint[i];

					D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
					dstLocation.pResource = _dst;
					dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
					dstLocation.SubresourceIndex = i;

					_commandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);
				}
			});
	}

	void D3D12RenderDevice::TransitionResourceState(ID3D12GraphicsCommandList* _commandList, ID3D12Resource* _resource, D3D12_RESOURCE_STATES _stateBefore, D3D12_RESOURCE_STATES _stateAfter)
	{
		if (_stateBefore == _stateAfter)
		{
			return;
		}

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = _resource;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = _stateBefore;
		barrier.Transition.StateAfter = _stateAfter;

		_commandList->ResourceBarrier(1, &barrier);
	}

	void D3D12RenderDevice::TransitionResourceStateImmediate(D3D12_RESOURCE_BARRIER _barrier)
	{
		ExecuteSingleTimeCommands([&](ID3D12GraphicsCommandList* _commandList)
			{
				_commandList->ResourceBarrier(1, &_barrier);
			}
		);
	}
}


