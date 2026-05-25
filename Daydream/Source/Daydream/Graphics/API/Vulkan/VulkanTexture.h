#pragma once

#include "Daydream/Graphics/Resources/Texture/Texture.h"
#include "VulkanRenderDevice.h"
#include "VulkanSampler.h"

namespace Daydream
{
	class VulkanGPUTexture : public GPUTexture
	{
	public:
		VulkanGPUTexture(VulkanRenderDevice* _device, const TextureDesc& _desc);
		VulkanGPUTexture(VulkanRenderDevice* _device, const TextureDesc& _desc, vk::Image _vulkanBackBuffer);
		virtual ~VulkanGPUTexture() = default;

		inline vk::Image GetVkImage() const { return image; }
	private:
		vk::Image image;

		vma::UniqueImage ownedImage;
		vma::UniqueAllocation ownedImageAllocation;
	};

	//class VulkanTexture2D :public Texture2D
	//{
	//public:
	//	VulkanTexture2D(VulkanRenderDevice* _device, const TextureDesc& _desc); // Dynamic(RenderTarget, ShaderResource...)
	//	//VulkanTexture2D(VulkanRenderDevice* _device, const FilePath& _path, const TextureDesc& _desc); // FileLoad
	//	//VulkanTexture2D(VulkanRenderDevice* _device, VkImage _image, VkFormat _format); // ForSwapchain
	//	virtual ~VulkanTexture2D();

	//	virtual void SetSampler(Shared<Sampler> _sampler) override;
	//	virtual bool HasSampler() override { return textureSampler != nullptr; }

	//	virtual void* GetImGuiHandle() override;

	//	vk::Image GetVkImage() { return textureImage.get(); }
	//	vk::ImageView GetImageView() { return srv.get(); }
	//	vk::Sampler GetSampler() { return textureSampler->GetSampler(); }

	//protected:

	//private:
	//	bool isSwapchainImage = false;
	//	VulkanRenderDevice* device;
	//	VulkanSampler* textureSampler = nullptr;

	//	vma::UniqueImage textureImage;
	//	vma::UniqueAllocation textureImageAllocation;
	//	vk::UniqueImageView srv;
	//	vk::UniqueImageView dsv;

	//	//ImGui Image¿ë
	//	VkDescriptorSet ImGuiDescriptorSet = VK_NULL_HANDLE;
	//};
}

