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
	}

	void D3D11RenderContext::BindPipelineState(Shared<GraphicsPipelineState> _pipelineState)
	{
		RenderContext::BindPipelineState(_pipelineState);
		D3D11GraphicsPipelineState* pso = Cast<D3D11GraphicsPipelineState*>(currentGraphicsPipelineState.get());
		pso->BindPipelineState();
	}
	void D3D11RenderContext::BindVertexBuffer(Shared<VertexBuffer> _vertexBuffer)
	{
		UInt32 offset = 0;
		UInt32 stride = _vertexBuffer->GetStride();

		D3D11GPUBuffer* vertexBuffer = Cast<D3D11GPUBuffer*>(_vertexBuffer->GetGPUBufferPtr());
		DAYDREAM_CORE_ASSERT(vertexBuffer, "vertexBuffer is nullptr!");
		ID3D11Buffer* d3d11Buffer = vertexBuffer->GetID3D11Buffer();
		device->GetContext()->IASetVertexBuffers(0, 1, &d3d11Buffer, &stride, &offset);
	}
	void D3D11RenderContext::BindIndexBuffer(Shared<IndexBuffer> _indexBuffer)
	{
		UInt32 offset = 0;
		D3D11GPUBuffer* indexBuffer = Cast<D3D11GPUBuffer*>(_indexBuffer->GetGPUBufferPtr());
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

	void D3D11RenderContext::BindShaderResourceView(const String& _name, Shared<TextureView> _textureView, Shared<Sampler> _sampler)
	{
		const ShaderReflectionData* bindingInfo = currentGraphicsPipelineState->GetBindingInfo(_name);
		if (bindingInfo == nullptr) return;
		//DAYDREAM_CORE_ASSERT(device->GetAPI() == RendererAPIType::DirectX11, "Wrong API!");
		D3D11TextureView* view = Cast<D3D11TextureView*>(_textureView.get());
		ID3D11ShaderResourceView* srv = view->GetSRV();
		D3D11Sampler* d3d11Sampler = Cast<D3D11Sampler*>(_sampler.get());
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

	void D3D11RenderContext::SetConstantBuffer(const String& _name, Shared<ConstantBuffer> _buffer)
	{
		const ShaderReflectionData* resourceInfo = currentGraphicsPipelineState->GetBindingInfo(_name);
		if (resourceInfo == nullptr) return;
		DAYDREAM_CORE_ASSERT(device->GetAPI() == RendererAPIType::DirectX11, "Wrong API!");
		D3D11GPUBuffer* constantBuffer = Cast<D3D11GPUBuffer*>(_buffer->GetGPUBufferPtr());
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

	void D3D11RenderContext::CopyTexture2D(Shared<Texture2D> _src, Shared<Texture2D> _dst)
	{
		D3D11GPUTexture* dst = Cast<D3D11GPUTexture*>(_dst->GetGPUTexturePtr());
		D3D11GPUTexture* src = Cast<D3D11GPUTexture*>(_src->GetGPUTexturePtr());

		device->GetContext()->CopyResource(dst->GetID3D11Resource(), src->GetID3D11Resource());
	}
	void Daydream::D3D11RenderContext::CopyTextureToCubemapFace(Shared<Texture2D> _srcTexture2D, Shared<TextureCube> _dstCubemap, UInt32 _faceIndex, UInt32 _mipLevel)
	{
		D3D11GPUTexture* src = Cast<D3D11GPUTexture*>(_srcTexture2D->GetGPUTexturePtr());
		D3D11GPUTexture* dst = Cast<D3D11GPUTexture*>(_dstCubemap->GetGPUTexturePtr());


		UInt32 dstSubresourceIndex = D3D11CalcSubresource(
			_mipLevel,
			_faceIndex,
			dst->GetDesc().mipLevels
		);

		UInt32 srcSubresourceIndex = 0;

		device->GetContext()->CopySubresourceRegion(
			dst->GetID3D11Resource(),           // 대상 리소스
			dstSubresourceIndex,  // 대상 Subresource
			0, 0, 0,              // 대상 좌표 (x, y, z)
			src->GetID3D11Resource(),         // 원본 리소스
			srcSubresourceIndex,  // 원본 Subresource
			nullptr               // 원본 영역 (nullptr은 전체를 의미)
		);
	}
	void D3D11RenderContext::CopyTextureCubeToTexture2D(Shared<TextureCube> _srcCubemap, UInt32 _faceIndex, Shared<Texture2D> _dstTexture2D, UInt32 _mipLevel)
	{
		D3D11GPUTexture* src = Cast<D3D11GPUTexture*>(_srcCubemap->GetGPUTexturePtr());
		D3D11GPUTexture* dst = Cast<D3D11GPUTexture*>(_dstTexture2D->GetGPUTexturePtr());

		UInt32 srcSubresourceIndex = D3D11CalcSubresource(
			_mipLevel,
			_faceIndex,
			src->GetDesc().mipLevels
		);

		UInt32 dstSubresourceIndex = 0;

		device->GetContext()->CopySubresourceRegion(
			dst->GetID3D11Resource(),           // 대상 리소스
			dstSubresourceIndex,  // 대상 Subresource
			0, 0, 0,              // 대상 좌표 (x, y, z)
			src->GetID3D11Resource(),         // 원본 리소스
			srcSubresourceIndex,  // 원본 Subresource
			nullptr               // 원본 영역 (nullptr은 전체를 의미)
		);

	}
	void D3D11RenderContext::CopyBuffer(Shared<GPUBuffer> _src, Shared<GPUBuffer> _dst, UInt32 _copySize)
	{
		D3D11GPUBuffer* src = Cast<D3D11GPUBuffer*>(_src.get());
		D3D11GPUBuffer* dst = Cast<D3D11GPUBuffer*>(_dst.get());

		device->GetContext()->CopyResource(dst->GetID3D11Buffer(), src->GetID3D11Buffer());
	}

	void D3D11RenderContext::CopyBufferToTexture(Shared<GPUBuffer> _src, Shared<GPUTexture> _dst)
	{
		D3D11_MAPPED_SUBRESOURCE mappedData;

		D3D11GPUBuffer* src = Cast<D3D11GPUBuffer*>(_src.get());
		D3D11GPUTexture* dst = Cast<D3D11GPUTexture*>(_dst.get());

		HRESULT hr = device->GetContext()->Map(src->GetID3D11Buffer(), 0, D3D11_MAP_READ, 0, &mappedData);
		DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to map source buffer for reading in DX11!");

		UInt32 bytesPerPixel = GraphicsUtility::GetRenderFormatSize(_dst->GetDesc().format);
		UInt32 rowPitch = _dst->GetWidth() * bytesPerPixel;
		UInt32 depthPitch = rowPitch * _dst->GetHeight();

		device->GetContext()->UpdateSubresource(dst->GetID3D11Resource(), 0, nullptr, mappedData.pData, rowPitch, depthPitch);

		device->GetContext()->Unmap(src->GetID3D11Buffer(), 0);
	}

	void D3D11RenderContext::GenerateMips(Shared<Texture> _texture)
	{
		D3D11GPUTexture* texture = Cast<D3D11GPUTexture*>(_texture->GetGPUTexturePtr());
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
