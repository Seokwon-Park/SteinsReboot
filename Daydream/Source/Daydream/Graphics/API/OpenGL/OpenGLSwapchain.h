#pragma once

#include "Daydream/Graphics/Core/Swapchain.h"
#include "OpenGLTextureView.h"


#include "GLFW/glfw3.h"

namespace Daydream
{
	class OpenGLSwapchain : public Swapchain
	{
	public:
		OpenGLSwapchain(const DaydreamWindow& _window, const SwapchainDesc& _desc);
		virtual ~OpenGLSwapchain() override;

		virtual void SetVSync(bool _enabled) override;
		virtual void Present() override;
		//virtual void ResizeSwapchain(UInt32 _width, UInt32 _height) override;

		virtual void BeginFrame() override;
		virtual void EndFrame() override;

		virtual TextureView* GetCurrentRenderTargetView() const { return backBufferRTV.get(); };
		virtual Shared<RenderCommandList> GetCurrentCommandList() const { return nullptr; };

	private:
		void CreateBackBufferView();

		GLFWwindow* window;

		UInt32 blitFBO;
		Shared<OpenGLGPUTexture> backBufferTexture;
		Shared<OpenGLTextureView> backBufferRTV;

	};
}