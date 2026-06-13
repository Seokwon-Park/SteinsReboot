#include "DaydreamPCH.h"
#include "D3D12TextureView.h"

#include "D3D12Utility.h"

namespace Daydream
{
	D3D12TextureView::D3D12TextureView(D3D12RenderDevice* _device, D3D12GPUTexture* _texture, const TextureViewDesc& _desc)
		:TextureView(_texture, _desc)
	{
		ID3D12Device* device = _device->GetDevice();
		ID3D12Resource* resource = _texture->GetID3D12Resource();

		RenderFormat format = (_desc.format == RenderFormat::UNKNOWN) ? _texture->GetDesc().format : _desc.format;
		DXGI_FORMAT dxgiFormat = GraphicsUtility::DirectX::ConvertToDXGIFormat(format);
		

		bool isArray = (_texture->GetDesc().type == TextureType::TextureCube || _texture->GetDesc().layerCount > 1);

		switch (_desc.type)
		{
		case TextureViewType::ShaderResource:
		{
			if (GraphicsUtility::IsDepthFormat(format))
			{
				dxgiFormat = GraphicsUtility::DirectX::ConvertToDepthSRVFormat(format);
			}
			else if (GraphicsUtility::IsStencilFormat(format))
			{
				dxgiFormat = GraphicsUtility::DirectX::ConvertToStencilSRVFormat(format);
			}

			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = dxgiFormat;
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

			if (_texture->GetDesc().type == TextureType::TextureCube && _desc.layerCount == 6)
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
				srvDesc.TextureCube.MostDetailedMip = _desc.baseMip;
				srvDesc.TextureCube.MipLevels = _desc.mipLevels;
			}
			else if (isArray)
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
				srvDesc.Texture2DArray.MostDetailedMip = _desc.baseMip;
				srvDesc.Texture2DArray.MipLevels = _desc.mipLevels;
				srvDesc.Texture2DArray.FirstArraySlice = _desc.baseLayer;
				srvDesc.Texture2DArray.ArraySize = _desc.layerCount;
			}
			else
			{
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = _desc.baseMip;
				srvDesc.Texture2D.MipLevels = _desc.mipLevels;
			}

			// Allocate descriptor and create view
			_device->GetCBVSRVUAVHeapAlloc().Alloc(&cpuHandle, &gpuHandle);
			device->CreateShaderResourceView(resource, &srvDesc, cpuHandle);
			break;
		}
		case TextureViewType::RenderTarget:
		{
			D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
			rtvDesc.Format = dxgiFormat;

			if (isArray)
			{
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
				rtvDesc.Texture2DArray.MipSlice = _desc.baseMip;
				rtvDesc.Texture2DArray.FirstArraySlice = _desc.baseLayer;
				rtvDesc.Texture2DArray.ArraySize = _desc.layerCount;
			}
			else
			{
				rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				rtvDesc.Texture2D.MipSlice = _desc.baseMip;
			}

			// RTV heap allocation
			_device->GetRTVHeapAlloc().Alloc(&cpuHandle);
			device->CreateRenderTargetView(resource, &rtvDesc, cpuHandle);
			break;
		}
		case TextureViewType::DepthStencil:
		{
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
			dsvDesc.Format = GraphicsUtility::DirectX::ConvertToDSVFormat(format);
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

			if (isArray)
			{
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
				dsvDesc.Texture2DArray.MipSlice = _desc.baseMip;
				dsvDesc.Texture2DArray.FirstArraySlice = _desc.baseLayer;
				dsvDesc.Texture2DArray.ArraySize = _desc.layerCount;
			}
			else
			{
				dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsvDesc.Texture2D.MipSlice = _desc.baseMip;
			}

			// DSV heap allocation
			_device->GetDSVHeapAlloc().Alloc(&cpuHandle);
			device->CreateDepthStencilView(resource, &dsvDesc, cpuHandle);
			break;
		}
		case TextureViewType::UnorderedAccess:
		{
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
			uavDesc.Format = dxgiFormat;

			if (isArray)
			{
				uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
				uavDesc.Texture2DArray.MipSlice = _desc.baseMip;
				uavDesc.Texture2DArray.FirstArraySlice = _desc.baseLayer;
				uavDesc.Texture2DArray.ArraySize = _desc.layerCount;
			}
			else
			{
				uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
				uavDesc.Texture2D.MipSlice = _desc.baseMip;
			}

			// Allocate descriptor and create view
			_device->GetCBVSRVUAVHeapAlloc().Alloc(&cpuHandle, &gpuHandle);
			device->CreateUnorderedAccessView(resource, nullptr, &uavDesc, cpuHandle);
			break;
		}

		default:
		{
			DAYDREAM_CORE_ASSERT(false, "Unknown TextureViewType!");
			break;
		}
		}
	}
}


