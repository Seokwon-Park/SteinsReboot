#include "DaydreamPCH.h"
#include "VulkanTexture.h"

#include "VulkanUtility.h"
#include "Daydream/Graphics/Utility/ImageLoader.h"

namespace Daydream
{
	VulkanGPUTexture::VulkanGPUTexture(VulkanRenderDevice* _device, const TextureDesc& _desc)
		:GPUTexture(_desc)
	{
		vk::ImageCreateInfo imageInfo = GraphicsUtility::Vulkan::ConvertToVulkanCreateInfo(_desc);
		vma::AllocationCreateInfo allocInfo = GraphicsUtility::Vulkan::ConvertToVMAAllocationInfo(_desc);
		
		std::tie(ownedImage, ownedImageAllocation) = _device->GetAllocator().createImageUnique(imageInfo, allocInfo);

		image = ownedImage.get();
	}

	VulkanGPUTexture::VulkanGPUTexture(VulkanRenderDevice* _device, const TextureDesc& _desc, vk::Image _image)
		:GPUTexture(_desc)
	{
		image = _image;
	}

	//VulkanTexture2D::VulkanTexture2D(VulkanRenderDevice* _device, const TextureDesc& _desc)
	//	:Texture2D(_desc)
	//{
	//	device = _device;

	//	vk::Format imageFormat = GraphicsUtility::Vulkan::ConvertToVkFormat(_desc.format);

	//	vk::ImageCreateInfo imageInfo{};
	//	vma::AllocationCreateInfo allocInfo{};

	//	imageInfo.imageType = vk::ImageType::e2D;
	//	imageInfo.extent.width = static_cast<UInt32>(desc.width);
	//	imageInfo.extent.height = static_cast<UInt32>(desc.height);
	//	imageInfo.extent.depth = 1;
	//	imageInfo.mipLevels = 1;
	//	imageInfo.arrayLayers = 1;
	//	imageInfo.format = imageFormat;
	//	imageInfo.tiling = vk::ImageTiling::eOptimal;
	//	imageInfo.initialLayout = vk::ImageLayout::eUndefined;
	//	imageInfo.usage = GraphicsUtility::Vulkan::ConvertToVkImageUsageFlags(_desc.textureUsage);
	//	imageInfo.sharingMode = vk::SharingMode::eExclusive;
	//	imageInfo.samples = vk::SampleCountFlagBits::e1;

	//	allocInfo.usage = vma::MemoryUsage::eGpuOnly;
	//	allocInfo.flags = {};

	//	std::tie(textureImage, textureImageAllocation) = device->CreateImage(imageInfo, allocInfo);

	//	if (imageFormat == vk::Format::eD24UnormS8Uint)
	//	{
	//		//barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;

	//		//device->TransitionImageLayout(barrier);

	//		srv = device->CreateImageView(textureImage.get(), imageFormat,
	//			vk::ImageAspectFlagBits::eDepth);
	//		dsv = device->CreateImageView(textureImage.get(), imageFormat,
	//			vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil);
	//	}
	//	else
	//	{
	//		//barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;

	//		//device->TransitionImageLayout(barrier);

	//		srv = device->CreateImageView(textureImage.get(), imageFormat,
	//			vk::ImageAspectFlagBits::eColor);
	//	}
	//}


	//VulkanTexture2D::~VulkanTexture2D()
	//{
	//	if (ImGuiDescriptorSet != VK_NULL_HANDLE)
	//	{
	//		ImGui_ImplVulkan_RemoveTexture(ImGuiDescriptorSet);
	//		ImGuiDescriptorSet = VK_NULL_HANDLE;
	//	}
	//}

	//void VulkanTexture2D::SetSampler(Shared<Sampler> _sampler)
	//{
	//	textureSampler = static_cast<VulkanSampler*>(_sampler.get());
	//}

	//void* VulkanTexture2D::GetImGuiHandle()
	//{
	//	if (ImGuiDescriptorSet != VK_NULL_HANDLE) return ImGuiDescriptorSet;
	//	auto imguiSampler = static_cast<VulkanSampler*>(ResourceManager::GetResource<Sampler>("LinearRepeat").get());
	//	return ImGuiDescriptorSet = ImGui_ImplVulkan_AddTexture(imguiSampler->GetSampler(), srv.get(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	//}

}
