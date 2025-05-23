#pragma once

#include "Steins/Render/Swapchain.h"
#include "D3D11GraphicsDevice.h"
#include "D3D11Framebuffer.h"

namespace Steins
{
	class D3D11SwapChain : public SwapChain
	{
	public:
		D3D11SwapChain(D3D11GraphicsDevice* _device, SwapChainSpecification* _desc, SteinsWindow* _window);
		virtual ~D3D11SwapChain();

		virtual void SetVSync(bool _enabled) override;
		virtual void SwapBuffers() override;

		inline IDXGISwapChain* GetDXGISwapChain() { return swapChain.Get(); }
		
	private:
		D3D11GraphicsDevice* device;
		ComPtr<IDXGISwapChain> swapChain;
	};
}