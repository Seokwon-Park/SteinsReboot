#pragma once

#include "Daydream/Graphics/Core/Swapchain.h"
#include "D3D11RenderDevice.h"
#include "D3D11TextureView.h"

namespace Daydream
{
	class D3D11Swapchain : public Swapchain
	{
	public:
		D3D11Swapchain(D3D11RenderDevice* _device, const DaydreamWindow& _window, const SwapchainDesc& _desc);
		virtual ~D3D11Swapchain();

		virtual void SetVSync(bool _enabled) override;
		virtual void Present() override;
		//virtual void ResizeSwapchain(UInt32 _width, UInt32 _height) override;

		virtual void BeginFrame() override;
		virtual void EndFrame() override;

		virtual TextureView* GetCurrentRenderTargetView() const { return backBufferRTV.get(); };
		virtual RenderCommandList* GetCurrentCommandList() const { return nullptr; };

		inline IDXGISwapChain* GetDXGISwapchain() { return swapchain.Get(); }

	private:
		void CreateBackBufferView();

		D3D11RenderDevice* device;
		ComPtr<IDXGISwapChain> swapchain;

		Shared<D3D11GPUTexture> backBufferTexture;
		Shared<D3D11TextureView> backBufferRTV;
	};
}