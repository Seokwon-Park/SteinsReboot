#include "SteinsPCH.h"
#include "VulkanSwapchain.h"

#include "Steins/Graphics/Utility/GraphicsUtil.h"

namespace Steins
{
	VulkanSwapchain::VulkanSwapchain(VulkanRenderDevice* _device, SteinsWindow* _window, const SwapchainDesc& _desc)
	{
		device = _device;
		desc = _desc;
		GLFWwindow* window = Cast<GLFWwindow>(_window->GetNativeWindow());
		//#if defined(STEINS_PLATFORM_WINDOWS)
		//		VkWin32SurfaceCreateInfoKHR createInfo{};
		//		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		//		createInfo.hwnd = glfwGetWin32Window(window);
		//		createInfo.hinstance = GetModuleHandle(nullptr);
		//#endif
		{
			VkResult result = glfwCreateWindowSurface(device->GetInstance(), window, nullptr, &surface);
			if (result != VK_SUCCESS) {
				STEINS_CORE_ERROR("Failed to create window surface!");
			}
		}

		SwapchainSupportDetails SwapchainSupport = device->QuerySwapchainSupport(surface);
		if (SwapchainSupport.formats.empty() || SwapchainSupport.presentModes.empty())
		{
			STEINS_CORE_ERROR("GPU has no supported formats or presentmodes");
		}


		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(SwapchainSupport.formats, _desc.format);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(SwapchainSupport.presentModes);
		format = surfaceFormat.format;
		extent = ChooseSwapExtent(SwapchainSupport.capabilities);

		imageCount = desc.bufferCount;

		// 서피스 제한 확인
		if (imageCount < SwapchainSupport.capabilities.minImageCount)
		{
			imageCount = SwapchainSupport.capabilities.minImageCount;
		}

		if (SwapchainSupport.capabilities.maxImageCount > 0 && imageCount > SwapchainSupport.capabilities.maxImageCount)
		{
			imageCount = SwapchainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.preTransform = SwapchainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VkResult result = vkCreateSwapchainKHR(device->GetDevice(), &createInfo, nullptr, &swapchain);
		STEINS_CORE_ASSERT(VK_SUCCEEDED(result), "Failed to create Swapchain");
		device->AddSwapchain(this);

		commandBuffers.resize(imageCount);
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = device->GetCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = commandBuffers.size();

		if (vkAllocateCommandBuffers(device->GetDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		imageAvailableSemaphores.resize(imageCount);
		renderFinishedSemaphores.resize(imageCount);
		inFlightFences.resize(imageCount);
		for (UInt32 i = 0; i < imageCount; i++)
		{
			result = vkCreateSemaphore(device->GetDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
			STEINS_CORE_ASSERT(VK_SUCCEEDED(result), "Failed to create semaphore!");
			result = vkCreateSemaphore(device->GetDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
			STEINS_CORE_ASSERT(VK_SUCCEEDED(result), "Failed to create semaphore!");
			result = vkCreateFence(device->GetDevice(), &fenceInfo, nullptr, &inFlightFences[i]);
			STEINS_CORE_ASSERT(VK_SUCCEEDED(result), "Failed to create fence!");
		}

		framebuffers.resize(desc.bufferCount);
		for (UInt32 i = 0; i < desc.bufferCount; i++)
		{
			framebuffers[i] = MakeShared<VulkanFramebuffer>(device, this, i);
		}
		BeginFrame();
		//framebuffers[imageIndex]->Begin();
	}
	VulkanSwapchain::~VulkanSwapchain()
	{
		vkDeviceWaitIdle(device->GetDevice());
		for (UInt32 i = 0; i < imageCount; i++)
		{
			vkDestroySemaphore(device->GetDevice(), imageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(device->GetDevice(), renderFinishedSemaphores[i], nullptr);
			vkDestroyFence(device->GetDevice(), inFlightFences[i], nullptr);
		}

		vkDestroySwapchainKHR(device->GetDevice(), swapchain, nullptr);
		vkDestroySurfaceKHR(device->GetInstance(), surface, nullptr);
	}
	void VulkanSwapchain::SetVSync(bool _enabled)
	{
	}
	void VulkanSwapchain::SwapBuffers()
	{
		//framebuffers[imageIndex]->End();
		//vkCmdEndRenderPass(cmdbuf);
		EndFrame();

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		//이 세마포어들에 신호가 다 와야 Submit을 한다.
		submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;

		VkCommandBuffer buffers[] = { device->GetCommandBuffer() };
		submitInfo.pCommandBuffers = buffers;

		//렌더링이 끝나면 여기다가 신호를 보내라.
		//VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];

		if (vkQueueSubmit(device->GetQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];

		VkSwapchainKHR Swapchains[] = { swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &swapchain;
		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(device->GetQueue(), &presentInfo);

		currentFrame = (currentFrame + 1) % imageCount;

		BeginFrame();
		//framebuffers[currentFrame]->Begin();
	}

	void VulkanSwapchain::ResizeSwapchain(UInt32 _width, UInt32 height)
	{
	}

	void VulkanSwapchain::BeginFrame()
	{
		//이전 프레임의 GPU 작업 완료됐다는 신호를 inFlightFence로 받기로 하고 대기
		vkWaitForFences(device->GetDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
		//완료 됐으면 펜스 상태는 신호받기 전으로
		vkResetFences(device->GetDevice(), 1, &inFlightFences[currentFrame]);

		//이미지를 GPU에 요청. 사용가능한 이미지의 인덱스를 imageIndex로 전달하고 imageAvailableSemaphore에 신호를 전달하라는 명령
		vkAcquireNextImageKHR(device->GetDevice(), swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		//이미지 요청만 해놓고 일단 커맨드 받기 시작
		device->SetCommandBuffer(commandBuffers[currentFrame]);
		vkResetCommandBuffer(commandBuffers[currentFrame], 0);
		//fb->Bind();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VkResult result = vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo);
		STEINS_CORE_ASSERT(result == VK_SUCCESS, "Failed to begin recording command buffer!");
	}

	void VulkanSwapchain::EndFrame()
	{
		if (vkEndCommandBuffer(commandBuffers[currentFrame]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	VkSurfaceFormatKHR VulkanSwapchain::ChooseSwapSurfaceFormat(const Array<VkSurfaceFormatKHR>& _availableFormats, RenderFormat _desiredFormat)
	{
		VkFormat desiredFormat = GraphicsUtil::RenderFormatToVkFormat(_desiredFormat);
		for (const VkSurfaceFormatKHR& availableFormat : _availableFormats)
		{
			if (availableFormat.format == desiredFormat && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return _availableFormats[0];
	}
	VkPresentModeKHR VulkanSwapchain::ChooseSwapPresentMode(const Array<VkPresentModeKHR>& _availablePresentModes)
	{
		for (const VkPresentModeKHR& availablePresentMode : _availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}
	VkExtent2D VulkanSwapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& _capabilities)
	{
		if (_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return _capabilities.currentExtent;
		}
		else {
			VkExtent2D actualExtent = {
				Cast<UInt32>(desc.width),
				Cast<UInt32>(desc.height)
			};

			actualExtent.width = std::clamp(actualExtent.width, _capabilities.minImageExtent.width, _capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, _capabilities.minImageExtent.height, _capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}
	//	void VulkanSwapchain::recordCommandBuffer(VkCommandBuffer _commandBuffer, UInt32 _imageIndex)
	//	{
	//		VkCommandBufferBeginInfo beginInfo{};
	//		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	//
	//		VkResult result = vkBeginCommandBuffer(_commandBuffer, &beginInfo);
	//		STEINS_CORE_ASSERT(result == VK_SUCCESS, "Failed to begin recording command buffer!");
	//
	//		//VkFramebuffer buf = ((VulkanFramebuffer*)backFramebuffer.get())->GetFrameBuffers()[_imageIndex];
	//
	//		//VkRenderPassBeginInfo renderPassInfo{};
	//		//renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	//		//renderPassInfo.renderPass = device->GetMainRenderPass();
	//		//renderPassInfo.framebuffer = buf;
	//		//renderPassInfo.renderArea.offset = { 0, 0 };
	//		//renderPassInfo.renderArea.extent = extent;
	//
	//		//VkClearValue clearColor = { {1.0f, 1.0f, 1.0f, 1.0f} };
	//		//renderPassInfo.clearValueCount = 1;
	//		//renderPassInfo.pClearValues = &clearColor;
	//
	//		//vkCmdBeginRenderPass(_commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	//
	//		framebuffers[_imageIndex]->Begin();
	//
	//		VkViewport viewport{};
	//		viewport.x = 0.0f;
	//		viewport.y = (float)extent.height;
	//		viewport.width = (float)extent.width;
	//		viewport.height = -(float)extent.height;
	//		viewport.minDepth = 0.0f;
	//		viewport.maxDepth = 1.0f;
	//		vkCmdSetViewport(_commandBuffer, 0, 1, &viewport);
	//
	//		VkRect2D scissor{};
	//		scissor.offset = { 0, 0 };
	//		scissor.extent = extent;
	//		vkCmdSetScissor(_commandBuffer, 0, 1, &scissor);
	//
	////		vkCmdDraw(_commandBuffer, 3, 1, 0, 0);
	//
	//
	//	}
}
