#include "DaydreamPCH.h"
#include "OpenGLSwapchain.h"

#include "Daydream/Core/Window.h"
#include "GLFW/glfw3.h"

#include "glad/glad.h"


namespace Daydream
{
	OpenGLSwapchain::OpenGLSwapchain(const DaydreamWindow& _window, const SwapchainDesc& _desc)
		:Swapchain(_desc)
	{
		desc = _desc;
		window = (GLFWwindow*)_window.GetNativeWindow();

		glCreateFramebuffers(1, &blitFBO);

		CreateBackBufferView();
	}

	OpenGLSwapchain::~OpenGLSwapchain()
	{
	}
	void OpenGLSwapchain::SetVSync(bool _enabled)
	{
		glfwSwapInterval(desc.isVSync);
	}

	void OpenGLSwapchain::Present()
	{
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glfwSwapBuffers(window);
	}

	void OpenGLSwapchain::BeginFrame()
	{
		GLFWwindow* currentContext = glfwGetCurrentContext();
		if (currentContext != window)
		{
			glfwMakeContextCurrent(window);
		}

		if(isSwapchainResized)
		{
			CreateBackBufferView();
		}
	}
	void OpenGLSwapchain::EndFrame()
	{
		glBlitNamedFramebuffer(
			blitFBO, 0,
			0, 0, desc.width, desc.height,
			0, 0, desc.width, desc.height,
			GL_COLOR_BUFFER_BIT,
			GL_NEAREST
		);

		glfwMakeContextCurrent(nullptr);
	}

	void OpenGLSwapchain::CreateBackBufferView()
	{
		TextureDesc textureDesc{};
		textureDesc.width = desc.width;
		textureDesc.height = desc.height;
		textureDesc.mipLevels = 1;
		textureDesc.sampleCount = 1;
		textureDesc.format = desc.format;
		textureDesc.textureUsage = TextureUsage::RenderTarget;
		textureDesc.type = TextureType::Texture2D;

		backBufferTexture = MakeShared<OpenGLGPUTexture>(textureDesc);

		TextureViewDesc viewDesc;
		viewDesc.format = desc.format;
		viewDesc.type = TextureViewType::RenderTarget;

		backBufferRTV = MakeShared<OpenGLTextureView>(backBufferTexture.get(), viewDesc);

		glNamedFramebufferTexture(blitFBO, GL_COLOR_ATTACHMENT0, backBufferRTV->GetTextureViewID(), 0);
	}
}
