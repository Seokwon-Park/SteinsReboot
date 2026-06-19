#include "DaydreamPCH.h"
#include "D3D11RenderDevice.h"
#include "D3D11RenderContext.h"
#include "D3D11Buffer.h"
#include "D3D11Swapchain.h"
#include "D3D11Shader.h"
#include "D3D11VertexShader.h"
#include "D3D11GraphicsPipelineState.h"
#include "D3D11ImGuiRenderer.h"
#include "D3D11Texture.h"
#include "D3D11TextureCube.h"
#include "D3D11TextureView.h"
#include "D3D11Sampler.h"

#include "Daydream/Graphics/Utility/GraphicsUtility.h"

namespace Daydream
{
	D3D11RenderDevice::D3D11RenderDevice()
	{
		API = RendererAPIType::DirectX11;
	}

	D3D11RenderDevice::~D3D11RenderDevice()
	{
		//#if defined(DEBUG) || defined(_DEBUG)
		//		Microsoft::WRL::ComPtr<ID3D11Debug> DXGIDebug;
		//
		//		if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&DXGIDebug))))
		//		{
		//			DXGIDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
		//			DXGIDebug = nullptr;
		//		}
		//#endif
				/*dxgiAdapter->Release();
				dxgiDevice->Release();
				dxgiFactory->Release();*/
				//device.Reset();
	}

	void D3D11RenderDevice::Init()
	{
		UInt32 creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
		// If the project is in a debug build, enable debugging via SDK Layers.
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};


		D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;
		HRESULT hr = D3D11CreateDevice(
			nullptr,
			driverType,
			nullptr,
			creationFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			device.GetAddressOf(),
			&featureLevel,
			deviceContext.GetAddressOf());

		if (FAILED(hr))
		{
			DAYDREAM_CORE_ERROR("Failed to Create m_D3DDevice!");
		}

		device->QueryInterface(IID_PPV_ARGS(dxgiDevice.GetAddressOf()));
		dxgiDevice->GetParent(IID_PPV_ARGS(dxgiAdapter.GetAddressOf()));
		dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));

		//Here is For Get DX11 Version
		DXGI_ADAPTER_DESC adapterDescription; // Vendor
		SecureZeroMemory(&adapterDescription, sizeof(DXGI_ADAPTER_DESC));
		char videoCardDescription[128]; // Renderer
		LARGE_INTEGER driverVersion; // Version

		// Vendor
		dxgiAdapter->GetDesc(&adapterDescription);
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

		//dxgiFactory->Release();
		//dxgiAdapter->Release();
		//dxgiDevice->Release();

		////Set DX11 DebugMode
		//if (debugLayerEnabled)
		//{
		//	device->QueryInterface(IID_PPV_ARGS(debugLayer.GetAddressOf()));
		//	debugLayer->ReportLiveDeviceObjects(D3D11_RLDO_IGNORE_INTERNAL);
		//	ID3D11InfoQueue* infoQueue;
		//	device->QueryInterface(IID_PPV_ARGS(&infoQueue));
		//	D3D11_MESSAGE_ID hide[] = { D3D11_MESSAGE_ID_DEVICE_DRAW_SAMPLER_NOT_SET };
		//	D3D11_INFO_QUEUE_FILTER filter;
		//	memset(&filter, 0, sizeof(filter));
		//	filter.DenyList.NumIDs = 1;
		//	filter.DenyList.pIDList = hide;
		//	infoQueue->AddStorageFilterEntries(&filter);
		//}

		info.APIName = "DirectX11 Info:";
		info.vendor = GraphicsUtility::GetVendor(adapterDescription.VendorId);
		info.physicalDeviceInfo = videoCardDescription;
		info.version = version;
	}

	void D3D11RenderDevice::Shutdown()
	{
	}

	Unique<RenderContext> D3D11RenderDevice::CreateContext()
	{
		return MakeUnique<D3D11RenderContext>(this);
	}
	Shared<RenderCommandList> D3D11RenderDevice::CreateRenderCommandList()
	{
		return MakeShared<RenderCommandList>();
	}
	Shared<GPUBuffer> D3D11RenderDevice::CreateGPUBuffer(const BufferDesc& _desc)
	{
		return MakeShared<D3D11GPUBuffer>(this, _desc);
	}
	Shared<GPUTexture> D3D11RenderDevice::CreateGPUTexture(const TextureDesc& _desc)
	{
		return MakeShared<D3D11GPUTexture>(this, _desc);
	}
	Shared<TextureView> D3D11RenderDevice::CreateTextureView(Texture* _texture, const TextureViewDesc& _desc)
	{
		D3D11GPUTexture* texture = Cast<D3D11GPUTexture*>(_texture->GetGPUTexture());
		return MakeShared<D3D11TextureView>(this, texture, _desc);
	}
	//Shared<VertexBuffer> D3D11RenderDevice::CreateDynamicVertexBuffer(UInt32 _size, UInt32 _stride, UInt32 _initialDataSize, const void* _initialData)
	//{
	//	BufferDesc desc{};
	//	desc.bufferUsage = BufferUsage::Vertex;
	//	desc.memoryUsage = MemoryUsage::Dynamic;
	//	desc.size = _size;
	//	desc.initialData = _initialData;

	//	Shared<D3D11GPUBuffer> gpuBuffer = MakeShared<D3D11GPUBuffer>(this, desc);

	//	Shared<VertexBuffer> vertexBuffer = MakeShared<VertexBuffer>(gpuBuffer, _stride);
	//	if (_initialData != nullptr)
	//	{
	//		vertexBuffer->UpdateData(_initialData, _initialDataSize);
	//	}
	//	return vertexBuffer;
	//}

	//Shared<VertexBuffer> D3D11RenderDevice::CreateStaticVertexBuffer(UInt32 _size, UInt32 _stride, const void* _initialData)
	//{
	//	BufferDesc desc{};
	//	desc.bufferUsage = BufferUsage::Vertex;
	//	desc.memoryUsage = MemoryUsage::Static;
	//	desc.size = _size;
	//	desc.initialData = _initialData;

	//	Shared<D3D11GPUBuffer> gpuBuffer = MakeShared<D3D11GPUBuffer>(this, desc);

	//	return MakeShared<VertexBuffer>(gpuBuffer, _stride);
	//}

	//Shared<IndexBuffer> D3D11RenderDevice::CreateIndexBuffer(const UInt32* _indices, UInt32 _count)
	//{
	//	BufferDesc desc{};
	//	desc.bufferUsage = BufferUsage::Index;
	//	desc.memoryUsage = MemoryUsage::Static;
	//	desc.size = sizeof(UInt32) * _count;
	//	desc.initialData = _indices;

	//	Shared<D3D11GPUBuffer> gpuBuffer = MakeShared<D3D11GPUBuffer>(this, desc);

	//	return  MakeShared<IndexBuffer>(gpuBuffer, _count);
	//}

	//Shared<ConstantBuffer> D3D11RenderDevice::CreateConstantBuffer(UInt32 _size)
	//{
	//	BufferDesc desc{};
	//	desc.bufferUsage = BufferUsage::Constant;
	//	desc.memoryUsage = MemoryUsage::Dynamic;
	//	desc.size = _size;

	//	Shared<D3D11GPUBuffer> gpuBuffer = MakeShared<D3D11GPUBuffer>(this, desc);

	//	return MakeShared<ConstantBuffer>(gpuBuffer);
	//}

	//Shared<RenderPass> D3D11RenderDevice::CreateRenderPass(const RenderPassDesc& _desc)
	//{
	//	return MakeShared<D3D11RenderPass>(this, _desc);
	//}

	//Shared<Framebuffer> D3D11RenderDevice::CreateFramebuffer(Shared<RenderPass> _renderPass, const FramebufferDesc& _desc)
	//{
	//	return _renderPass->CreateFramebuffer(_desc);
	//}

	Shared<GraphicsPipelineState> D3D11RenderDevice::CreatePipelineState(const GraphicsPipelineStateDesc& _desc)
	{
		return MakeShared<D3D11GraphicsPipelineState>(this, _desc);
	}

	Shared<Shader> D3D11RenderDevice::CreateShader(const ShaderType& _type)
	{
		switch (_type)
		{
		case ShaderType::None:  return nullptr;
		case ShaderType::Vertex: return MakeShared<D3D11VertexShader>(this);
		case ShaderType::Hull: return nullptr;
		case ShaderType::Domain: return nullptr;
		case ShaderType::Geometry: return nullptr;
		case ShaderType::Pixel: return MakeShared<D3D11PixelShader>(this);
		case ShaderType::Compute: return nullptr;
		default:
			break;
		}
		return Shared<Shader>();
	}

	Shared<Swapchain> D3D11RenderDevice::CreateSwapchain(const DaydreamWindow& _window, const SwapchainDesc& _desc)
	{
		return MakeShared<D3D11Swapchain>(this, _window, _desc);
	}

	//Shared<Texture2D> D3D11RenderDevice::CreateTexture2D(const void* _imageData, const TextureDesc& _desc)
	//{
	//	//return MakeShared<D3D11Texture2D>(this, _path, _desc);
	//	return MakeShared<D3D11Texture2D>(this, _desc, _imageData);
	//}

	//Shared<Texture2D> D3D11RenderDevice::CreateEmptyTexture2D(const TextureDesc& _desc)
	//{
	//	return MakeShared<D3D11Texture2D>(this, _desc);
	//}

	//Shared<TextureCube> D3D11RenderDevice::CreateTextureCube(Array<const void*>& _imageDatas, const TextureDesc& _desc)
	//{
	//	return MakeShared<D3D11TextureCube>(this, _desc, _imageDatas);
	//}

	//Shared<TextureCube> D3D11RenderDevice::CreateTextureCube(const Array<Shared<Texture2D>>& _textures, const TextureDesc& _desc)
	//{
	//	Array<const void*> dummy;
	//	Shared<D3D11TextureCube> textureCube = MakeShared<D3D11TextureCube>(this, _desc, dummy);
	//	for (int i = 0; i < _textures.size(); ++i)
	//	{
	//		CopyTextureToCubemapFace(
	//			textureCube.get(),   // 대상: 큐브맵 리소스
	//			i,                             // 대상 면 인덱스
	//			_textures[i].get()// 원본: Texture2D 리소스
	//		);
	//	}
	//	return textureCube;
	//}

	//Shared<TextureCube> D3D11RenderDevice::CreateEmptyTextureCube(const TextureDesc& _desc)
	//{
	//	Array<const void*> dummy;
	//	auto textureCube = MakeShared<D3D11TextureCube>(this, _desc, dummy);
	//	return textureCube;
	//}

	Shared<Sampler> D3D11RenderDevice::CreateSampler(const SamplerDesc& _desc)
	{
		return MakeShared<D3D11Sampler>(this, _desc);
	}

	Unique<ImGuiRenderer> D3D11RenderDevice::CreateImGuiRenderer()
	{
		return MakeUnique<D3D11ImGuiRenderer>(this);
	}
}