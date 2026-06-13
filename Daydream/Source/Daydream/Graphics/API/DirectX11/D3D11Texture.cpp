#include "DaydreamPCH.h"
#include "D3D11Texture.h"

#include "D3D11Sampler.h"
#include "D3D11Utility.h"
#include "Daydream/Graphics/Utility/ImageLoader.h"

namespace Daydream
{
	D3D11GPUTexture::D3D11GPUTexture(D3D11RenderDevice* _device, const TextureDesc& _desc)
		:GPUTexture(_desc)
	{
		DXGI_FORMAT format = GraphicsUtility::DirectX::ConvertToDXGIFormat(_desc.format);
		UInt32 bindFlags = GraphicsUtility::DirectX11::ConvertToD3D11TextureBindFlags(_desc.textureUsage);

		switch (_desc.type)
		{
		case TextureType::Unknown:
			break;
		case TextureType::Texture1D:
		case TextureType::Texture1DArray:
		{
			D3D11_TEXTURE1D_DESC textureDesc{};
			textureDesc.Width = _desc.width;
			textureDesc.MipLevels = _desc.mipLevels;
			textureDesc.ArraySize = (_desc.type == TextureType::Texture1DArray) ? _desc.layerCount : 1;
			textureDesc.Format = format;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = bindFlags;

			ComPtr<ID3D11Texture1D> texture1D;
			HRESULT hr = _device->GetDevice()->CreateTexture1D(&textureDesc, nullptr, texture1D.GetAddressOf());
			DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create Texture1D!");

			texture = texture1D; // ComPtr 업캐스팅
			break;
		}
		case TextureType::Texture2D:
		case TextureType::Texture2DArray:
		case TextureType::Texture2DMultisample:
		case TextureType::TextureCube:
		case TextureType::TextureCubeArray:
		{
			D3D11_TEXTURE2D_DESC textureDesc{};
			textureDesc.Width = _desc.width;
			textureDesc.Height = _desc.height;
			textureDesc.Format = format;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = bindFlags;
			textureDesc.MipLevels = (_desc.type == TextureType::Texture2DMultisample) ? 1 : _desc.mipLevels;
			if (_desc.type == TextureType::TextureCube)
				textureDesc.ArraySize = 6;
			else if (_desc.type == TextureType::TextureCubeArray)
				textureDesc.ArraySize = 6 * _desc.layerCount;
			else if (_desc.type == TextureType::Texture2DArray)
				textureDesc.ArraySize = _desc.layerCount;
			else
				textureDesc.ArraySize = 1; // 일반 2D 또는 2DMultisample

			// MSAA 셋팅
			if (_desc.type == TextureType::Texture2DMultisample)
			{
				textureDesc.SampleDesc.Count = _desc.sampleCount;
				textureDesc.SampleDesc.Quality = 0; // 일반적인 기본값
			}
			else
			{
				textureDesc.SampleDesc.Count = 1;
				textureDesc.SampleDesc.Quality = 0;
			}

			if (_desc.type == TextureType::TextureCube || _desc.type == TextureType::TextureCubeArray)
				textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;

			if (_desc.mipLevels != 1)
			{
				// GenerateMips를 쓰기 위해 필요한 플래그
				textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
				textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
			}

			ComPtr<ID3D11Texture2D> texture2D;
			HRESULT hr = _device->GetDevice()->CreateTexture2D(&textureDesc, nullptr, texture2D.GetAddressOf());
			DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create Texture2D!");

			texture = texture2D;
			break;

		}
		case TextureType::Texture3D:
		{
			D3D11_TEXTURE3D_DESC textureDesc{};
			textureDesc.Width = _desc.width;
			textureDesc.Height = _desc.height;
			textureDesc.Depth = _desc.layerCount;
			textureDesc.MipLevels = _desc.mipLevels;
			textureDesc.Format = format;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = bindFlags;

			ComPtr<ID3D11Texture3D> texture3D;
			HRESULT hr = _device->GetDevice()->CreateTexture3D(&textureDesc, nullptr, texture3D.GetAddressOf());
			DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create Texture3D!");

			texture = texture3D;
			break;
		}
		default:
			break;
		}
	}

	D3D11GPUTexture::D3D11GPUTexture(D3D11RenderDevice* _device, const TextureDesc& _desc, ComPtr<ID3D11Texture2D> _d3d11BackBuffer)
		:GPUTexture(_desc)
	{
		texture = _d3d11BackBuffer;
	}


	//D3D11Texture2D::D3D11Texture2D(D3D11RenderDevice* _device, const TextureDesc& _desc, const void* _initialData)
	//	:Texture2D(_desc)
	//{
	//	device = _device;

	//	DXGI_FORMAT textureFormat = GraphicsUtility::DirectX::ConvertToDXGIFormat(_desc.format);
	//	DXGI_FORMAT srvFormat = textureFormat;
	//	DXGI_FORMAT dsvFormat = textureFormat;

	//	D3D11_TEXTURE2D_DESC textureDesc = {};
	//	textureDesc.Width = textureDesc.width;
	//	textureDesc.Height = textureDesc.height;
	//	textureDesc.MipLevels = 1;
	//	textureDesc.ArraySize = 1;
	//	textureDesc.Format = textureFormat;
	//	textureDesc.SampleDesc.Count = 1;
	//	textureDesc.SampleDesc.Quality = 0;
	//	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	//	textureDesc.BindFlags = GraphicsUtility::DirectX11::ConvertToD3D11BindFlags(_desc.textureUsage);
	//	textureDesc.CPUAccessFlags = 0;

	//	HRESULT hr;
	//	if (_initialData)
	//	{
	//		D3D11_SUBRESOURCE_DATA pixelData{};
	//		pixelData.pSysMem = _initialData;
	//		pixelData.SysMemPitch = textureDesc.Width * GraphicsUtility::GetRenderFormatSize(_desc.format);

	//		hr = device->GetDevice()->CreateTexture2D(&textureDesc, &pixelData, texture.GetAddressOf());
	//	}
	//	else
	//	{
	//		hr = device->GetDevice()->CreateTexture2D(&textureDesc, nullptr, texture.GetAddressOf());
	//	} 
	//	DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create Texture!");


	//	if (textureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL && textureDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	//	{
	//		srvFormat = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	//		dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//	}

	//	//D3D11_SAMPLER_DESC samplerDesc;
	//	//ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	//	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//	//samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//	//samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//	//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//	//samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//	//samplerDesc.MinLOD = 0;
	//	//samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//	//hr = device->GetDevice()->CreateSamplerState(&samplerDesc, textureSampler.GetAddressOf());
	//	//DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create Sampler!");

	//	if (textureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
	//	{
	//		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	//		dsvDesc.Format = dsvFormat;
	//		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	//		dsvDesc.Flags = 0;

	//		device->GetDevice()->CreateDepthStencilView(texture.Get(), &dsvDesc, views.dsv.GetAddressOf());
	//		DAYDREAM_CORE_ASSERT(views.dsv, "This texture was not created with the Depth Stencil View (DSV) bind flag.");
	//	}

	//	if (textureDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	//	{
	//		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	//		srvDesc.Format = srvFormat;
	//		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//		srvDesc.Texture2D.MostDetailedMip = 0;
	//		srvDesc.Texture2D.MipLevels = 1;

	//		device->GetDevice()->CreateShaderResourceView(texture.Get(), &srvDesc, views.srv.GetAddressOf());
	//		DAYDREAM_CORE_ASSERT(views.srv, "This texture was not created with the Shader Resource View (SRV) bind flag.");
	//	}

	//	if (textureDesc.BindFlags & D3D11_BIND_RENDER_TARGET)
	//	{
	//		device->GetDevice()->CreateRenderTargetView(texture.Get(), nullptr, views.rtv.GetAddressOf());
	//		DAYDREAM_CORE_ASSERT(views.rtv, "This texture was not created with the Render Target View (RTV) bind flag.");

	//	}

	//	if (textureDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
	//	{
	//		device->GetDevice()->CreateUnorderedAccessView(texture.Get(), nullptr, views.uav.GetAddressOf());
	//		DAYDREAM_CORE_ASSERT(views.uav, "This texture was not created with the Unordered Access View (UAV) bind flag.");
	//	}
	//}

	//D3D11Texture2D::~D3D11Texture2D()
	//{
	//	device = nullptr;
	//	textureSampler = nullptr;
	//	texture.Reset();
	//	views.rtv.Reset();
	//	views.dsv.Reset();
	//	views.srv.Reset();
	//	views.uav.Reset();
	//}


	//void D3D11Texture2D::SetSampler(Shared<Sampler> _sampler)
	//{
	//	textureSampler = static_cast<D3D11Sampler*>(_sampler.get());
	//}

}
