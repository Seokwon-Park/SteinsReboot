#pragma once

#include "Daydream/Graphics/Core/RenderDevice.h"


namespace Daydream
{
	class D3D11RenderDevice : public RenderDevice
	{
	public:
		D3D11RenderDevice();
		virtual ~D3D11RenderDevice() override;

		virtual void Init() override;
		virtual void Shutdown() override;

		virtual Unique<RenderContext> CreateContext() override;
		virtual Shared<RenderCommandList> CreateRenderCommandList() override;
		virtual Shared<GPUBuffer> CreateGPUBuffer(const BufferDesc& _desc) override;
		virtual Shared<GPUTexture> CreateGPUTexture(const TextureDesc& _desc) override;
		virtual Shared<TextureView> CreateTextureView(Texture* _texture, const TextureViewDesc& _desc) override;
		virtual Shared<GraphicsPipelineState> CreatePipelineState(const GraphicsPipelineStateDesc& _desc)override;
		virtual Shared<Shader> CreateShader(const ShaderType& _type) override;
		virtual Shared<Swapchain> CreateSwapchain(const DaydreamWindow& _window, const SwapchainDesc& _desc)override;
		virtual Shared<Sampler> CreateSampler(const SamplerDesc& _desc) override;
		virtual Unique<ImGuiRenderer> CreateImGuiRenderer() override;

		ID3D11Device* GetDevice() { return device.Get(); }
		ID3D11DeviceContext* GetContext() { return deviceContext.Get(); }
		IDXGIFactory7* GetFactory() { return dxgiFactory.Get(); }
	private:
		bool debugLayerEnabled = true;
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1;

		ComPtr<ID3D11Debug> debugLayer = nullptr;
		ComPtr<ID3D11Device> device = nullptr;
		ComPtr<ID3D11DeviceContext> deviceContext = nullptr;

		ComPtr<IDXGIDevice> dxgiDevice = nullptr;
		ComPtr<IDXGIAdapter> dxgiAdapter = nullptr;
		ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	};
}

