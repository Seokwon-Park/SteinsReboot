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

		virtual void BindPipelineState(Shared<GraphicsPipelineState> _pipelineState);

		virtual void BindVertexBuffer(Shared<VertexBuffer> _vertexBuffer) {};
		virtual void BindIndexBuffer(Shared<IndexBuffer> _indexBuffer) {};

		//virtual void SetTexture2D(const String& _name, Shared<Texture2D> _texture) ;
		//virtual void SetTextureCube(const String& _name, Shared<TextureCube> _textureCube) {};
		virtual void BindShaderResourceView(const String& _name, Shared<TextureView> _textureView, Shared<Sampler> _sampler) {};
		virtual void SetConstantBuffer(const String& _name, Shared<ConstantBuffer> _buffer) {};

		virtual void CopyBuffer(Shared<GPUBuffer> _src, Shared<GPUBuffer> _dst, UInt32 _copySize) {};
		virtual void CopyBufferToTexture(Shared<GPUBuffer> _src, Shared<GPUTexture> _dst) {};

		// Uploads raw pixel data to the texture. 
		// Backends may override this to handle API-specific memory alignment requirements.
		virtual void CopyDataToTexture2D(Shared<Texture2D> _target, Shared<Array<Byte>> _data);
		virtual void CopyTexture2D(Shared<Texture2D> _src, Shared<Texture2D> _dst) {};
		virtual void CopyTextureToCubemapFace(Shared<Texture2D> _srcTexture2D, Shared<TextureCube> _dstCubemap, UInt32 _faceIndex, UInt32 _mipLevel = 0) {};
		virtual void CopyTextureCubeToTexture2D(Shared<TextureCube> _srcCubemap, UInt32 _faceIndex, Shared<Texture2D> _dstTexture2D, UInt32 _mipLevel = 0) {};
		//virtual void CopyTextureToBuffer(Shared<Texture2D> _srcTexture, Shared<Buffer> _dstBuffer, UInt32 _offsetX, UInt32 _offsetY, UInt32 _width, UInt32 _height) = 0;

		virtual void GenerateMips(Shared<Texture> _texture) {};

		virtual void TransitionTextureState(Shared<GPUTexture> _texture,
			ResourceState _beforeState,
			ResourceState _afterState, 
			UInt32 _baseMip = 0,
			UInt32 _mipLevels = -1, 
			UInt32 _baseLayer = 0, 
			UInt32 _layerCount = -1) {};

		virtual void TransitionBufferState(
			Shared<GPUBuffer> _buffer,
			ResourceState _beforeState,
			ResourceState _afterState
		) {};

		virtual void SetActiveCommandList(Shared<RenderCommandList> _commandList) {};
		Shared<RenderCommandList> GetActiveCommandList() const { return activeCommandList; }

		inline void CaptureResource(Shared<GPUResource> _resource)
		{
			capturedResources.push_back(_resource);
		}
		inline Array<Shared<GPUResource>> GetCapturedResources()
		{
			return std::move(capturedResources);
		}
	protected:
		Shared<GraphicsPipelineState> currentGraphicsPipelineState;
		Shared<RenderCommandList> activeCommandList;

		Array<Shared<GPUResource>> capturedResources;

	private:
	};
}