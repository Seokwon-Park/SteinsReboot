#include "DaydreamPCH.h"
#include "D3D11TextureView.h"
#include "D3D11Utility.h"

namespace Daydream
{
	D3D11TextureView::D3D11TextureView(D3D11RenderDevice* _device, D3D11GPUTexture* _texture, const TextureViewDesc& _desc)
		:TextureView(_texture, _desc)
	{
        ID3D11Device* d3dDevice = _device->GetDevice();
        ID3D11Resource* d3dResource = _texture->GetID3D11Resource();

        RenderFormat targetFormat = (_desc.format == RenderFormat::UNKNOWN) ? _texture->GetDesc().format : _desc.format;
        DXGI_FORMAT dxgiFormat = GraphicsUtility::DirectX::ConvertToDXGIFormat(targetFormat); 

        bool isArray = (_texture->GetDesc().type == TextureType::TextureCube || _texture->GetDesc().layerCount > 1);

        switch (_desc.type)
        {
        case TextureViewType::ShaderResource:
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
            srvDesc.Format = dxgiFormat;

            if (_texture->GetDesc().type == TextureType::TextureCube && _desc.layerCount == 6)
            {
                srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
                srvDesc.TextureCube.MostDetailedMip = _desc.baseMip;
                srvDesc.TextureCube.MipLevels = _desc.mipLevels;
            }
            else if (isArray)
            {
                srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
                srvDesc.Texture2DArray.MostDetailedMip = _desc.baseMip;
                srvDesc.Texture2DArray.MipLevels = _desc.mipLevels;
                srvDesc.Texture2DArray.FirstArraySlice = _desc.baseLayer;
                srvDesc.Texture2DArray.ArraySize = _desc.layerCount;
            }
            else
            {
                srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                srvDesc.Texture2D.MostDetailedMip = _desc.baseMip;
                srvDesc.Texture2D.MipLevels = _desc.mipLevels;
            }

            ComPtr<ID3D11ShaderResourceView> srv = nullptr;
            HRESULT hr = d3dDevice->CreateShaderResourceView(d3dResource, &srvDesc, srv.GetAddressOf());
            DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create D3D11 Shader Resource View!");
            view = std::move(srv);
            break;
        }

        case TextureViewType::RenderTarget:
        {
            D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
            rtvDesc.Format = dxgiFormat;

            if (isArray)
            {
                rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
                rtvDesc.Texture2DArray.MipSlice = _desc.baseMip;
                rtvDesc.Texture2DArray.FirstArraySlice = _desc.baseLayer;
                rtvDesc.Texture2DArray.ArraySize = _desc.layerCount;
            }
            else
            {
                rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
                rtvDesc.Texture2D.MipSlice = _desc.baseMip;
            }

            ComPtr<ID3D11RenderTargetView> rtv = nullptr;
            HRESULT hr = d3dDevice->CreateRenderTargetView(d3dResource, &rtvDesc, rtv.GetAddressOf());
            DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create D3D11 Render Target View!");
            view = std::move(rtv);
            break;
        }

        case TextureViewType::DepthStencil:
        {
            D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
            dsvDesc.Format = dxgiFormat;

            if (isArray)
            {
                dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
                dsvDesc.Texture2DArray.MipSlice = _desc.baseMip;
                dsvDesc.Texture2DArray.FirstArraySlice = _desc.baseLayer;
                dsvDesc.Texture2DArray.ArraySize = _desc.layerCount;
            }
            else
            {
                dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
                dsvDesc.Texture2D.MipSlice = _desc.baseMip;
            }

            ComPtr<ID3D11DepthStencilView> dsv = nullptr;
            HRESULT hr = d3dDevice->CreateDepthStencilView(d3dResource, &dsvDesc, dsv.GetAddressOf());
            DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create D3D11 Depth Stencil View!");
            view = std::move(dsv);
            break;
        }

        case TextureViewType::UnorderedAccess:
        {
            D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
            uavDesc.Format = dxgiFormat;

            if (isArray)
            {
                uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
                uavDesc.Texture2DArray.MipSlice = _desc.baseMip;
                uavDesc.Texture2DArray.FirstArraySlice = _desc.baseLayer;
                uavDesc.Texture2DArray.ArraySize = _desc.layerCount;
            }
            else
            {
                uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
                uavDesc.Texture2D.MipSlice = _desc.baseMip;
            }

            ComPtr<ID3D11UnorderedAccessView> uav = nullptr;
            HRESULT hr = d3dDevice->CreateUnorderedAccessView(d3dResource, &uavDesc, uav.GetAddressOf());
            DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create D3D11 Unordered Access View!");
            view = std::move(uav);
            break;
        }

        default:
            DAYDREAM_CORE_ASSERT(false, "Unknown TextureViewType!");
            break;
        }

	}
    D3D11TextureView::~D3D11TextureView()
    {
        view = nullptr;
    }
}

