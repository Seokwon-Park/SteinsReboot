#pragma once
#include "Steins/Enum/RendererEnums.h"
#include "Framebuffer.h"

namespace Steins
{
	class SteinsWindow;
	struct SwapChainSpecification
	{
		UInt32 width = 0;
		UInt32 height = 0;
		UInt32 bufferCount = 2;
		RenderFormat format = RenderFormat::R8G8B8A8_UNORM;
		bool isFullscreen = false;
		bool isVSync = false;
	};

	class SwapChain
	{
	public:
		virtual ~SwapChain() = default;

		bool GetVSync() const { return desc.isVSync; }
		virtual void SetVSync(bool _enabled) = 0;

		virtual void SwapBuffers() = 0;
		
		inline Framebuffer* GetBackFramebuffer() { return backFramebuffer.get(); };
		static Shared<SwapChain> Create(SwapChainSpecification* _desc, SteinsWindow* _window);
	protected:
		SwapChainSpecification desc;
		Shared<Framebuffer> backFramebuffer;
	};
}