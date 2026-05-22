#pragma once

#include "VulkanTexture.h"
#include "VulkanRenderDevice.h"

#include "Daydream/Graphics/Resources/Texture/TextureView.h"

namespace Daydream
{
	class VulkanTextureView : public TextureView
	{
	public:
		VulkanTextureView(VulkanRenderDevice* _device, VulkanGPUTexture* _texture, const TextureViewDesc& _desc);
		~VulkanTextureView() override = default;

		virtual void* GetUIHandle() const { return imageView.get(); }

		void SetImageView(vk::UniqueImageView _imageView) { imageView = std::move(_imageView); }
		vk::ImageView GetVkImageView() const { return imageView.get(); }

	private:
		VulkanRenderDevice* device;
		TextureViewDesc desc;
		vk::UniqueImageView imageView;
	};
}