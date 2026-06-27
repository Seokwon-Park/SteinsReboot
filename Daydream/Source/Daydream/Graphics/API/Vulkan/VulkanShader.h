#pragma once

#include "Daydream/Graphics/Resources/Shader.h"
#include "VulkanRenderDevice.h"

#include "vulkan/vulkan.h"

namespace Daydream
{
	class VulkanShader : public Shader
	{
	public:
		VulkanShader(VulkanRenderDevice* _device, const ShaderType& _type);
		virtual ~VulkanShader() override;

		virtual bool CreateNativeShader(const Array<UInt8>& _bytecode) override;

		vk::ShaderModule GetVkShaderModule() const { return shader.get(); }
	private:
		VulkanRenderDevice* device;
		vk::UniqueShaderModule shader;
		vk::ShaderStageFlagBits stageBit;
		// Inherited via Shader
	};
}