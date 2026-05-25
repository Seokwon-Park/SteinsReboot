#include "DaydreamPCH.h"
#include "D3D12RenderContext.h"

#include "D3D12GraphicsPipelineState.h"
#include "D3D12Texture.h"
#include "D3D12TextureView.h"
#include "D3D12TextureCube.h"
#include "D3D12Buffer.h"
#include "D3D12Framebuffer.h"
#include "D3D12Utility.h"
#include "Daydream/Graphics/Resources/Mesh.h"
#include "Daydream/Graphics/Manager/ResourceManager.h"
#include "Daydream/Graphics/Core/Renderer.h"

namespace Daydream
{
	D3D12RenderContext::D3D12RenderContext(D3D12RenderDevice* _device)
	{
		device = _device;
	}
	void D3D12RenderContext::SetViewport(UInt32 _x, UInt32 _y, UInt32 _width, UInt32 _height)
	{
		D3D12_RECT rect;
		rect.left = _x;
		rect.top = _y;
		rect.right = _width;
		rect.bottom = _height;

		GetD3D12ActiveCommandList()->RSSetScissorRects(1, &rect);

		D3D12_VIEWPORT viewport = {};
		viewport.Width = Cast<Float32>(_width);
		viewport.Height = Cast<Float32>(_height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = Cast<Float32>(_x);
		viewport.TopLeftY = Cast<Float32>(_y);

		GetD3D12ActiveCommandList()->RSSetViewports(1, &viewport);
	}

	void D3D12RenderContext::DrawIndexed(UInt32 _indexCount, UInt32 _startIndex, UInt32 _baseVertex)
	{
		GetD3D12ActiveCommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		GetD3D12ActiveCommandList()->DrawIndexedInstanced(_indexCount, 1, _startIndex, _baseVertex, 0);
	}

	void D3D12RenderContext::BeginRendering(const RenderingInfo& _renderingInfo)
	{
		Array<D3D12_CPU_DESCRIPTOR_HANDLE> rtHandles;
		for (const AttachmentDesc& attachmentDesc : _renderingInfo.colorAttachments)
		{
			ClearValue rtvClearValue = attachmentDesc.clearValue;
			D3D12TextureView* d3d12View = Cast<D3D12TextureView*>(attachmentDesc.view);
			if (attachmentDesc.loadOp == AttachmentLoadOp::Clear)
			{
				GetD3D12ActiveCommandList()->ClearRenderTargetView(d3d12View->GetCPUHandle(), &rtvClearValue.colorClearValue.color[0], 0, nullptr);
			}
			rtHandles.push_back(d3d12View->GetCPUHandle());
		}

		const D3D12_CPU_DESCRIPTOR_HANDLE* dsvPtr = nullptr;
		if (_renderingInfo.depthAttachment.view != nullptr)
		{
			ClearValue dsvClearValue = _renderingInfo.depthAttachment.clearValue;
			D3D12TextureView* d3d12View = Cast<D3D12TextureView*>(_renderingInfo.depthAttachment.view);
			D3D12_CPU_DESCRIPTOR_HANDLE dsv = d3d12View->GetCPUHandle();
			dsvPtr = (dsv.ptr != 0) ? &dsv : nullptr;
			if (_renderingInfo.depthAttachment.loadOp == AttachmentLoadOp::Clear)
			{
				GetD3D12ActiveCommandList()->ClearDepthStencilView(d3d12View->GetCPUHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, dsvClearValue.depthClearValue, dsvClearValue.stencilClearValue, 0, nullptr);
			}
		}
		GetD3D12ActiveCommandList()->OMSetRenderTargets((UInt32)rtHandles.size(), rtHandles.data(), 0, dsvPtr);

		SetViewport(
			_renderingInfo.renderArea.x,
			_renderingInfo.renderArea.y,
			_renderingInfo.renderArea.width,
			_renderingInfo.renderArea.height
		);



		//const Array<D3D12_CPU_DESCRIPTOR_HANDLE> rtHandles;
		//for (auto rtHandle : rtHandles)
		//{
		//	GetD3D12ActiveCommandList()->ClearRenderTargetView(rtHandle, _renderPass->GetClearColor().color, 0, nullptr);
		//}

		//if (currentFramebuffer->HasDepthAttachment())
		//{
		//	auto depthHandle = currentFramebuffer->GetDepthStencilView();
		//	GetD3D12ActiveCommandList()->ClearDepthStencilView(depthHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		//	GetD3D12ActiveCommandList()->OMSetRenderTargets((UInt32)rtHandles.size(), rtHandles.data(), false, &depthHandle);
		//}
		//else
		//{
		//	GetD3D12ActiveCommandList()->OMSetRenderTargets((UInt32)rtHandles.size(), rtHandles.data(), false, nullptr);
		//}


	}
	void D3D12RenderContext::EndRendering(const RenderingInfo& _renderingInfo)
	{
	}
	void D3D12RenderContext::BindPipelineState(const GraphicsPipelineState* _pipelineState)
	{
		RenderContext::BindPipelineState(_pipelineState);
		D3D12GraphicsPipelineState* d3d12PipelineState = Cast<D3D12GraphicsPipelineState*>(_pipelineState);

		GetD3D12ActiveCommandList()->SetGraphicsRootSignature(d3d12PipelineState->GetID3D12RootSignature());
		GetD3D12ActiveCommandList()->SetPipelineState(d3d12PipelineState->GetID3D12PipelineState());
	}
	void D3D12RenderContext::BindVertexBuffer(const GPUBuffer* _vertexBuffer, UInt32 _stride)
	{
		D3D12GPUBuffer* vertexBuffer = Cast<D3D12GPUBuffer*>(_vertexBuffer);
		ID3D12Resource* d3d12Resource = vertexBuffer->GetID3D12Resource();
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		vertexBufferView.BufferLocation = d3d12Resource->GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = (UInt32)vertexBuffer->GetSize();
		vertexBufferView.StrideInBytes = _stride;

		GetD3D12ActiveCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	}
	void D3D12RenderContext::BindIndexBuffer(const GPUBuffer* _indexBuffer)
	{
		D3D12GPUBuffer* indexBuffer = Cast<D3D12GPUBuffer*>(_indexBuffer);
		ID3D12Resource* d3d12Resource = indexBuffer->GetID3D12Resource();

		D3D12_INDEX_BUFFER_VIEW indexBufferView;
		indexBufferView.BufferLocation = d3d12Resource->GetGPUVirtualAddress();
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		indexBufferView.SizeInBytes = indexBuffer->GetSize();

		GetD3D12ActiveCommandList()->IASetIndexBuffer(&indexBufferView);
	}
	//void D3D12RenderContext::SetTexture2D(const String& _name, Shared<Texture2D> _texture)
	//{
	//	if (_texture == nullptr) return;
	//	RenderContext::SetTexture2D(_name, _texture);
	//	D3D12PipelineState* d3d12PipelineState = Cast<D3D12PipelineState*>(activePipelineState.get());
	//	const ShaderReflectionData* resourceInfo = activePipelineState->GetBindingInfo(_name);
	//	if (resourceInfo == nullptr) return;
	//	DAYDREAM_CORE_ASSERT(device->GetAPI() == RendererAPIType::DirectX12, "Wrong API!");
	//	Shared<D3D12Texture2D> d3d12Tex = SharedCast<D3D12Texture2D>(_texture);
	//	GetD3D12ActiveCommandList()->SetGraphicsRootDescriptorTable(d3d12PipelineState->GetDescriptorTableIndex(_name), d3d12Tex->GetSRVGPUHandle());
	//	String samplerName = _name + "Sampler";
	//	const ShaderReflectionData* samplerInfo = activePipelineState->GetBindingInfo(samplerName);
	//	if (samplerInfo)
	//	{
	//		GetD3D12ActiveCommandList()->SetGraphicsRootDescriptorTable(d3d12PipelineState->GetDescriptorTableIndex(samplerName), d3d12Tex->GetSamplerHandle());
	//	}
	//}
	//void D3D12RenderContext::SetTextureCube(const String& _name, Shared<TextureCube> _textureCube)
	//{
	//	if (_textureCube == nullptr) return;
	//	D3D12PipelineState* d3d12PipelineState = Cast<D3D12PipelineState*>(activePipelineState.get());
	//	const ShaderReflectionData* resourceInfo = activePipelineState->GetBindingInfo(_name);
	//	if (resourceInfo == nullptr) return;

	void D3D12RenderContext::BindShaderResourceView(const String& _name, const TextureView* _textureView, const Sampler* _sampler)
	{
		D3D12GraphicsPipelineState* d3d12PipelineState = Cast<D3D12GraphicsPipelineState*>(currentGraphicsPipelineState);
		const ShaderReflectionData* resourceInfo = currentGraphicsPipelineState->GetBindingInfo(_name);
		if (resourceInfo == nullptr) return;
		DAYDREAM_CORE_ASSERT(device->GetAPI() == RendererAPIType::DirectX12, "Wrong API!");
		D3D12TextureView* d3d12Tex = Cast<D3D12TextureView*>(_textureView);
		D3D12Sampler* d3d12Sampler = Cast<D3D12Sampler*>(_sampler);

		GetD3D12ActiveCommandList()->SetGraphicsRootDescriptorTable(d3d12PipelineState->GetDescriptorTableIndex(_name), d3d12Tex->GetGPUHandle());
		String samplerName = _name + "Sampler";
		const ShaderReflectionData* samplerInfo = currentGraphicsPipelineState->GetBindingInfo(samplerName);
		GetD3D12ActiveCommandList()->SetGraphicsRootDescriptorTable(d3d12PipelineState->GetDescriptorTableIndex(samplerName), d3d12Sampler->GetSamplerHandle());
	}

	//	DAYDREAM_CORE_ASSERT(device->GetAPI() == RendererAPIType::DirectX12, "Wrong API!");
	//	Shared<D3D12TextureCube> d3d12Tex = SharedCast<D3D12TextureCube>(_textureCube);
	//	GetD3D12ActiveCommandList()->SetGraphicsRootDescriptorTable(d3d12PipelineState->GetDescriptorTableIndex(_name), d3d12Tex->GetSRVGPUHandle());
	//	String samplerName = _name + "Sampler";
	//	const ShaderReflectionData* samplerInfo = activePipelineState->GetBindingInfo(_name);
	//	if (samplerInfo)
	//	{
	//		GetD3D12ActiveCommandList()->SetGraphicsRootDescriptorTable(d3d12PipelineState->GetDescriptorTableIndex(samplerName), d3d12Tex->GetSamplerHandle());
	//	}
	//}
	void D3D12RenderContext::BindConstantBuffer(const String& _name, const ConstantBuffer* _buffer)
	{
		if (_buffer == nullptr) return;
		D3D12GraphicsPipelineState* d3d12PipelineState = Cast<D3D12GraphicsPipelineState*>(currentGraphicsPipelineState);
		const ShaderReflectionData* resourceInfo = currentGraphicsPipelineState->GetBindingInfo(_name);
		if (resourceInfo == nullptr) return;
		DAYDREAM_CORE_ASSERT(device->GetAPI() == RendererAPIType::DirectX12, "Wrong API!");

		D3D12GPUBuffer* constantBuffer = Cast<D3D12GPUBuffer*>(_buffer->GetGPUBuffer());
		ID3D12Resource* d3d12Resource = constantBuffer->GetID3D12Resource();
		D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = d3d12Resource->GetGPUVirtualAddress();

		GetD3D12ActiveCommandList()->SetGraphicsRootConstantBufferView(d3d12PipelineState->GetDescriptorTableIndex(_name), gpuAddress);
	}

	void D3D12RenderContext::CopyBuffer(const GPUBuffer* _src, const GPUBuffer* _dst, UInt32 _copySize, UInt32 _srcOffset, UInt32 _dstOffset)
	{
		D3D12GPUBuffer* src = Cast<D3D12GPUBuffer*>(_src);
		D3D12GPUBuffer* dst = Cast<D3D12GPUBuffer*>(_dst);

		GetD3D12ActiveCommandList()->CopyBufferRegion(
			dst->GetID3D12Resource(),
			_dstOffset,
			src->GetID3D12Resource(),
			_srcOffset,
			_copySize
		);
	}

	void D3D12RenderContext::CopyBufferToTexture(const GPUBuffer* _src, const GPUTexture* _dst)
	{
		// 1. 객체 캐스팅 및 리소스 가져오기
		D3D12GPUBuffer* srcBuffer = Cast<D3D12GPUBuffer*>(_src);
		D3D12GPUTexture* dstTexture = Cast<D3D12GPUTexture*>(_dst);

		ID3D12Resource* srcResource = srcBuffer->GetID3D12Resource();
		ID3D12Resource* dstResource = dstTexture->GetID3D12Resource();

		D3D12_RESOURCE_DESC dstDesc = dstResource->GetDesc();
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedFootprint;
		UINT numRows;
		UINT64 rowSizeInBytes;
		UINT64 totalBytes;

		device->GetDevice()->GetCopyableFootprints(&dstDesc, 0, 1, 0, &placedFootprint, &numRows, &rowSizeInBytes, &totalBytes);

		D3D12_TEXTURE_COPY_LOCATION dstLocation{};
		dstLocation.pResource = dstResource;
		dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstLocation.SubresourceIndex = 0; // 원본 이미지(0번 밉맵)로 복사

		D3D12_TEXTURE_COPY_LOCATION srcLocation{};
		srcLocation.pResource = srcResource;
		srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		placedFootprint.Offset = 0;
		srcLocation.PlacedFootprint = placedFootprint;

		GetD3D12ActiveCommandList()->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);
	}

	void D3D12RenderContext::CopyDataToTexture2D(const Texture2D* _target, const void* _data)
	{
		D3D12GPUTexture* dstTexture = Cast<D3D12GPUTexture*>(_target->GetGPUTexture());
		TextureDesc desc = dstTexture->GetDesc();

		UInt32 formatSize = GraphicsUtility::GetRenderFormatSize(desc.format);
		UInt32 unalignedRowPitch = desc.width * formatSize;
		UInt32 alignedRowPitch = (unalignedRowPitch + 255) & ~255;
		UInt32 uploadSize = alignedRowPitch * desc.height;

		Shared<UploadBuffer> stagingBuffer = UploadBuffer::Create(uploadSize);
		ID3D12Resource* stagingResource = Cast<D3D12GPUBuffer*>(stagingBuffer->GetGPUBuffer())->GetID3D12Resource();

		void* mappedData = nullptr;
		stagingResource->Map(0, nullptr, &mappedData);

		Byte* dstBytes = static_cast<Byte*>(mappedData);
		const Byte* srcBytes = (Byte*)_data;

		for (UInt32 y = 0; y < desc.height; ++y)
		{
			memcpy(dstBytes + (y * alignedRowPitch), srcBytes + (y * unalignedRowPitch), unalignedRowPitch);
		}
		stagingResource->Unmap(0, nullptr);

		CopyBufferToTexture(stagingBuffer->GetGPUBuffer(), _target->GetGPUTexture());
	}

	void D3D12RenderContext::CopyTexture2D(const Texture2D* _src, const Texture2D* _dst)
	{
		D3D12GPUTexture* src = Cast<D3D12GPUTexture*>(_src->GetGPUTexture());
		D3D12GPUTexture* dst = Cast<D3D12GPUTexture*>(_dst->GetGPUTexture());

		D3D12_RESOURCE_BARRIER barriers[2] = {};

		barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barriers[0].Transition.pResource = src->GetID3D12Resource();
		barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;

		barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barriers[1].Transition.pResource = dst->GetID3D12Resource();
		barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		GetD3D12ActiveCommandList()->ResourceBarrier(2, barriers);

		GetD3D12ActiveCommandList()->CopyResource(dst->GetID3D12Resource(), src->GetID3D12Resource());

		barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barriers[0].Transition.pResource = src->GetID3D12Resource();
		barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
		barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

		barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barriers[1].Transition.pResource = dst->GetID3D12Resource();
		barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		GetD3D12ActiveCommandList()->ResourceBarrier(2, barriers);
	}
	void D3D12RenderContext::CopyTextureToCubemapFace(const Texture2D* _srcTexture2D, const TextureCube* _dstCubemap, UInt32 _faceIndex, UInt32 _mipLevel)
	{
		D3D12GPUTexture* dst = Cast<D3D12GPUTexture*>(_dstCubemap->GetGPUTexture());
		D3D12GPUTexture* src = Cast<D3D12GPUTexture*>(_srcTexture2D->GetGPUTexture());

		D3D12_RESOURCE_BARRIER barriers[2] = {};

		barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barriers[0].Transition.pResource = src->GetID3D12Resource();
		barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;

		barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barriers[1].Transition.pResource = dst->GetID3D12Resource();
		barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
		GetD3D12ActiveCommandList()->ResourceBarrier(2, barriers);

		D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
		srcLocation.pResource = src->GetID3D12Resource();
		srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		srcLocation.SubresourceIndex = 0;

		D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
		dstLocation.pResource = dst->GetID3D12Resource();
		dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dstLocation.SubresourceIndex = _mipLevel + _faceIndex * dst->GetMipLevels();

		GetD3D12ActiveCommandList()->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);

		barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barriers[0].Transition.pResource = src->GetID3D12Resource();
		barriers[0].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
		barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

		barriers[1].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barriers[1].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barriers[1].Transition.pResource = dst->GetID3D12Resource();
		barriers[1].Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		GetD3D12ActiveCommandList()->ResourceBarrier(2, barriers);
	}


	//TODO : using ComputeShader Later
	void Daydream::D3D12RenderContext::GenerateMips(GPUTexture* _texture)
	{
		D3D12GPUTexture* d3d12Texture = Cast<D3D12GPUTexture*>(_texture);

		UInt32 mipLevels = _texture->GetMipLevels();
		UInt32 layerCount = _texture->GetLayerCount();
		DXGI_FORMAT format = GraphicsUtility::DirectX::ConvertToDXGIFormat(_texture->GetFormat());

		bool isArray = (_texture->GetDesc().type == TextureType::TextureCube || _texture->GetDesc().layerCount > 1);

		Array<D3D12_CPU_DESCRIPTOR_HANDLE> rtvHandles(mipLevels * layerCount);
		Array<Pair<D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE>> srvHandles(mipLevels * layerCount);

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		rtvDesc.Format = format;
		srvDesc.Format = format;

		for (UInt32 baseMip = 0; baseMip < mipLevels; baseMip++)
		{
			for (UInt32 baseLayer = 0; baseLayer < layerCount; baseLayer++)
			{
				UInt32 currentIndex = mipLevels * baseLayer + baseMip;

				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				if (isArray)
				{
					srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
					srvDesc.Texture2DArray.MostDetailedMip = baseMip;
					srvDesc.Texture2DArray.MipLevels = 1;
					srvDesc.Texture2DArray.FirstArraySlice = baseLayer;
					srvDesc.Texture2DArray.ArraySize = 1;
				}
				else
				{
					srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					srvDesc.Texture2D.MostDetailedMip = baseMip;
					srvDesc.Texture2D.MipLevels = 1;
				}

				device->GetCBVSRVUAVHeapAlloc().Alloc(&srvHandles[currentIndex].first, &srvHandles[currentIndex].second);
				device->GetDevice()->CreateShaderResourceView(d3d12Texture->GetID3D12Resource(), &srvDesc, srvHandles[currentIndex].first);

				if (baseMip > 0)
				{
					if (isArray)
					{
						rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
						rtvDesc.Texture2DArray.MipSlice = baseMip;
						rtvDesc.Texture2DArray.FirstArraySlice = baseLayer;
						rtvDesc.Texture2DArray.ArraySize = 1;
					}
					else
					{
						rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
						rtvDesc.Texture2D.MipSlice = baseMip;
					}

					// RTV heap allocation
					device->GetRTVHeapAlloc().Alloc(&rtvHandles[currentIndex]);
					device->GetDevice()->CreateRenderTargetView(d3d12Texture->GetID3D12Resource(), &rtvDesc, rtvHandles[currentIndex]);
				}
			}
		}


		auto generateMipPSO = ResourceManager::GetResource<GraphicsPipelineState>("GenerateMipsPSO");
		auto quadMesh = ResourceManager::GetResource<Mesh>("Quad");

		BindPipelineState(generateMipPSO);
		D3D12GraphicsPipelineState* d3d12PipelineState = Cast<D3D12GraphicsPipelineState*>(generateMipPSO);
		BindVertexBuffer(quadMesh->GetVertexBuffer()->GetGPUBuffer(), quadMesh->GetVertexBuffer()->GetStride());
		BindIndexBuffer(quadMesh->GetIndexBuffer()->GetGPUBuffer());
		GetD3D12ActiveCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (UInt32 mip = 1; mip < mipLevels; mip++)
		{
			UInt32 mipWidth = std::max(1U, _texture->GetWidth() >> mip);
			UInt32 mipHeight = std::max(1U, _texture->GetHeight() >> mip);

			D3D12_RECT rect;
			rect.left = 0;
			rect.top = 0;
			rect.right = Cast<UInt32>(mipWidth);
			rect.bottom = Cast<UInt32>(mipHeight);

			GetD3D12ActiveCommandList()->RSSetScissorRects(1, &rect);

			D3D12_VIEWPORT viewport = {};
			viewport.Width = Cast<Float32>(mipWidth);
			viewport.Height = Cast<Float32>(mipHeight);
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0;
			viewport.TopLeftY = 0;

			GetD3D12ActiveCommandList()->RSSetViewports(1, &viewport);

			for (UInt32 face = 0; face < 6; face++)
			{
				UInt32 currentIndex = mipLevels * face + mip;
				UInt32 prevIndex = mipLevels * face + (mip - 1);


				D3D12_RESOURCE_BARRIER barrier = {};
				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				barrier.Transition.pResource = d3d12Texture->GetID3D12Resource();
				barrier.Transition.Subresource = prevIndex;
				barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
				barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
				GetD3D12ActiveCommandList()->ResourceBarrier(1, &barrier);

				GetD3D12ActiveCommandList()->SetGraphicsRootDescriptorTable(d3d12PipelineState->GetDescriptorTableIndex("Texture"), srvHandles[prevIndex].second);

				//GetD3D12ActiveCommandList()->ClearRenderTargetView(d3d12Tex->GetRTVCPUHandle(index), color, 0, nullptr);

				GetD3D12ActiveCommandList()->OMSetRenderTargets(1, &rtvHandles[currentIndex], false, nullptr);
				GetD3D12ActiveCommandList()->DrawIndexedInstanced(quadMesh->GetIndexCount(), 1, 0, 0, 0);

				barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
				barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
				GetD3D12ActiveCommandList()->ResourceBarrier(1, &barrier);
			}
		}
	}
	void D3D12RenderContext::TransitionTextureState(const GPUTexture* _texture, ResourceState _beforeState, ResourceState _afterState, UInt32 _baseMip, UInt32 _mipLevels, UInt32 _baseLayer, UInt32 _layerCount)
	{
		if (_beforeState == _afterState)
		{
			DAYDREAM_RENDERER_WARN("Before State == After State");
			return;
		}

		const D3D12GPUTexture* d3d12Texture = Cast<const D3D12GPUTexture*>(_texture);
		ID3D12Resource* resource = d3d12Texture->GetID3D12Resource();

		_mipLevels = (_mipLevels == -1 ? _texture->GetMipLevels() : _mipLevels);
		_layerCount = (_layerCount == -1 ? _texture->GetLayerCount() : _layerCount);

		UInt32 totalMipLevels = d3d12Texture->GetMipLevels();
		UInt32 totalLayers = d3d12Texture->GetLayerCount();

		if (_baseLayer == 0 && _baseMip == 0 && _layerCount == totalLayers && _mipLevels == totalMipLevels)
		{
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.pResource = resource;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES; // 전체 통째로!
			barrier.Transition.StateBefore = GraphicsUtility::DirectX12::ConvertToD3D12ResourceStates(_beforeState);
			barrier.Transition.StateAfter = GraphicsUtility::DirectX12::ConvertToD3D12ResourceStates(_afterState);

			GetD3D12ActiveCommandList()->ResourceBarrier(1, &barrier);
			return;
		}

		std::vector<D3D12_RESOURCE_BARRIER> barriers;
		barriers.reserve(_layerCount * _mipLevels);
		for (UInt32 layer = 0; layer < _layerCount; ++layer)
		{
			for (UInt32 mip = 0; mip < _mipLevels; ++mip)
			{
				UInt32 targetLayer = _baseLayer + layer;
				UInt32 targetMip = _baseMip + mip;

				UInt32 subresourceIndex = targetMip + (targetLayer * totalMipLevels);

				D3D12_RESOURCE_BARRIER barrier = {};
				barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				barrier.Transition.pResource = resource;
				barrier.Transition.Subresource = subresourceIndex;
				barrier.Transition.StateBefore = GraphicsUtility::DirectX12::ConvertToD3D12ResourceStates(_beforeState);
				barrier.Transition.StateAfter = GraphicsUtility::DirectX12::ConvertToD3D12ResourceStates(_afterState);

				barriers.push_back(barrier);
			}
		}

		if (!barriers.empty())
		{
			GetD3D12ActiveCommandList()->ResourceBarrier(static_cast<UINT>(barriers.size()), barriers.data());
		}
	}
	void Daydream::D3D12RenderContext::TransitionBufferState(const GPUBuffer* _buffer, ResourceState _beforeState, ResourceState _afterState)
	{
		if (_beforeState == _afterState)
		{
			DAYDREAM_RENDERER_WARN("Before State == After State");
			return;
		}

		D3D12GPUBuffer* d3d12Buffer = Cast<D3D12GPUBuffer*>(_buffer);

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = d3d12Buffer->GetID3D12Resource();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = GraphicsUtility::DirectX12::ConvertToD3D12ResourceStates(_beforeState);
		barrier.Transition.StateAfter = GraphicsUtility::DirectX12::ConvertToD3D12ResourceStates(_afterState);

		GetD3D12ActiveCommandList()->ResourceBarrier(1, &barrier);
	}

	void D3D12RenderContext::SetActiveCommandList(RenderCommandList* _commandList)
	{
		activeCommandList = _commandList;
		activeD3D12CommandList = Cast<D3D12RenderCommandList*>(_commandList)->GetID3D12GraphicsCommandList();
	}
}
