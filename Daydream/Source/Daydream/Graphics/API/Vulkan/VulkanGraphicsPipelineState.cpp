#include "DaydreamPCH.h"
#include "VulkanGraphicsPipelineState.h"

#include "VulkanSwapchain.h"
#include "VulkanUtility.h"
#include "Daydream/Graphics/Utility/GraphicsUtility.h"

#include "VulkanTexture.h"
#include "VulkanShader.h"

namespace Daydream
{
	vk::DescriptorType ToVkDescType(ShaderResourceType _type)
	{
		switch (_type)
		{
		case ShaderResourceType::ConstantBuffer:
			return vk::DescriptorType::eUniformBuffer;
			break;
		case ShaderResourceType::Texture:
			return vk::DescriptorType::eCombinedImageSampler;
			break;
		default:
			break;
		}
		DAYDREAM_CORE_ASSERT(false, "Wrong type")
			return {};
	}

	VulkanGraphicsPipelineState::VulkanGraphicsPipelineState(VulkanRenderDevice* _device, const GraphicsPipelineStateDesc& _desc)
		:GraphicsPipelineState(_desc)
	{
		device = _device;
		
		vk::VertexInputBindingDescription vertexInputdesc;
		vertexInputdesc.binding = 0;
		vertexInputdesc.stride = 0;
		vertexInputdesc.inputRate = vk::VertexInputRate::eVertex;

		UInt32 offset = 0;
		Array<vk::VertexInputAttributeDescription> attribDescArray;
		for (const auto& info : shaderGroup->GetInputData())
		{
			vk::VertexInputAttributeDescription attribDesc{};
			attribDesc.location = info.set;
			attribDesc.binding = info.binding;
			attribDesc.format = GraphicsUtility::Vulkan::ConvertToVkFormat(info.format);
			attribDesc.offset = offset;

			offset += (UInt32)info.size;
			vertexInputdesc.stride += (UInt32)info.size;

			attribDescArray.push_back(attribDesc);
		}

		for (Shader* shader : shaderGroup->GetShaders())
		{
			entryPoints[shader->GetType()] = GraphicsUtility::GetShaderEntryPointName(shader->GetType());

			VulkanShader* vulkanShader = Cast<VulkanShader*>(shader);

			vk::PipelineShaderStageCreateInfo shaderStageInfo{};
			shaderStageInfo.module = vulkanShader->GetVkShaderModule();
			shaderStageInfo.pName = entryPoints[shader->GetType()].c_str();
			shaderStageInfo.stage = GraphicsUtility::Vulkan::ConvertToShaderStageFlagBit(shader->GetType());

			shaderStages.push_back(shaderStageInfo);
		}

		UInt32 setCount = shaderGroup->GetSetCount();
		Array<Array<vk::DescriptorSetLayoutBinding>> setBindings(setCount);
		for (const auto& [name, data] : shaderGroup->GetShaderBindingMap())
		{
			vk::DescriptorSetLayoutBinding binding = {};
			binding.binding = data.binding;
			binding.descriptorType = ToVkDescType(data.shaderResourceType);
			binding.descriptorCount = 1;
			binding.stageFlags = GraphicsUtility::Vulkan::ConvertToShaderStageFlagBit(data.shaderType);
			binding.pImmutableSamplers = nullptr;
			setBindings[data.set].push_back(binding);
		}

		//descriptorSetLayouts.resize(setCount);

		for (const auto& bindings : setBindings)
		{
			Array<vk::DescriptorBindingFlags> bindingFlags(bindings.size(), vk::DescriptorBindingFlagBits::eUpdateAfterBind);

			vk::DescriptorSetLayoutBindingFlagsCreateInfo extendedInfo{};
			extendedInfo.bindingCount = (UInt32)bindingFlags.size();
			extendedInfo.pBindingFlags = bindingFlags.data();

			vk::DescriptorSetLayoutCreateInfo layoutCreateInfo{};
			//layoutCreateInfo.pNext = &extendedInfo; // 확장 정보 연결!
			layoutCreateInfo.bindingCount = Cast<UInt32>(bindings.size());
			layoutCreateInfo.pBindings = bindings.data();
			layoutCreateInfo.flags = /*vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool |*/ 
				vk::DescriptorSetLayoutCreateFlagBits::ePushDescriptor;

			descriptorSetLayouts.push_back(device->GetDevice().createDescriptorSetLayoutUnique(layoutCreateInfo));
		}

		rawDescriptorSetLayouts.clear();
		for (const auto& uniqueLayout : descriptorSetLayouts)
		{
			rawDescriptorSetLayouts.push_back(uniqueLayout.get());
		}

		Array<vk::DynamicState> dynamicStates =
		{
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor,
		};

		vk::PipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.dynamicStateCount = static_cast<UInt32>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		vk::PipelineViewportStateCreateInfo viewportState{};
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &vertexInputdesc; // Optional
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<UInt32>(attribDescArray.size());
		vertexInputInfo.pVertexAttributeDescriptions = attribDescArray.data(); // Optional

		vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
		

		vk::PipelineRasterizationStateCreateInfo rasterizer = GraphicsUtility::Vulkan::TranslateToVulkanRasterizerCreateInfo(_desc.rasterizerState);
		//vk::PipelineRasterizationStateCreateInfo rasterizer{};
		//rasterizer.depthClampEnable = VK_FALSE;
		//rasterizer.rasterizerDiscardEnable = VK_FALSE;
		//rasterizer.polygonMode = vk::PolygonMode::eFill;
		//rasterizer.lineWidth = 1.0f;
		//rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		//rasterizer.frontFace = vk::FrontFace::eClockwise;
		//rasterizer.depthBiasEnable = VK_FALSE;
		//rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		//rasterizer.depthBiasClamp = 0.0f; // Optional
		//rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		vk::PipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR
			| vk::ColorComponentFlagBits::eG
			| vk::ColorComponentFlagBits::eB
			| vk::ColorComponentFlagBits::eA;
		colorBlendAttachment.blendEnable = VK_FALSE;
		//colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne; // Optional
		//colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero; // Optional
		//colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd; // Optional
		//colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne; // Optional
		//colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
		//colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd; // Optional

		UInt32 colorAttachmentCount = (UInt32)desc.renderTargetFormats.size();

		std::vector<vk::PipelineColorBlendAttachmentState> blendAttachmentStates;
		blendAttachmentStates.resize(colorAttachmentCount, colorBlendAttachment);

		vk::PipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = vk::LogicOp::eCopy; // Optional
		colorBlending.attachmentCount = colorAttachmentCount;
		colorBlending.pAttachments = blendAttachmentStates.data();
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		vk::PipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.pNext = nullptr; // 확장기능
		depthStencil.flags = {}; // 나중에 사용될 예약된 필드
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = vk::CompareOp::eLess;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_TRUE;
		depthStencil.front.failOp = vk::StencilOp::eKeep;           // 스텐실 테스트 실패 시
		depthStencil.front.passOp = vk::StencilOp::eKeep;           // 스텐실 테스트 통과, 깊이 테스트 통과 시
		depthStencil.front.depthFailOp = vk::StencilOp::eKeep;      // 스텐실 테스트 통과, 깊이 테스트 실패 시
		depthStencil.front.compareOp = vk::CompareOp::eAlways;      // 스텐실 비교 연산
		depthStencil.front.compareMask = 0xFF;                    // 비교 마스크
		depthStencil.front.writeMask = 0xFF;                      // 쓰기 마스크
		depthStencil.front.reference = 0;                         // 참조 값

		depthStencil.back = depthStencil.front;

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.setLayoutCount = static_cast<UInt32>(rawDescriptorSetLayouts.size()); // Optional
		pipelineLayoutInfo.pSetLayouts = rawDescriptorSetLayouts.data(); // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		pipelineLayout = device->GetDevice().createPipelineLayoutUnique(pipelineLayoutInfo);

		vk::PipelineRenderingCreateInfo renderingInfo{};
		Array<vk::Format> colorFormats(colorAttachmentCount);
		
		for (UInt64 i = 0; i < colorFormats.size(); i++)
		{
			colorFormats[i] = GraphicsUtility::Vulkan::ConvertToVkFormat(desc.renderTargetFormats[i]);
		}
		renderingInfo.colorAttachmentCount = colorAttachmentCount;
		renderingInfo.pColorAttachmentFormats = colorFormats.data();
		renderingInfo.depthAttachmentFormat = desc.depthStencilFormat == RenderFormat::UNKNOWN ?
			vk::Format::eUndefined : 
			GraphicsUtility::Vulkan::ConvertToVkFormat(desc.depthStencilFormat); // 깊이 버퍼 사용 시

		vk::GraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.stageCount = (UInt32)shaderStages.size();
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipelineLayout.get();
		pipelineInfo.renderPass = VK_NULL_HANDLE;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional
		pipelineInfo.pNext = &renderingInfo;

		vk::ResultValue<vk::UniquePipeline> resultValue = device->GetDevice().createGraphicsPipelineUnique({}, pipelineInfo);
		pipeline = std::move(resultValue.value);
	}

	VulkanGraphicsPipelineState::~VulkanGraphicsPipelineState()
	{
	}
	void VulkanGraphicsPipelineState::Bind() const
	{
		//vkCmdBindDescriptorSets(device->GetCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSets.data(), 0, nullptr);
	}
	//Shared<Material> VulkanPipelineState::CreateMaterial()
	//{
	//	return MakeShared<VulkanMaterial>(device, this);
	//}
}
