#pragma once
#include "Daydream/Graphics/Core/RenderDevice.h"
#include "GLFW/glfw3.h"
//#include "glad/glad.h"

namespace Daydream
{
	class OpenGLRenderDevice: public RenderDevice
	{
	public:
		OpenGLRenderDevice();
		~OpenGLRenderDevice() override;

		virtual void Init() override;
		virtual void Shutdown() override;

		virtual Unique<RenderContext> CreateContext() override;
		virtual Shared<RenderCommandList> CreateRenderCommandList() override;
		virtual Shared<GPUBuffer> CreateGPUBuffer(const BufferDesc& _desc) override;
		virtual Shared<GPUTexture> CreateGPUTexture(const TextureDesc & _desc) override;
		virtual Shared<TextureView> CreateTextureView(Texture* _texture, const TextureViewDesc& _desc) override;
		//virtual Shared<VertexBuffer> CreateDynamicVertexBuffer(UInt32 _size, UInt32 _stride, UInt32 _initialDataSize = 0, const void* _initialData = nullptr) override;
		//virtual Shared<VertexBuffer> CreateStaticVertexBuffer(UInt32 _size, UInt32 _stride, const void* _initialData) override;
		//virtual Shared<IndexBuffer> CreateIndexBuffer(const UInt32 * _indices, UInt32 _count) override;
		//virtual Shared<ConstantBuffer> CreateConstantBuffer(UInt32 _size) override;
		//virtual Shared<RenderPass> CreateRenderPass(const RenderPassDesc& _desc) override;
		//virtual Shared<Framebuffer> CreateFramebuffer(Shared<RenderPass> _renderPass, const FramebufferDesc & _desc) override;
		virtual Shared<GraphicsPipelineState> CreatePipelineState(const GraphicsPipelineStateDesc& _desc)override;
		virtual Shared<Shader> CreateShader(const ShaderType& _type) override;
		virtual Shared<Swapchain> CreateSwapchain(const DaydreamWindow& _window, const SwapchainDesc& _desc)override;
		//virtual Shared<Texture2D> CreateEmptyTexture2D(const TextureDesc& _desc) override;
		//virtual Shared<Texture2D> CreateTexture2D(const void* _imageData, const TextureDesc& _desc)override;
		//virtual Shared<TextureCube> CreateTextureCube(Array<const void*>& _imagePixels, const TextureDesc& _desc)override;
		//virtual Shared<TextureCube> CreateTextureCube(const Array<Shared<Texture2D>>& _textures, const TextureDesc& _desc) override;
		//virtual Shared<TextureCube> CreateEmptyTextureCube(const TextureDesc& _desc) override;
		virtual Shared<Sampler> CreateSampler(const SamplerDesc& _desc) override;
		virtual Unique<ImGuiRenderer> CreateImGuiRenderer() override;

		//virtual void CopyTexture2D(Shared<Texture2D> _src, Shared<Texture2D> _dst) override;
		//virtual void CopyTextureToCubemapFace(TextureCube* _dstCubemap, UInt32 _faceIndex, Texture2D* _srcTexture2D, UInt32 _mipLevel = 0) override;

		String GetVersion() const { return version; }
	protected:

	private:

		std::string version = "#version 450";
	};
}
