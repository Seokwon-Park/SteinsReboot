#pragma once

#include "vulkan/vulkan.hpp"
#include "vk_mem_alloc.hpp"

#include "Daydream/Enum/RendererEnums.h"
#include "Daydream/Graphics/Utility/GraphicsUtility.h"

#include "Daydream/Graphics/Resources/Buffer.h"
#include "Daydream/Graphics/Resources/Texture/Texture.h"
#include "Daydream/Graphics/Resources/Sampler.h"
#include "Daydream/Graphics/States/RasterizerState.h"


namespace Daydream::GraphicsUtility::Vulkan
{
	Pair<vk::PipelineStageFlags, vk::AccessFlags> ConvertToVulkanStageAndAccess(ResourceState _state);
	vk::ImageLayout ConvertToVulkanImageLayout(ResourceState _state);
	vk::ImageAspectFlags GetImageAspectFlags(RenderFormat _format);

	vk::BufferCreateInfo ConvertToVkImageCreateInfo(const BufferDesc& _desc);
	vma::AllocationCreateInfo ConvertToVMAAllocationInfo(const BufferDesc& _desc);

	vk::ImageCreateInfo ConvertToVulkanCreateInfo(const TextureDesc& _desc);
	vma::AllocationCreateInfo ConvertToVMAAllocationInfo(const TextureDesc& _desc);

	vk::AttachmentLoadOp ConvertToLoadOp(AttachmentLoadOp op);
	vk::AttachmentStoreOp ConvertToStoreOp(AttachmentStoreOp op);

	vk::Format ConvertToVkFormat(RenderFormat _format);
	vk::ShaderStageFlagBits ConvertToShaderStageFlagBit(ShaderType _type);
	vk::ImageUsageFlags ConvertToVkImageUsageFlags(TextureUsage usageFlags);

	vk::SamplerAddressMode ConvertToVkAddressMode(WrapMode _wrapMode);
	vk::Filter ConvertToVkFilter(FilterMode _filter);
	vk::SamplerMipmapMode ConvertToVkMipmapMode(FilterMode _mipMapFilter);
	vk::CompareOp ConvertToVkCompareOp(ComparisonFunc _func);

	vk::CullModeFlags ConvertToVulkanCullMode(const CullMode& _cullMode);
	vk::PolygonMode ConvertToVulkanFillMode(const FillMode& _fillMode);

	vk::SamplerCreateInfo TranslateToVulkanSamplerCreateInfo(const SamplerDesc& _desc);
	vk::PipelineRasterizationStateCreateInfo TranslateToVulkanRasterizerCreateInfo(const RasterizerStateDesc& _desc);

}