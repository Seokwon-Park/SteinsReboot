#include "DaydreamPCH.h"
#include "VulkanImGuiRenderer.h"
#include "VulkanSwapchain.h"
#include "VulkanUtility.h"

#include "backends/imgui_impl_vulkan.h"

namespace Daydream
{
	VulkanImGuiRenderer::VulkanImGuiRenderer(VulkanRenderDevice* _device)
	{
		device = _device;
	}

	void VulkanImGuiRenderer::Init(DaydreamWindow* _window)
	{
		ImGuiRenderer::Init(_window);
		ImGui_ImplVulkan_InitInfo info{};
		//info.ApiVersion =;
		info.Instance = device->GetInstance();
		info.PhysicalDevice = device->GetPhysicalDevice();
		info.Device = device->GetDevice();
		info.QueueFamily = device->GetGraphicsFamilyIndex();
		info.Queue = device->GetGraphicsQueue();
		info.DescriptorPool = device->GetDescriptorPool();
		info.MinImageCount = 3;
		info.ImageCount = 3;

		static VkFormat colorFormat = (VkFormat)GraphicsUtility::Vulkan::ConvertToVkFormat(_window->GetSwapchain()->GetDesc().format);
		info.UseDynamicRendering = true;
		info.PipelineInfoMain.PipelineRenderingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
		info.PipelineInfoMain.PipelineRenderingCreateInfo.pNext = nullptr;
		info.PipelineInfoMain.PipelineRenderingCreateInfo.colorAttachmentCount = 1;
		info.PipelineInfoMain.PipelineRenderingCreateInfo.pColorAttachmentFormats = &colorFormat;
		info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		info.UsePushDescriptorSet = true;

		ImGui_ImplVulkan_Init(&info);
	}
	void VulkanImGuiRenderer::Shutdown()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGuiRenderer::Shutdown();
	}
	void VulkanImGuiRenderer::NewFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGuiRenderer::NewFrame();
	}
	void VulkanImGuiRenderer::RenderDrawData(RenderCommandList* _activeCommandList, ImDrawData* _drawData)
	{
		ImGuiRenderer::RenderDrawData(_activeCommandList, _drawData);
		ImGui_ImplVulkan_RenderDrawData(_drawData, Cast<VulkanRenderCommandList*>(_activeCommandList)->GetVkCommandBuffer());
	}
}
