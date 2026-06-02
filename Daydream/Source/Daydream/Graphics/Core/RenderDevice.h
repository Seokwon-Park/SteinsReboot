#pragma once

#include "Daydream/Enum/RendererEnums.h"
#include "Daydream/ImGui/ImGuiRenderer.h"

namespace Daydream
{
	struct SwapchainDesc;
	struct RenderPassDesc;
	struct FramebufferDesc;
	struct GraphicsPipelineStateDesc;
	struct BufferDesc;
	struct TextureDesc;
	struct TextureViewDesc;
	struct SamplerDesc;
	struct ShaderResourceViewDesc;
	class DaydreamWindow;
	class RenderContext;

	//////////////////////   Buffer   ///////////////////////
	class GPUBuffer;
	//////////////////////////////////////////////////////////

	//////////////////////   Texture   ///////////////////////
	class GPUTexture;
	class TextureView;
	//////////////////////////////////////////////////////////

	class RenderPass;
	class Framebuffer;
	class GraphicsPipelineState;
	class Shader;
	class Swapchain;
	class Texture;
	class Texture2D;
	class TextureCube;
	class Sampler;
	class ImGuiRenderer;
	class VertexArray;
	class BufferLayout;
	class Material;

	struct RendererAPIInfo
	{
		String APIName;
		String vendor;
		String physicalDeviceInfo;
		String version;
	};

	class RenderDevice
	{
	public:
		virtual ~RenderDevice() {};

		virtual void Init() = 0;
		virtual void Shutdown() = 0;

		virtual Unique<RenderContext> CreateContext() = 0;
		virtual Shared<RenderCommandList> CreateRenderCommandList() = 0;
		virtual Shared<GPUBuffer> CreateGPUBuffer(const BufferDesc& _desc) = 0;
		virtual Shared<GPUTexture> CreateGPUTexture(const TextureDesc& _desc) = 0;
		virtual Shared<TextureView> CreateTextureView(Texture* _texture, const TextureViewDesc& _desc) = 0;
		//virtual Shared<VertexBuffer> CreateDynamicVertexBuffer(UInt32 _size, UInt32 _stride, UInt32 _initialDataSize = 0, const void* _initialData = nullptr);
		//virtual Shared<VertexBuffer> CreateStaticVertexBuffer(UInt32 _size, UInt32 _stride, const void* _initialData);
		//virtual Shared<IndexBuffer> CreateIndexBuffer(const UInt32* _indices, UInt32 _count);
		//virtual Shared<ConstantBuffer> CreateConstantBuffer(UInt32 _size);
		//virtual Shared<RenderPass> CreateRenderPass(const RenderPassDesc& _desc) = 0;
		//virtual Shared<Framebuffer> CreateFramebuffer(Shared<RenderPass> _renderPass, const FramebufferDesc& _desc) = 0;
		virtual Shared<GraphicsPipelineState> CreatePipelineState(const GraphicsPipelineStateDesc& _desc) = 0;
		virtual Shared<Shader> CreateShader(const std::string& _src, const ShaderType& _type, ShaderLoadMode _mode) = 0;
		virtual Shared<Swapchain> CreateSwapchain(const DaydreamWindow& _window, const SwapchainDesc& _desc) = 0;
		//virtual Shared<Texture2D> CreateEmptyTexture2D(const TextureDesc& _desc) = 0;
		//virtual Shared<Texture2D> CreateTexture2D(const void* _imageData, const TextureDesc& _desc)  = 0;
		//virtual Shared<TextureCube> CreateTextureCube(Array<const void*>& _imagePixels, const TextureDesc& _desc) = 0;
		//virtual Shared<TextureCube> CreateTextureCube(const Array<Shared<Texture2D>>& _textures, const TextureDesc& _desc) { return nullptr; };
		//virtual Shared<TextureCube> CreateEmptyTextureCube(const TextureDesc& _desc) = 0;
		virtual Shared<Sampler> CreateSampler(const SamplerDesc& _desc) = 0;
		virtual Unique<ImGuiRenderer> CreateImGuiRenderer() = 0;

		virtual UInt32 GetAlignedRowPitch(UInt32 _width, RenderFormat _format) const;

		inline const RendererAPIInfo& GetAPIInfo() { return info; }

		inline RendererAPIType GetAPI() const { return API; }

		static Unique<RenderDevice> Create(RendererAPIType _API);
	protected:
		RendererAPIType API = RendererAPIType::None;
		RendererAPIInfo info;
	};
}
