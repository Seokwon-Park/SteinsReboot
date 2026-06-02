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
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, FALSE); 

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

	UInt32 D3D12RenderDevice::GetAlignedRowPitch(UInt32 _width, RenderFormat _format) const
	{
		return (_width * GraphicsUtility::GetRenderFormatSize(_format) + 255) & ~255;
	}	
}


