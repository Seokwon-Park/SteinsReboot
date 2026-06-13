#include "DaydreamPCH.h"
#include "VulkanTextureView.h"
#include "VulkanUtility.h"
#include "Daydream/Graphics/Manager/SamplerRegistry.h"


namespace Daydream
{
	VulkanTextureView::VulkanTextureView(VulkanRenderDevice* _device, VulkanGPUTexture* _texture, const TextureViewDesc& _desc)
		:TextureView(_texture, _desc)
	{
		vk::Device device = _device->GetDevice();
		RenderFormat format = (_desc.format == RenderFormat::UNKNOWN) ? _texture->GetDesc().format : _desc.format;


		bool isArray = (
			(_texture->GetType() == TextureType::TextureCube ||
				_texture->GetType() == TextureType::Texture2DArray)
			&& _desc.layerCount > 1);

		vk::ImageViewType viewType = vk::ImageViewType::e2D;
		if (_texture->GetType() == TextureType::TextureCube && _desc.layerCount == 6)
		{
			viewType = vk::ImageViewType::eCube;
		}
		else if (isArray)
		{
			viewType = vk::ImageViewType::e2DArray;
		}

		vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eColor;
		if (GraphicsUtility::IsDepthFormat(format))
		{
			if (_desc.type == TextureViewType::DepthStencil)
			{
				aspectMask = vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
			}
			else if (_desc.type == TextureViewType::ShaderResource)
			{
				aspectMask = vk::ImageAspectFlagBits::eDepth;
			}
		}
		vk::ImageViewCreateInfo viewInfo{};
		viewInfo.image = _texture->GetVkImage();
		viewInfo.viewType = viewType;
		viewInfo.format = GraphicsUtility::Vulkan::ConvertToVkFormat(_texture->GetFormat());
		viewInfo.subresourceRange.aspectMask = aspectMask;
		viewInfo.subresourceRange.baseMipLevel = _desc.baseMip;
		viewInfo.subresourceRange.levelCount = _desc.mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = _desc.baseLayer;
		viewInfo.subresourceRange.layerCount = _desc.layerCount;

		imageView = device.createImageViewUnique(viewInfo);
	}
}

