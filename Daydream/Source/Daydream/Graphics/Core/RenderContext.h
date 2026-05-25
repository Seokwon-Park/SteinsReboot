#pragma once

#include "RenderCommandList.h"
#include "RenderingInfo.h"
#include "Daydream/Enum/RendererEnums.h"
#include "Daydream/Graphics/Resources/Texture/Texture2D.h"
#include "Daydream/Graphics/Resources/Texture/TextureCube.h"


namespace Daydream
{
	class RenderDevice;
	class GPUBuffer;
	class GPUTexture;
	class GPUResource;
	class VertexBuffer;
	class IndexBuffer;
	class Mesh;
	class Material;
	class Texture;
	class Texture2D;
	class TextureCube;
	class TextureView;
	class ConstantBuffer;
	class GraphicsPipelineState;

	struct RenderingInfo;

	struct TextureSubresourceLayers
	{
		UInt32 mipLevel = 0;
		UInt32 baseArrayLayer = 0;
		UInt32 layerCount = 1; // 2D면 1, 큐브맵 전체면 6
	};

	struct TextureCopyRegion
	{
		TextureSubresourceLayers srcSubresource;
		TextureSubresourceLayers dstSubresource;

		// 복사할 시작 위치 (보통 0, 0, 0)
		UInt32 srcOffset[3] = { 0, 0, 0 };
		UInt32 dstOffset[3] = { 0, 0, 0 };

		// 복사할 크기 (3D면 depth가 1 이상, 2D/Cube는 depth가 1)
		UInt32 extent[3] = { 1, 1, 1 };
	};

	class RenderContext
	{
	public:
		virtual ~RenderContext() = default;

		virtual void BeginCommandList() = 0;
		virtual void EndCommandList() {};
		virtual void SetViewport(UInt32 _x, UInt32 _y, UInt32 _width, UInt32 _height) = 0;
		virtual void DrawIndexed(UInt32 _indexCount, UInt32 _startIndex = 0, UInt32 _baseVertex = 0) = 0;

		virtual void BeginRendering(const RenderingInfo& _renderingInfo) {};
		virtual void EndRendering(const RenderingInfo& _renderingInfo) {};

		virtual void BindPipelineState(const GraphicsPipelineState* _pipelineState);

		virtual void BindVertexBuffer(const GPUBuffer* _vertexBuffer, UInt32 _stride) {};
		virtual void BindIndexBuffer(const GPUBuffer* _indexBuffer) {};

		//virtual void SetTexture2D(const String& _name, Shared<Texture2D> _texture) ;
		//virtual void SetTextureCube(const String& _name, Shared<TextureCube> _textureCube) {};
		virtual void BindShaderResourceView(const String& _name, const TextureView* _textureView, const Sampler* _sampler) {};
		virtual void BindConstantBuffer(const String& _name, const ConstantBuffer* _buffer) {};
		virtual void PushConstants(const String& _name, const void* _data, UInt32 _size) {};

		virtual void CopyBuffer(const GPUBuffer* _src, const GPUBuffer* _dst, UInt32 _copySize, UInt32 _srcOffset = 0, UInt32 _dstOffset = 0) {};
		virtual void CopyBufferToTexture(const GPUBuffer* _src, const GPUTexture* _dst) {};

		// Uploads raw pixel data to the texture. 
		// Backends may override this to handle API-specific memory alignment requirements.
		virtual void CopyDataToTexture2D(const Texture2D* _target, const void*_data);
		virtual void CopyTexture2D(const Texture2D* _src, const Texture2D* _dst) {};
		virtual void CopyTextureToCubemapFace(const Texture2D* _srcTexture2D, const TextureCube* _dstCubemap, UInt32 _dstFaceIndex, UInt32 _mipLevel = 0) {};
		virtual void CopyTextureCubeToTexture2D(const TextureCube* _srcCubemap, const Texture2D* _dstTexture2D, UInt32 _srcFaceIndex, UInt32 _mipLevel = 0) {};
		//virtual void CopyTextureToBuffer(Shared<Texture2D> _srcTexture, Shared<Buffer> _dstBuffer, UInt32 _offsetX, UInt32 _offsetY, UInt32 _width, UInt 32 _height) = 0;

		virtual void GenerateMips(GPUTexture* _texture) {};

		virtual void TransitionTextureState(const GPUTexture* _texture,
			ResourceState _beforeState,
			ResourceState _afterState,
			UInt32 _baseMip = 0,
			UInt32 _mipLevels = -1,
			UInt32 _baseLayer = 0,
			UInt32 _layerCount = -1) {
		};

		virtual void TransitionBufferState(const GPUBuffer* _buffer,
			ResourceState _beforeState,
			ResourceState _afterState) {};

		virtual void SetActiveCommandList(RenderCommandList * _commandList) {};
		RenderCommandList* GetActiveCommandList() const { return activeCommandList; }

		inline void CaptureResource(Shared<GPUResource> _resource)
		{
			capturedResources.push_back(_resource);
		}
		inline Array<Shared<GPUResource>> GetCapturedResources()
		{
			return std::move(capturedResources);
		}
	protected:
		const GraphicsPipelineState* currentGraphicsPipelineState;
		RenderCommandList* activeCommandList;

		Array<Shared<GPUResource>> capturedResources;

	private:
	};
}