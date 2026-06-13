#include "DaydreamPCH.h"
#include "VulkanSampler.h"
#include "VulkanUtility.h"

namespace Daydream
{
	VulkanSampler::VulkanSampler(VulkanRenderDevice* _device, const SamplerDesc& _desc)
	{
		device = _device;

		vk::SamplerCreateInfo samplerInfo = GraphicsUtility::Vulkan::TranslateToVkSamplerCreateInfo(_desc);
		sampler = device->GetDevice().createSamplerUnique(samplerInfo);
	}

	VulkanSampler::~VulkanSampler()
	{
		
	}
}

