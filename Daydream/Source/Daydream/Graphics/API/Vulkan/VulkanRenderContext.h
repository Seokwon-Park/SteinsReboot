#pragma once

#include "Daydream/Graphics/Core/RenderContext.h"
#include "VulkanRenderDevice.h"
#include "VulkanRenderCommandList.h"

namespace Daydream
{
	class VulkanRenderContext : public RenderContext
	{
	public:
		VulkanRenderContext(VulkanRenderDevice* _device);
		virtual ~VulkanRenderContext();

		virtual void BeginCommandList() override;
		virtual void EndCommandList() override;
		virtual void SetViewport(UInt32 _x, UInt32 _y, UInt32 _width, UInt32 _height) override;
		virtual void DrawIndexed(UInt32 _indexCount, UInt32 _startIndex, UInt32 _baseVertex) override;

		virtual void BeginRendering(const RenderingInfo& _renderingInfo) override;
		virtual void EndRendering(const RenderingInfo& _renderingInfo) override;

		//virtual void BeginRenderPass(Shared<RenderPass> _renderPass, Shared<Framebuffer> _framebuffer) override;
		//virtual void EndRenderPass(Shared<RenderPass> _renderPass) override;

		virtual void BindPipelineState(const GraphicsPipelineState* _pipelineState) override;

		virtual void BindVertexBuffer(const GPUBuffer* _vertexBuffer, UInt32 _stride) override;
		virtual void BindIndexBuffer(const GPUBuffer* _indexBuffer) override;

		/*virtual void SetTexture2D(const String& _name, Shared<Texture2D> _texture) override;
		virtual void SetTextureCube(const String& _name, Shared<TextureCube> _textureCube) override;*/
		virtual void BindShaderResourceView(const String& _name, const TextureView* _textureView, const Sampler* _sampler) override;
		virtual void BindConstantBuffer(const String& _name, const GPUBuffer* _buffer) override;

		virtual void CopyBuffer(const GPUBuffer* _src, const GPUBuffer* _dst, UInt32 _copySize, UInt32 _srcOffset, UInt32 _dstOffset) override;
		virtual void CopyBufferToTexture(const GPUBuffer* _src, const GPUTexture* _dst) override;

		virtual void CopyTexture(const GPUTexture* _src, const GPUTexture* _dst, const TextureCopyRegion& _region) override;

		virtual void GenerateMips(GPUTexture* _texture) override;

		virtual void TransitionTextureState(GPUTexture * _texture,
			ResourceState _afterState,
			UInt32 _baseMip, 
			UInt32 _mipLevels, 
			UInt32 _baseLayer, 
			UInt32 _layerCount) override;

		virtual void TransitionBufferState(GPUBuffer * _buffer,
			ResourceState _afterState) override;

		virtual void SetActiveCommandList(RenderCommandList * _commandList) override;

	private:
		vk::CommandBuffer GetActiveCommandBuffer();
		
		VulkanRenderDevice* device;
		vk::CommandBuffer activeCommandBuffer;
	};
}