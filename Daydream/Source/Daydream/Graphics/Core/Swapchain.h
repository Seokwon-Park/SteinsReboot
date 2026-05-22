#pragma once
#include "Daydream/Enum/RendererEnums.h"
#include "Daydream/Graphics/Resources/Texture/Texture2D.h"
#include "Daydream/Graphics/Resources/Texture/TextureView.h"
#include "Daydream/Graphics/Core/RenderCommandList.h"

namespace Daydream
{
	class DaydreamWindow;
	struct SwapchainDesc
	{
		UInt32 width = 0;
		UInt32 height = 0;
		UInt32 imageCount = 2;
		RenderFormat format = RenderFormat::R8G8B8A8_UNORM;
		bool isFullscreen = false;
		bool isVSync = false;
	};

	class Swapchain
	{
	public:
		Swapchain(const SwapchainDesc& _desc);
		virtual ~Swapchain() = default;

		bool GetVSync() const { return desc.isVSync; }
		virtual void SetVSync(bool _enabled) = 0;

		virtual void Present() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;

		virtual TextureView* GetCurrentRenderTargetView() const = 0;
		virtual Shared<RenderCommandList> GetCurrentCommandList() const = 0;

		inline const UInt32 GetWidth() const { return desc.width; }
		inline const UInt32 GetHeight() const { return desc.height; }

		inline const SwapchainDesc& GetDesc() const { return desc; };
		void ResizeSwapchain(UInt32 _width, UInt32 _height);

		static Shared<Swapchain> Create(const DaydreamWindow& _window, const SwapchainDesc& _desc);
	protected:
		bool isSwapchainResized = false;

		SwapchainDesc desc;
		Array<Texture2D> swapchainBackBuffers;
	};
}