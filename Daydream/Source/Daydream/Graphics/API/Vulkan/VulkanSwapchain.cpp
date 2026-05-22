#include "DaydreamPCH.h"
#include "VulkanSwapchain.h"

#include "VulkanUtility.h"

namespace Daydream
{
	VulkanSwapchain::VulkanSwapchain(VulkanRenderDevice* _device, const DaydreamWindow& _window, const SwapchainDesc& _desc)
		:Swapchain(_desc)
	{
		device = _device;
		desc = _desc;
		window = Cast<GLFWwindow*>(_window.GetNativeWindow());
		//#if defined(DAYDREAM_PLATFORM_WINDOWS)
		//		VkWin32SurfaceCreateInfoKHR createInfo{};
		//		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		//		createInfo.hwnd = glfwGetWin32Window(window);
		//		createInfo.hinstance = GetModuleHandle(nullptr);
		//#endif
		VkSurfaceKHR tempSurface;
		VkResult result = glfwCreateWindowSurface(device->GetInstance(), window, nullptr, &tempSurface);
		if (result != VK_SUCCESS)
		{
			DAYDREAM_CORE_ERROR("Failed to create window surface!");
		}

		surface = vk::UniqueSurfaceKHR(tempSurface, vk::detail::ObjectDestroy<vk::Instance, vk::detail::DispatchLoaderDynamic>(device->GetInstance()));

		CreateSwapchain();
		CreateCommandLists();

		CreateBackBufferView();

		vk::SemaphoreCreateInfo semaphoreInfo{};
		vk::FenceCreateInfo fenceInfo{};
		fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

		imageAvailableSemaphores.resize(imageCount);
		renderFinishedSemaphores.resize(imageCount);
		inFlightFences.resize(imageCount);
		for (UInt32 i = 0; i < imageCount; i++)
		{
			imageAvailableSemaphores[i] = device->GetDevice().createSemaphoreUnique(semaphoreInfo);
			renderFinishedSemaphores[i] = device->GetDevice().createSemaphoreUnique(semaphoreInfo);
			inFlightFences[i] = device->GetDevice().createFenceUnique(fenceInfo);
		}


	}
	void VulkanSwapchain::CreateSwapchain()
	{
		SwapchainSupportDetails SwapchainSupport = device->QuerySwapchainSupport(*surface);
		if (SwapchainSupport.formats.empty() || SwapchainSupport.presentModes.empty())
		{
			DAYDREAM_CORE_ERROR("GPU has no supported formats or presentmodes");
		}

		surfaceFormat = ChooseSwapSurfaceFormat(SwapchainSupport.formats, desc.format);
		presentMode = ChooseSwapPresentMode(SwapchainSupport.presentModes);
		format = surfaceFormat.format;
		extent = ChooseSwapExtent(SwapchainSupport.capabilities);

		imageCount = desc.imageCount;

		// 서피스 제한 확인
		if (imageCount < SwapchainSupport.capabilities.minImageCount)
		{
			imageCount = SwapchainSupport.capabilities.minImageCount;
		}

		if (SwapchainSupport.capabilities.maxImageCount > 0 && imageCount > SwapchainSupport.capabilities.maxImageCount)
		{
			imageCount = SwapchainSupport.capabilities.maxImageCount;
		}

		vk::SwapchainCreateInfoKHR createInfo{};
		createInfo.surface = *surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
		createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		createInfo.preTransform = SwapchainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = *swapchain;

		vk::SwapchainKHR newSwapchain;

		newSwapchain = device->GetDevice().createSwapchainKHR(createInfo);

		//swapchain.release();

		swapchain = vk::UniqueSwapchainKHR(newSwapchain, vk::detail::ObjectDestroy<vk::Device, vk::detail::DispatchLoaderDynamic>(device->GetDevice()));

		swapchainImages = device->GetDevice().getSwapchainImagesKHR(swapchain.get());
			
	}
	void VulkanSwapchain::CreateCommandLists()
	{
		commandLists.assign(imageCount, MakeUnique<VulkanRenderCommandList>(device));
	}

	VulkanSwapchain::~VulkanSwapchain()
	{
		device->GetDevice().waitIdle();
	}
	void VulkanSwapchain::SetVSync(bool _enabled)
	{
	}
	void VulkanSwapchain::Present()
	{
		//EndFrame();

		vk::PresentInfoKHR presentInfo{};
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame].get();

		//vk::SwapchainKHR Swapchains[] = { swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain.get();
		presentInfo.pImageIndices = &imageIndex;

		try
		{
			vk::Result result = device->GetGraphicsQueue().presentKHR(presentInfo);
		}
		catch (const vk::OutOfDateKHRError&)
		{
			ResizeSwapchain();
		}

		currentFrame = (currentFrame + 1) % imageCount;
	}

	void VulkanSwapchain::BeginFrame()
	{
		if (isSwapchainResized)
		{
			ResizeSwapchain();
			isSwapchainResized = false;
		}

		//이미지를 GPU에 요청. 사용가능한 이미지의 인덱스를 imageIndex로 전달하고 imageAvailableSemaphore에 신호를 전달하라는 명령
		vk::Result result = device->GetDevice().acquireNextImageKHR(swapchain.get(), UINT64_MAX, imageAvailableSemaphores[currentFrame].get(), VK_NULL_HANDLE, &imageIndex);

		//if (result == vk::Result::eErrorOutOfDateKHR)
		//{
		//	ResizeSwapchain();
		//	return; 
		//}
		DAYDREAM_CORE_ASSERT(result == vk::Result::eSuccess || result == vk::Result::eSuboptimalKHR, "Failed to acquire swapchain image!");

		currentCommandBuffer = commandLists[currentFrame]->GetVkCommandBuffer();
		commandLists[currentFrame]->Begin();

		vk::ImageMemoryBarrier barrier{};
		barrier.image = backBufferTextures[imageIndex]->GetVkImage();
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.oldLayout = vk::ImageLayout::eUndefined;
		barrier.newLayout = vk::ImageLayout::eColorAttachmentOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eNone;
		barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

		// commandBuffer의 멤버 함수 pipelineBarrier 호출
		currentCommandBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eBottomOfPipe,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			{},
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

	}

	void VulkanSwapchain::EndFrame()
	{
		vk::ImageMemoryBarrier barrier{};
		barrier.image = backBufferTextures[imageIndex]->GetVkImage();
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
		barrier.newLayout = vk::ImageLayout::ePresentSrcKHR;
		barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eNone;

		// commandBuffer의 멤버 함수 pipelineBarrier 호출
		currentCommandBuffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::PipelineStageFlagBits::eBottomOfPipe,
			{},
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		auto fence = commandLists[currentFrame]->GetVkFence();
		currentCommandBuffer.end();

		vk::SubmitInfo submitInfo{};

		vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		submitInfo.waitSemaphoreCount = 1;
		//이 세마포어들에 신호가 다 와야 Submit을 한다.
		submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame].get();
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;

		submitInfo.pCommandBuffers = &currentCommandBuffer;

		//렌더링이 끝나면 여기다가 신호를 보내라.
		//VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame].get();

		vk::Result result = device->GetGraphicsQueue().submit(1, &submitInfo, fence);
	}

	void VulkanSwapchain::ResizeSwapchain()
	{
		device->GetDevice().waitIdle();

		backBufferTextures.clear();
		backBufferRTVs.clear();

		swapchainImages.clear();
		swapchain.reset();
		CreateSwapchain();
		swapchainImages = device->GetDevice().getSwapchainImagesKHR(swapchain.get());

		CreateBackBufferView();
	}

	vk::SurfaceFormatKHR VulkanSwapchain::ChooseSwapSurfaceFormat(const Array<vk::SurfaceFormatKHR>& _availableFormats, RenderFormat _desiredFormat)
	{
		vk::Format desiredFormat = GraphicsUtility::Vulkan::ConvertToVkFormat(_desiredFormat);
		for (const vk::SurfaceFormatKHR& availableFormat : _availableFormats)
		{
			if (availableFormat.format == desiredFormat && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			{
				return availableFormat;
			}
		}

		return _availableFormats[0];
	}
	vk::PresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(const Array<vk::PresentModeKHR>& _availablePresentModes)
	{
		for (const vk::PresentModeKHR& availablePresentMode : _availablePresentModes)
		{
			if (availablePresentMode == vk::PresentModeKHR::eMailbox)
			{
				return availablePresentMode;
			}
		}

		return vk::PresentModeKHR::eFifo;
	}
	vk::Extent2D VulkanSwapchain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& _capabilities)
	{
		if (_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return _capabilities.currentExtent;
		}
		else {
			vk::Extent2D actualExtent = {
				Cast<UInt32>(desc.width),
				Cast<UInt32>(desc.height)
			};

			actualExtent.width = std::clamp(actualExtent.width, _capabilities.minImageExtent.width, _capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, _capabilities.minImageExtent.height, _capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}


	void VulkanSwapchain::CreateBackBufferView()
	{
		TextureDesc textureDesc{};
		textureDesc.width = desc.width;
		textureDesc.height = desc.height;
		textureDesc.mipLevels = 1;
		textureDesc.sampleCount = 1;
		textureDesc.format = desc.format;
		textureDesc.textureUsage = TextureUsage::RenderTarget;
		textureDesc.type = TextureType::Texture2D;

		TextureViewDesc viewDesc;
		viewDesc.format = desc.format;
		viewDesc.type = TextureViewType::RenderTarget;

		backBufferTextures.resize(desc.imageCount);
		backBufferRTVs.resize(desc.imageCount);
		for (UInt32 i = 0; i < desc.imageCount; i++)
		{
			backBufferTextures[i] = MakeShared<VulkanGPUTexture>(device, textureDesc, swapchainImages[i]);
			backBufferRTVs[i] = MakeShared<VulkanTextureView>(device, backBufferTextures[i].get(), viewDesc);
		}
	}
}
