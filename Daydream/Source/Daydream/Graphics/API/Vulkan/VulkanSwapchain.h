#pragma once

#include "Daydream/Graphics/Core/Swapchain.h"
#if defined(DAYDREAM_PLATFORM_WINDOWS)
#define VK_USE_PLATFORM_WIN32_KHR
#endif
#include "Daydream/Graphics/Core/RenderDevice.h"
#include "Daydream/Core/Window.h"

#include "VulkanTextureView.h"
#include "VulkanFrameBuffer.h"
#include "VulkanRenderDevice.h"
#include "VulkanRenderCommandList.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Daydream
{
	class VulkanSwapchain : public Swapchain
	{
	public:
		VulkanSwapchain(VulkanRenderDevice* _device, const DaydreamWindow& _window, const SwapchainDesc& _desc);
		virtual ~VulkanSwapchain() override;

		virtual void SetVSync(bool _enabled) override;
		virtual void Present() override;

		virtual void BeginFrame() override;
		virtual void EndFrame() override;

		//virtual void BeginRenderPass() override;
		//virtual void EndRenderPass() override;

		inline virtual TextureView* GetCurrentRenderTargetView() const { return backBufferRTVs[currentFrame].get(); };
		inline virtual RenderCommandList* GetCurrentCommandList() const override { return commandLists[currentFrame].get(); }


		inline vk::Format GetFormat() const { return format; }
		inline vk::SwapchainKHR GetVkSwapchain() const { return swapchain.get(); }
		inline vk::Extent2D GetExtent() const { return extent; }
		inline Array<vk::Image> GetSwapchainImages() const { return swapchainImages; };
	private:
		void CreateSwapchain();
		void CreateCommandLists();
		void ResizeSwapchain();
		void CreateBackBufferView();

		vk::SurfaceFormatKHR ChooseSwapSurfaceFormat(const Array<vk::SurfaceFormatKHR>& _availableFormats, RenderFormat _desiredFormat);
		vk::PresentModeKHR ChooseSwapPresentMode(const Array<vk::PresentModeKHR>& _availablePresentModes);
		vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& _capabilities);

		VulkanRenderDevice* device;

		vk::UniqueSurfaceKHR surface; // Vulkan window surface
		vk::UniqueSwapchainKHR swapchain;
		vk::SurfaceFormatKHR surfaceFormat;
		vk::PresentModeKHR presentMode;
		vk::Format format; // swapchain image format
		vk::Extent2D extent;

		Array<Shared<VulkanRenderCommandList>> commandLists;
		vk::CommandBuffer currentCommandBuffer;
		vk::Fence currentFence;

		Array<Shared<VulkanGPUTexture>> backBufferTextures;
		Array<Shared<VulkanTextureView>> backBufferRTVs;
		

		Array<vk::UniqueSemaphore> imageAvailableSemaphores;
		Array<vk::UniqueSemaphore> renderFinishedSemaphores;
		Array<vk::UniqueFence> inFlightFences;
		Array<vk::Image> swapchainImages;
		UInt32 imageCount = 0;

		UInt32 currentFrame = 0;
		UInt32 imageIndex = 0;
		GLFWwindow* window;
	};
}