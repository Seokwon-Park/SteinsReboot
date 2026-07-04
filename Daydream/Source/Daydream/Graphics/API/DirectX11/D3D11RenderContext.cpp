#include "DaydreamPCH.h"
#include "D3D11RenderContext.h"
#include "D3D11RenderDevice.h"

#include "D3D11GraphicsPipelineState.h"
#include "D3D11Texture.h"
#include "D3D11TextureView.h"
#include "D3D11TextureCube.h"
#include "D3D11Buffer.h"
#include "D3D11Utility.h"

#include "Daydream/Graphics/Resources/Mesh.h"

namespace Daydream
{
	D3D11RenderContext::D3D11RenderContext(D3D11RenderDevice* _device)
	{
		device = _device;
	}

	void D3D11RenderContext::SetViewport(UInt32 _x, UInt32 _y, UInt32 _width, UInt32 _height)
	{
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = Cast<Float32>(_x);
		viewport.TopLeftY = Cast<Float32>(_y);
		viewport.Width = Cast<Float32>(_width);
		viewport.Height = Cast<Float32>(_height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;

		device->GetContext()->RSSetViewports(1, &viewport);
	}

	void D3D11RenderContext::DrawIndexed(UInt32 _indexCount, UInt32 _startIndex, UInt32 _baseVertex)
	{
		device->GetContext()->DrawIndexed(_indexCount, _startIndex, _baseVertex);
	}

	void D3D11RenderContext::BeginRendering(const RenderingInfo& _renderingInfo)
	{
		Array<ID3D11RenderTargetView*> rtvs;
		for (const AttachmentDesc& attachmentDesc : _renderingInfo.colorAttachments)
		{
			ClearValue rtvClearValue = attachmentDesc.clearValue;
			D3D11TextureView* d3d11TextureView = Cast<D3D11TextureView*>(attachmentDesc.view);
			if (attachmentDesc.loadOp == AttachmentLoadOp::Clear)
			{
				device->GetContext()->ClearRenderTargetView(d3d11TextureView->GetRTV(), &rtvClearValue.colorClearValue.color[0]);
			}
			rtvs.push_back(d3d11TextureView->GetRTV());
		}

		ID3D11DepthStencilView* dsv = nullptr;
		if (_renderingInfo.depthAttachment.view != nullptr)
		{
			ClearValue dsvClearValue = _renderingInfo.depthAttachment.clearValue;
			D3D11TextureView* d3d11TextureView = Cast<D3D11TextureView*>(_renderingInfo.depthAttachment.view);
			dsv = d3d11TextureView->GetDSV();
			if (_renderingInfo.depthAttachment.loadOp == AttachmentLoadOp::Clear)
			{
				device->GetContext()->ClearDepthStencilView(d3d11TextureView->GetDSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, dsvClearValue.depthClearValue, dsvClearValue.stencilClearValue);
			}
		}
		device->GetContext()->OMSetRenderTargets((UInt32)rtvs.size(), rtvs.data(), dsv);
		SetViewport(
			_renderingInfo.renderArea.x,
			_renderingInfo.renderArea.y,
			_renderingInfo.renderArea.width,
			_renderingInfo.renderArea.height
		);
	}

	void D3D11RenderContext::EndRendering(const RenderingInfo& _renderingInfo)
	{
		device->GetContext()->OMSetRenderTargets(0, nullptr, nullptr);

		auto bindingMap = currentGraphicsPipelineState->GetShaderPipeline()->GetShaderBindingMap();
		ID3D11ShaderResourceView* nullSRV = nullptr;
		for (auto [name, data] : bindingMap)
		{
			switch (data.shaderType)
			{
			case ShaderType::None:
				DAYDREAM_CORE_ASSERT(false, "ERROR");
				break;
			case ShaderType::Vertex:
			{
				device->GetContext()->VSSetShaderResources(data.binding, 1, &nullSRV);
				break;
			}
			case ShaderType::Hull:
				break;
			case ShaderType::Domain:
				break;
			case ShaderType::Geometry:
				break;
			case ShaderType::Pixel:
			{
				device->GetContext()->PSSetShaderResources(data.binding, 1, &nullSRV);
				break;
			}
			case ShaderType::Compute:
				break;
			default:
				break;
			}
		}
	}

	void D3D11RenderContext::BindPipelineState(const GraphicsPipelineState* _pipelineState)
	{
		RenderContext::BindPipelineState(_pipelineState);
		const D3D11GraphicsPipelineState* pso = Cast<const D3D11GraphicsPipelineState*>(currentGraphicsPipelineState);
		pso->BindPipelineState();
	}

	void D3D11RenderContext::BindVertexBuffer(const GPUBuffer* _vertexBuffer, UInt32 _stride)
	{
		UInt32 offset = 0;
		UInt32 stride = _stride;

		const D3D11GPUBuffer* vertexBuffer = Cast<const D3D11GPUBuffer*>(_vertexBuffer);
		DAYDREAM_CORE_ASSERT(vertexBuffer, "vertexBuffer is nullptr!");
		ID3D11Buffer* d3d11Buffer = vertexBuffer->GetID3D11Buffer();
		device->GetContext()->IASetVertexBuffers(0, 1, &d3d11Buffer, &stride, &offset);
	}
	void D3D11RenderContext::BindIndexBuffer(const GPUBuffer* _indexBuffer)
	{
		UInt32 offset = 0;
		const D3D11GPUBuffer* indexBuffer = Cast<const D3D11GPUBuffer*>(_indexBuffer);
		DAYDREAM_CORE_ASSERT(indexBuffer, "indexBuffer is nullptr!");
		device->GetContext()->IASetIndexBuffer(indexBuffer->GetID3D11Buffer(), DXGI_FORMAT_R32_UINT, offset);
	}

	//void D3D11RenderContext::SetTexture2D(const String& _name, Shared<Texture2D> _texture)
	//{
	//	RenderContext::SetTexture2D(_name, _texture);
	//	const ShaderReflectionData* resourceInfo = activePipelineState->GetBindingInfo(_name);
	//	if (resourceInfo == nullptr) return;

	//	//	DAYDREAM_CORE_ASSERT(device->GetAPI() == RendererAPIType::DirectX11, "Wrong API!");

	//	Shared<D3D11Texture2D> d3d11Texture = static_pointer_cast<D3D11Texture2D>(_texture);
	//	ID3D11ShaderResourceView* srv = d3d11Texture->GetSRV();
	//	ID3D11SamplerState* sampler = d3d11Texture->GetSampler();
	//	switch (resourceInfo->shaderType)
	//	{
	//	case Daydream::ShaderType::None:
	//		DAYDREAM_CORE_ASSERT(false, "ERROR");
	//		break;
	//	case Daydream::ShaderType::Vertex:
	//	{
	//		device->GetContext()->VSSetShaderResources(resourceInfo->binding, 1, &srv);
	//		device->GetContext()->VSSetSamplers(resourceInfo->binding, 1, &sampler);
	//		break;
	//	}
	//	case Daydream::ShaderType::Hull:
	//		break;
	//	case Daydream::ShaderType::Domain:
	//		break;
	//	case Daydream::ShaderType::Geometry:
	//		break;
	//	case Daydream::ShaderType::Pixel:
	//	{
	//		device->GetContext()->PSSetShaderResources(resourceInfo->binding, 1, &srv);
	//		device->GetContext()->PSSetSamplers(resourceInfo->binding, 1, &sampler);
	//		break;
	//	}
	//	case Daydream::ShaderType::Compute:
	//		break;
	//	default:
	//		break;
	//	}
	//}

	//void D3D11RenderContext::SetTextureCube(const String& _name, Shared<TextureCube> _textureCube)
	//{
	//	const ShaderReflectionData* bindingInfo = activePipelineState->GetBindingInfo(_name);
	//	if (bindingInfo == nullptr) return;
	//	//DAYDREAM_CORE_ASSERT(device->GetAPI() == RendererAPIType::DirectX11, "Wrong API!");
	//	Shared<D3D11TextureCube> d3d11Texture = static_pointer_cast<D3D11TextureCube>(_textureCube);
	//	ID3D11ShaderResourceView* srv = d3d11Texture->GetSRV();
	//	ID3D11SamplerState* sampler = d3d11Texture->GetSampler();
	//	switch (bindingInfo->shaderType)
	//	{
	//	case Daydream::ShaderType::None:
	//		DAYDREAM_CORE_ASSERT(false, "ERROR");
	//		break;
	//	case Daydream::ShaderType::Vertex:
	//	{
	//		device->GetContext()->VSSetShaderResources(bindingInfo->binding, 1, &srv);
	//		device->GetContext()->VSSetSamplers(bindingInfo->binding, 1, &sampler);
	//		break;
	//	}
	//	case Daydream::ShaderType::Hull:
	//		break;
	//	case Daydream::ShaderType::Domain:
	//		break;
	//	case Daydream::ShaderType::Geometry:
	//		break;
	//	case Daydream::ShaderType::Pixel:
	//	{
	//		device->GetContext()->PSSetShaderResources(bindingInfo->binding, 1, &srv);
	//		device->GetContext()->PSSetSamplers(bindingInfo->binding, 1, &sampler);
	//		break;
	//	}
	//	case Daydream::ShaderType::Compute:
	//		break;
	//	default:
	//		break;
	//	}
	//}

	void D3D11RenderContext::BindShaderResourceView(const String& _name, const TextureView* _textureView, const Sampler* _sampler)
	{
		const ShaderReflectionData* bindingInfo = currentGraphicsPipelineState->GetBindingInfo(_name);
		if (bindingInfo == nullptr) return;
		//DAYDREAM_CORE_ASSERT(device->GetAPI() == RendererAPIType::DirectX11, "Wrong API!");
		const D3D11TextureView* view = Cast<const D3D11TextureView*>(_textureView);
		ID3D11ShaderResourceView* srv = view->GetSRV();
		const D3D11Sampler* d3d11Sampler = Cast<const D3D11Sampler*>(_sampler);
		ID3D11SamplerState* samplerState = d3d11Sampler->GetID3D11SamplerState();
		switch (bindingInfo->shaderType)
		{
		case ShaderType::None:
			DAYDREAM_CORE_ASSERT(false, "ERROR");
			break;
		case ShaderType::Vertex:
		{
			device->GetContext()->VSSetShaderResources(bindingInfo->binding, 1, &srv);
			device->GetContext()->VSSetSamplers(bindingInfo->binding, 1, &samplerState);
			break;
		}
		case ShaderType::Hull:
			break;
		case ShaderType::Domain:
			break;
		case ShaderType::Geometry:
			break;
		case ShaderType::Pixel:
		{
			device->GetContext()->PSSetShaderResources(bindingInfo->binding, 1, &srv);
			device->GetContext()->PSSetSamplers(bindingInfo->binding, 1, &samplerState);
			break;
		}
		case ShaderType::Compute:
			break;
		default:
			break;
		}
	}

	void Daydream::D3D11RenderContext::BindConstantBuffer(const String& _name, const GPUBuffer* _buffer)
	{
		const ShaderReflectionData* resourceInfo = currentGraphicsPipelineState->GetBindingInfo(_name);
		if (resourceInfo == nullptr) return;
		DAYDREAM_CORE_ASSERT(device->GetAPI() == RendererAPIType::DirectX11, "Wrong API!");
		const D3D11GPUBuffer* constantBuffer = Cast<const D3D11GPUBuffer*>(_buffer);
		DAYDREAM_CORE_ASSERT(constantBuffer, "vertexBuffer is nullptr!");
		ID3D11Buffer* d3d11Buffer = constantBuffer->GetID3D11Buffer();
		switch (resourceInfo->shaderType)
		{
		case ShaderType::None:
			break;
		case ShaderType::Vertex:
			device->GetContext()->VSSetConstantBuffers(resourceInfo->binding, 1, &d3d11Buffer);
			break;
		case ShaderType::Hull:
			break;
		case ShaderType::Domain:
			break;
		case ShaderType::Geometry:
			break;
		case ShaderType::Pixel:
			device->GetContext()->PSSetConstantBuffers(resourceInfo->binding, 1, &d3d11Buffer);
			break;
		case ShaderType::Compute:
			break;
		default:
			break;
		}
	}



	void D3D11RenderContext::CopyBuffer(const GPUBuffer* _src, const GPUBuffer* _dst, UInt32 _copySize, UInt32 _srcOffset, UInt32 _dstOffset)
	{
		const D3D11GPUBuffer* src = Cast<const D3D11GPUBuffer*>(_src);
		const D3D11GPUBuffer* dst = Cast<const D3D11GPUBuffer*>(_dst);

		D3D11_BOX srcBox;
		srcBox.left = _srcOffset;
		srcBox.right = _srcOffset + _copySize;
		srcBox.top = 0; srcBox.bottom = 1; // 1D 버퍼이므로 Y, Z는 1로 고정
		srcBox.front = 0; srcBox.back = 1;

		device->GetContext()->CopySubresourceRegion(
			dst->GetID3D11Buffer(), 0,
			_dstOffset, 0, 0, // 대상 위치 (X, Y, Z)
			src->GetID3D11Buffer(), 0, &srcBox
		);
	}

	void D3D11RenderContext::CopyBufferToTexture(const GPUBuffer* _src, const GPUTexture* _dst)
	{
		D3D11_MAPPED_SUBRESOURCE mappedData;

		const D3D11GPUBuffer* src = Cast<const D3D11GPUBuffer*>(_src);
		const D3D11GPUTexture* dst = Cast<const D3D11GPUTexture*>(_dst);

		HRESULT hr = device->GetContext()->Map(src->GetID3D11Buffer(), 0, D3D11_MAP_READ, 0, &mappedData);
		DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to map source buffer for reading in DX11!");

		UInt32 bytesPerPixel = GraphicsUtility::GetRenderFormatSize(_dst->GetDesc().format);
		UInt32 rowPitch = _dst->GetWidth() * bytesPerPixel;
		UInt32 depthPitch = rowPitch * _dst->GetHeight();

		device->GetContext()->UpdateSubresource(dst->GetID3D11Resource(), 0, nullptr, mappedData.pData, rowPitch, depthPitch);

		device->GetContext()->Unmap(src->GetID3D11Buffer(), 0);
	}

	void D3D11RenderContext::CopyTexture(const GPUTexture* _src, const GPUTexture* _dst, const TextureCopyRegion& _region)
	{
		const D3D11GPUTexture* src = Cast<const D3D11GPUTexture*>(_src);
		const D3D11GPUTexture* dst = Cast<const D3D11GPUTexture*>(_dst);

		for (UInt32 i = 0; i < _region.srcSubresource.layerCount; ++i)
		{
			UINT srcSub = D3D11CalcSubresource(_region.srcSubresource.mipLevel, _region.srcSubresource.baseLayer + i, src->GetMipLevels());
			UINT dstSub = D3D11CalcSubresource(_region.dstSubresource.mipLevel, _region.dstSubresource.baseLayer + i, dst->GetMipLevels());

			D3D11_BOX srcBox;
			srcBox.left = _region.srcOffset[0];
			srcBox.top = _region.srcOffset[1];
			srcBox.front = _region.srcOffset[2];
			srcBox.right = srcBox.left + _region.extent[0];
			srcBox.bottom = srcBox.top + _region.extent[1];
			srcBox.back = srcBox.front + _region.extent[2];

			device->GetContext()->CopySubresourceRegion(
				dst->GetID3D11Resource(), dstSub,
				_region.dstOffset[0], _region.dstOffset[1], _region.dstOffset[2],
				src->GetID3D11Resource(), srcSub, &srcBox
			);
		}
	}

	void D3D11RenderContext::GenerateMips(GPUTexture* _texture)
	{
		const D3D11GPUTexture* texture = Cast<const D3D11GPUTexture*>(_texture);
		ComPtr<ID3D11ShaderResourceView> srv = nullptr;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = GraphicsUtility::DirectX::ConvertToDXGIFormat(texture->GetFormat());

		bool isArray = (_texture->GetDesc().type == TextureType::TextureCube || _texture->GetDesc().layerCount > 1);

		if (_texture->GetDesc().type == TextureType::TextureCube && _texture->GetLayerCount() == 6)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MostDetailedMip = 0;
			srvDesc.TextureCube.MipLevels = -1;
		}
		else if (isArray)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.MostDetailedMip = 0;
			srvDesc.Texture2DArray.MipLevels = -1;
			srvDesc.Texture2DArray.FirstArraySlice = 0;
			srvDesc.Texture2DArray.ArraySize = _texture->GetLayerCount();
		}
		else
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = -1;
		}

		device->GetDevice()->CreateShaderResourceView(texture->GetID3D11Resource(), &srvDesc, srv.GetAddressOf());

		if (srv)
		{
			device->GetContext()->GenerateMips(srv.Get());
		}
	}
}
