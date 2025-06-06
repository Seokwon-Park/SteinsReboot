#pragma once

#include "Steins/Render/SwapChain.h"

#include "GLFW/glfw3.h"

namespace Steins
{
	class OpenGLSwapChain : public SwapChain
	{
	public:
		OpenGLSwapChain(SwapChainSpecification* _desc, SteinsWindow* _window);
		virtual ~OpenGLSwapChain() override;

		virtual void SetVSync(bool _enabled) override;
		virtual void SwapBuffers() override;
		virtual void ResizeSwapChain(UInt32 _width, UInt32 height) override;

		virtual Framebuffer* GetBackFramebuffer() { return nullptr; };
	private:
		GLFWwindow* window;
		
	};
}