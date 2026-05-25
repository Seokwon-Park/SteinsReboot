#include "DaydreamPCH.h"
#include "VulkanRenderDevice.h"
#include "VulkanRenderContext.h"
#include "VulkanRenderCommandList.h"
#include "VulkanBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanGraphicsPipelineState.h"
#include "VulkanShader.h"
#include "VulkanSwapchain.h"
#include "VulkanImGuiRenderer.h"
#include "VulkanTexture.h"
#include "VulkanTextureView.h"
#include "VulkanTextureCube.h"
#include "VulkanSampler.h"
#include "Daydream/Graphics/Utility/GraphicsUtility.h"
#include "Daydream/Graphics/Utility/ImageLoader.h"

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include "backends/imgui_impl_vulkan.h"

namespace vk
{
	namespace detail
	{
		DispatchLoaderDynamic defaultDispatchLoaderDynamic;
	}
}

namespace Daydream
{
	namespace
	{
		static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(
			vk::DebugUtilsMessageSeverityFlagBitsEXT _messageSeverity,
			vk::DebugUtilsMessageTypeFlagsEXT _messageType,
			const vk::DebugUtilsMessengerCallbackDataEXT* _pCallbackData,
			void* _pUserData)
		{
			if (_messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
			{
				DAYDREAM_CORE_ERROR("Validation layer: {0}", _pCallbackData->pMessage);
			}
			else if (_messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
			{
				DAYDREAM_CORE_WARN("Validation layer: {0}", _pCallbackData->pMessage);
			}
			else if (_messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo)
			{
				DAYDREAM_CORE_INFO("Validation layer: {0}", _pCallbackData->pMessage);
			}
			else if (_messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose)
			{
				DAYDREAM_CORE_TRACE("Validation layer: {0}", _pCallbackData->pMessage);
			}

			return VK_FALSE;
		}

		static VkResult CreateDebugUtilsMessengerEXT(VkInstance _instance, const VkDebugUtilsMessengerCreateInfoEXT* _pCreateInfo, const VkAllocationCallbacks* _pAllocator, VkDebugUtilsMessengerEXT* _pDebugMessenger)
		{
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT");
			if (func != nullptr)
			{
				return func(_instance, _pCreateInfo, _pAllocator, _pDebugMessenger);
			}
			else
			{
				return VK_ERROR_EXTENSION_NOT_PRESENT;
			}
		}

		static void DestroyDebugUtilsMessengerEXT(VkInstance _instance, VkDebugUtilsMessengerEXT _debugMessenger, const VkAllocationCallbacks* _pAllocator)
		{
			auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT");
			if (func != nullptr)
			{
				func(_instance, _debugMessenger, _pAllocator);
			}
		}
	}

	VulkanRenderDevice::VulkanRenderDevice()
	{
		API = RendererAPIType::Vulkan;
	}

	VulkanRenderDevice::~VulkanRenderDevice()
	{
	}

	void VulkanRenderDevice::Init()
	{
		CreateInstance();
		SetupDebugMessenger();
		PickPhysicalDevice();
		CreateLogicalDevice();

		vk::CommandPoolCreateInfo poolCreateInfo{};
		poolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		poolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		commandPool = device->createCommandPoolUnique(poolCreateInfo, nullptr);

		{
			Array<vk::DescriptorPoolSize> poolSizes =
			{
				{ vk::DescriptorType::eCombinedImageSampler , IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE },
				{ vk::DescriptorType::eCombinedImageSampler , 1024 },
				{ vk::DescriptorType::eSampler, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE },
				{ vk::DescriptorType::eUniformBuffer , 256 },
			};


			vk::DescriptorPoolCreateInfo poolInfo = {};
			poolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet | vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind;
			poolInfo.maxSets = 0;
			for (vk::DescriptorPoolSize& poolSize : poolSizes)
				poolInfo.maxSets += poolSize.descriptorCount;
			poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
			poolInfo.pPoolSizes = poolSizes.data();
			descriptorPool = device->createDescriptorPoolUnique(poolInfo, nullptr);
		}

		vma::AllocatorCreateInfo allocatorInfo;
		allocatorInfo.physicalDevice = physicalDevice; // vk::PhysicalDevice
		allocatorInfo.device = device.get();             // vk::Device
		allocatorInfo.instance = instance.get();           // vk::Instance
		allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_4; // 사용 중인 Vulkan 버전

		allocator = vma::createAllocatorUnique(allocatorInfo);

		vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
		info.APIName = "Vulkan Info:";
		info.vendor = GraphicsUtility::GetVendor(properties.vendorID);
		info.physicalDeviceInfo = properties.deviceName.data();
		info.version = VK_API_VERSION_MAJOR(properties.apiVersion) + '.' + VK_API_VERSION_MINOR(properties.apiVersion) + '.' + VK_API_VERSION_PATCH(properties.apiVersion);
	}

	void VulkanRenderDevice::Shutdown()
	{
	}


	Unique<RenderContext> VulkanRenderDevice::CreateContext()
	{
		return MakeUnique<VulkanRenderContext>(this);
	}

	Shared<RenderCommandList> VulkanRenderDevice::CreateRenderCommandList()
	{
		return MakeShared<VulkanRenderCommandList>(this);
	}

	Shared<GPUBuffer> VulkanRenderDevice::CreateGPUBuffer(const BufferDesc& _desc)
	{
		return MakeShared<VulkanGPUBuffer>(this, _desc);
	}

	Shared<GPUTexture> VulkanRenderDevice::CreateGPUTexture(const TextureDesc & _desc)
	{
		return MakeShared<VulkanGPUTexture>(this, _desc);
	}

	Shared<TextureView> VulkanRenderDevice::CreateTextureView(Texture* _texture, const TextureViewDesc& _desc)
	{
		VulkanGPUTexture* texture = Cast<VulkanGPUTexture*>(_texture->GetGPUTexture());
		return MakeShared<VulkanTextureView>(this, texture, _desc);
	}

	//Shared<VertexBuffer> VulkanRenderDevice::CreateDynamicVertexBuffer(UInt32 _size, UInt32 _stride, UInt32 _initialDataSize, const void* _initialData)
	//{
	//	BufferDesc desc{};
	//	desc.bufferUsage = BufferUsage::Vertex;
	//	desc.memoryUsage = MemoryUsage::Dynamic;
	//	desc.size = _size;
	//	desc.initialData = _initialData;

	//	Shared<VulkanGPUBuffer> gpuBuffer = MakeShared<VulkanGPUBuffer>(this, desc);

	//	Shared<VertexBuffer> vertexBuffer = MakeShared<VertexBuffer>(gpuBuffer, _stride);
	//	if (_initialData)
	//	{
	//		vertexBuffer->SetData(_initialData, _initialDataSize);
	//	}
	//	return vertexBuffer;
	//}

	//Shared<VertexBuffer> VulkanRenderDevice::CreateStaticVertexBuffer(UInt32 _size, UInt32 _stride, const void* _initialData)
	//{
	//	auto vertexBuffer = MakeShared<VulkanVertexBuffer>(this, MemoryUsage::Static, _size);

	//	auto [uploadBuffer, uploadBufferAllocation] = CreateBuffer(
	//		_size,
	//		vk::BufferUsageFlagBits::eTransferSrc,
	//		vma::MemoryUsage::eCpuOnly,
	//		vma::AllocationCreateFlagBits::eMapped);

	//	vma::AllocationInfo allocationInfo;
	//	allocator->getAllocationInfo(uploadBufferAllocation.get(), &allocationInfo);
	//	memcpy(allocationInfo.pMappedData, _initialData, _size);
	//	CopyBuffer(uploadBuffer.get(), vertexBuffer->GetVkBuffer(), _size);

	//	return vertexBuffer;
	//}

	//Shared<IndexBuffer> VulkanRenderDevice::CreateIndexBuffer(const UInt32* _indices, UInt32 _count)
	//{
	//	auto indexBuffer = MakeShared<VulkanIndexBuffer>(this, _count);

	//	UInt32 bufferSize = sizeof(UInt32) * _count;
	//	auto [uploadBuffer, uploadBufferAllocation] = CreateBuffer(
	//		bufferSize,
	//		vk::BufferUsageFlagBits::eTransferSrc,
	//		vma::MemoryUsage::eCpuOnly,
	//		vma::AllocationCreateFlagBits::eMapped);

	//	vma::AllocationInfo allocationInfo;
	//	allocator->getAllocationInfo(uploadBufferAllocation.get(), &allocationInfo);
	//	memcpy(allocationInfo.pMappedData, _indices, bufferSize);

	//	CopyBuffer(uploadBuffer.get(), indexBuffer->GetVkBuffer(), bufferSize); // CopyBuffer는 vkCmdCopyBuffer를 호출하는 헬퍼 함수

	//	return indexBuffer;
	//}

	//Shared<ConstantBuffer> VulkanRenderDevice::CreateConstantBuffer(UInt32 _size)
	//{
	//	return MakeShared<VulkanConstantBuffer>(this, _size);
	//}


	//Shared<RenderPass> VulkanRenderDevice::CreateRenderPass(const RenderPassDesc& _desc)
	//{
	//	return MakeShared<VulkanRenderPass>(this, _desc);
	//}

	//Shared<Framebuffer> VulkanRenderDevice::CreateFramebuffer(Shared<RenderPass> _renderPass, const FramebufferDesc& _desc)
	//{
	//	return _renderPass->CreateFramebuffer(_desc);
	//}

	Shared<GraphicsPipelineState> VulkanRenderDevice::CreatePipelineState(const GraphicsPipelineStateDesc& _desc)
	{
		return MakeShared<VulkanGraphicsPipelineState>(this, _desc);
	}

	Shared<Shader> VulkanRenderDevice::CreateShader(const std::string& _src, const ShaderType& _type, ShaderLoadMode _mode)
	{
		return MakeShared<VulkanShader>(this, _src, _type, _mode);
	}

	Shared<Swapchain> VulkanRenderDevice::CreateSwapchain(const DaydreamWindow& _window, const SwapchainDesc& _desc)
	{
		return MakeShared<VulkanSwapchain>(this, _window, _desc);
	}

	//Shared<Texture2D> VulkanRenderDevice::CreateTexture2D(const void* _imageData, const TextureDesc& _desc)
	//{
	//	auto texture = MakeShared<VulkanTexture2D>(this, _desc);

	//	if (_imageData != nullptr)
	//	{
	//		UInt32 imageSize = _desc.width * _desc.height * GraphicsUtility::GetRenderFormatSize(_desc.format);
	//		auto [uploadBuffer, uploadBufferAllocation] = CreateBuffer(
	//			imageSize,
	//			vk::BufferUsageFlagBits::eTransferSrc,
	//			vma::MemoryUsage::eCpuOnly,
	//			vma::AllocationCreateFlagBits::eMapped);

	//		vma::AllocationInfo allocationInfo;
	//		allocator->getAllocationInfo(uploadBufferAllocation.get(), &allocationInfo);
	//		memcpy(allocationInfo.pMappedData, _imageData, imageSize);

	//		vk::ImageMemoryBarrier barrier{};
	//		barrier.oldLayout = vk::ImageLayout::eUndefined;
	//		barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
	//		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//		barrier.image = texture->GetVkImage();
	//		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	//		barrier.subresourceRange.baseArrayLayer = 0;
	//		barrier.subresourceRange.baseMipLevel = 0;
	//		barrier.subresourceRange.layerCount = 1;
	//		barrier.subresourceRange.levelCount = 1;

	//		TransitionImageLayoutImmediate(barrier);

	//		CopyBufferToImage(uploadBuffer.get(), texture->GetVkImage(), _desc.width, _desc.height);

	//		//vk::ImageMemoryBarrier barrier{};
	//		barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
	//		barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	//		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//		barrier.image = texture->GetVkImage();
	//		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	//		barrier.subresourceRange.baseArrayLayer = 0;
	//		barrier.subresourceRange.baseMipLevel = 0;
	//		barrier.subresourceRange.layerCount = 1;
	//		barrier.subresourceRange.levelCount = 1;

	//		TransitionImageLayoutImmediate(barrier);
	//	}
	//	return texture;
	//}

	//Shared<Texture2D> VulkanRenderDevice::CreateEmptyTexture2D(const TextureDesc& _desc)
	//{
	//	auto texture = MakeShared<VulkanTexture2D>(this, _desc);

	//	vk::ImageMemoryBarrier barrier{};
	//	barrier.oldLayout = vk::ImageLayout::eUndefined;
	//	barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	//	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barrier.image = texture->GetVkImage();
	//	barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	//	barrier.subresourceRange.baseArrayLayer = 0;
	//	barrier.subresourceRange.baseMipLevel = 0;
	//	barrier.subresourceRange.layerCount = 1;
	//	barrier.subresourceRange.levelCount = 1;

	//	TransitionImageLayoutImmediate(barrier);

	//	return texture;
	//}

	//Shared<TextureCube> VulkanRenderDevice::CreateTextureCube(Array<const void*>& _imagePixels, const TextureDesc& _desc)
	//{
	//	Shared<VulkanTextureCube> textureCube = MakeShared<VulkanTextureCube>(this, _desc);

	//	UInt32 imageSize = _desc.width * _desc.height * GraphicsUtility::GetRenderFormatSize(_desc.format);
	//	UInt32 bufferSize = imageSize * 6;
	//	vk::BufferCreateInfo bufferInfo;
	//	bufferInfo.size = bufferSize;
	//	bufferInfo.usage = vk::BufferUsageFlagBits::eTransferSrc; // 복사 소스

	//	vma::AllocationCreateInfo allocInfo = {};
	//	allocInfo.usage = vma::MemoryUsage::eAuto;
	//	allocInfo.flags = vma::AllocationCreateFlagBits::eHostAccessSequentialWrite |
	//		vma::AllocationCreateFlagBits::eMapped;
	//	auto [uploadBuffer, uploadBufferAllocation] = CreateBuffer(
	//		bufferInfo, allocInfo);

	//	char* mappedData = static_cast<char*>(allocator->getAllocationInfo(uploadBufferAllocation.get()).pMappedData);
	//	for (int i = 0; i < 6; ++i) {
	//		memcpy(mappedData + (imageSize * i), _imagePixels[i], imageSize);
	//	}

	//	Array<vk::BufferImageCopy> bufferCopyRegions;
	//	for (UInt32 i = 0; i < 6; i++)
	//	{
	//		VkBufferImageCopy region = {};
	//		region.bufferOffset = imageSize * i;
	//		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	//		region.imageSubresource.mipLevel = 0;
	//		region.imageSubresource.baseArrayLayer = i;
	//		region.imageSubresource.layerCount = 1;
	//		region.imageExtent = { _desc.width, _desc.height, 1 };
	//		bufferCopyRegions.push_back(region);
	//	}

	//	CopyBufferToImage(uploadBuffer.get(), textureCube->GetVkImage(), bufferCopyRegions);

	//	vk::ImageMemoryBarrier barrier{};
	//	barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
	//	barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	//	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barrier.image = textureCube->GetVkImage();
	//	barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	//	barrier.subresourceRange.baseArrayLayer = 0;
	//	barrier.subresourceRange.baseMipLevel = 0;
	//	barrier.subresourceRange.layerCount = 6;
	//	barrier.subresourceRange.levelCount = 1;

	//	TransitionImageLayoutImmediate(barrier);

	//	return textureCube;
	//}

	//Shared<TextureCube> VulkanRenderDevice::CreateEmptyTextureCube(const TextureDesc& _desc)
	//{
	//	auto textureCube = MakeShared<VulkanTextureCube>(this, _desc);

	//	vk::ImageMemoryBarrier barrier{};
	//	barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
	//	barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	//	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barrier.image = textureCube->GetVkImage();
	//	barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	//	barrier.subresourceRange.baseArrayLayer = 0;
	//	barrier.subresourceRange.baseMipLevel = 0;
	//	barrier.subresourceRange.layerCount = 6;
	//	barrier.subresourceRange.levelCount = 1;

	//	TransitionImageLayoutImmediate(barrier);

	//	return textureCube;
	//}

	Shared<Sampler> VulkanRenderDevice::CreateSampler(const SamplerDesc& _desc)
	{
		return MakeShared<VulkanSampler>(this, _desc);
	}

	Unique<ImGuiRenderer> VulkanRenderDevice::CreateImGuiRenderer()
	{
		return MakeUnique<VulkanImGuiRenderer>(this);
	}

	//void VulkanRenderDevice::CopyTexture2D(Shared<Texture2D> _src, Shared<Texture2D> _dst)
	//{
	//	vk::ImageMemoryBarrier barriers[2] = {};

	//	VulkanTexture2D* src = (VulkanTexture2D*)_src.get();
	//	VulkanTexture2D* dst = (VulkanTexture2D*)_dst.get();

	//	// 원본 이미지를 TRANSFER_SRC로 변경
	//	barriers[0].oldLayout = vk::ImageLayout::eShaderReadOnlyOptimal; // 또는 현재 레이아웃
	//	barriers[0].newLayout = vk::ImageLayout::eTransferSrcOptimal;
	//	barriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barriers[0].image = src->GetVkImage();
	//	barriers[0].subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	//	barriers[0].subresourceRange.baseMipLevel = 0;
	//	barriers[0].subresourceRange.levelCount = 1;
	//	barriers[0].subresourceRange.baseArrayLayer = 0;
	//	barriers[0].subresourceRange.layerCount = 1;
	//	barriers[0].srcAccessMask = {}; // 이전 작업이 없다고 가정
	//	barriers[0].dstAccessMask = vk::AccessFlagBits::eTransferRead;

	//	// 대상 이미지를 TRANSFER_DST로 변경
	//	barriers[1].oldLayout = vk::ImageLayout::eUndefined; // 또는 현재 레이아웃
	//	barriers[1].newLayout = vk::ImageLayout::eTransferDstOptimal;
	//	barriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barriers[1].image = dst->GetVkImage();
	//	barriers[1].subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	//	barriers[1].subresourceRange.baseMipLevel = 0;
	//	barriers[1].subresourceRange.levelCount = 1;
	//	barriers[1].subresourceRange.baseArrayLayer = 0;
	//	barriers[1].subresourceRange.layerCount = 1;
	//	barriers[1].srcAccessMask = {};
	//	barriers[1].dstAccessMask = vk::AccessFlagBits::eTransferWrite;

	//	currentCommandBuffer.pipelineBarrier(
	//		vk::PipelineStageFlagBits::eTopOfPipe,  // 이전 작업 단계
	//		vk::PipelineStageFlagBits::eTransfer,     // 다음 작업 단계 (전송)
	//		{},
	//		0, nullptr,
	//		0, nullptr,
	//		2, barriers
	//	);

	//	// 2. 복사 명령 기록
	//	vk::ImageCopy copyRegion = {};
	//	copyRegion.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	//	copyRegion.srcSubresource.layerCount = 1;
	//	copyRegion.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	//	copyRegion.dstSubresource.layerCount = 1;
	//	copyRegion.extent.width = src->GetWidth();
	//	copyRegion.extent.height = src->GetHeight();
	//	copyRegion.extent.depth = 1;

	//	currentCommandBuffer.copyImage(
	//		src->GetVkImage(), vk::ImageLayout::eTransferSrcOptimal,
	//		dst->GetVkImage(), vk::ImageLayout::eTransferDstOptimal,
	//		1, &copyRegion
	//	);


	//	// 원본 이미지를 TRANSFER_SRC로 변경
	//	barriers[0].oldLayout = vk::ImageLayout::eTransferSrcOptimal; // 또는 현재 레이아웃
	//	barriers[0].newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	//	barriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barriers[0].image = src->GetVkImage();
	//	barriers[0].subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	//	barriers[0].subresourceRange.baseMipLevel = 0;
	//	barriers[0].subresourceRange.levelCount = 1;
	//	barriers[0].subresourceRange.baseArrayLayer = 0;
	//	barriers[0].subresourceRange.layerCount = 1;
	//	barriers[0].srcAccessMask = vk::AccessFlagBits::eTransferRead; // 이전 작업이 없다고 가정
	//	barriers[0].dstAccessMask = {};

	//	// 대상 이미지를 TRANSFER_DST로 변경
	//	barriers[1].oldLayout = vk::ImageLayout::eTransferDstOptimal; // 또는 현재 레이아웃
	//	barriers[1].newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	//	barriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	//	barriers[1].image = dst->GetVkImage();
	//	barriers[1].subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	//	barriers[1].subresourceRange.baseMipLevel = 0;
	//	barriers[1].subresourceRange.levelCount = 1;
	//	barriers[1].subresourceRange.baseArrayLayer = 0;
	//	barriers[1].subresourceRange.layerCount = 1;
	//	barriers[1].srcAccessMask = vk::AccessFlagBits::eTransferWrite;
	//	barriers[1].dstAccessMask = {};

	//	currentCommandBuffer.pipelineBarrier(
	//		vk::PipelineStageFlagBits::eTransfer,  // 이전 작업 단계
	//		vk::PipelineStageFlagBits::eFragmentShader,     // 다음 작업 단계 (전송)
	//		{},
	//		0, nullptr,
	//		0, nullptr,
	//		2, barriers
	//	);
	//}

	vk::CommandBuffer VulkanRenderDevice::BeginSingleTimeCommands()
	{
		vk::CommandBufferAllocateInfo allocInfo{};
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandPool = commandPool.get();
		allocInfo.commandBufferCount = 1;

		std::vector<vk::CommandBuffer> commandBuffers(1);
		commandBuffers = device->allocateCommandBuffers(allocInfo);

		vk::CommandBufferBeginInfo beginInfo{};
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		commandBuffers[0].begin(beginInfo);

		return commandBuffers[0];
	}

	void VulkanRenderDevice::EndSingleTimeCommands(vk::CommandBuffer _commandBuffer)
	{
		_commandBuffer.end();

		vk::SubmitInfo submitInfo{};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &_commandBuffer;

		vk::Result result = graphicsQueue.submit(1, &submitInfo, {});
		vkQueueWaitIdle(graphicsQueue);

		device->freeCommandBuffers(commandPool.get(), 1, &_commandBuffer);
	}

	SwapchainSupportDetails VulkanRenderDevice::QuerySwapchainSupport(vk::SurfaceKHR _surface)
	{
		SwapchainSupportDetails details;
		details.capabilities = physicalDevice.getSurfaceCapabilitiesKHR(_surface);
		details.formats = physicalDevice.getSurfaceFormatsKHR(_surface);
		details.presentModes = physicalDevice.getSurfacePresentModesKHR(_surface);

		return details;
	}

	UInt32 VulkanRenderDevice::FindMemoryType(UInt32 _typeFilter, vk::MemoryPropertyFlags _properties)
	{
		vk::PhysicalDeviceMemoryProperties memProperties;
		memProperties = physicalDevice.getMemoryProperties();

		for (UInt32 i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((_typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & _properties) == _properties)
			{
				return i;
			}
		}
		return 0;
	}

	Pair<vma::UniqueBuffer, vma::UniqueAllocation> VulkanRenderDevice::CreateBuffer(vk::DeviceSize _size, vk::BufferUsageFlags _usage,
		vma::MemoryUsage _memoryUsage, vma::AllocationCreateFlags _flags)
	{
		vk::BufferCreateInfo bufferInfo{};
		bufferInfo.size = _size;
		bufferInfo.usage = _usage;
		bufferInfo.sharingMode = vk::SharingMode::eExclusive;

		vma::AllocationCreateInfo allocInfo{};
		allocInfo.usage = _memoryUsage;
		allocInfo.flags = _flags;

		//vk::MemoryRequirements memRequirements = device->getBufferMemoryRequirements(buffer);

		//vk::MemoryAllocateInfo allocInfo{};
		//allocInfo.allocationSize = memRequirements.size;
		//allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, _properties);

		//vk::DeviceMemory bufferMemory = device->allocateMemory(allocInfo);
		//device->bindBufferMemory(buffer, bufferMemory, 0);

		return allocator->createBufferUnique(bufferInfo, allocInfo);
	}

	Pair<vma::UniqueBuffer, vma::UniqueAllocation> VulkanRenderDevice::CreateBuffer(vk::BufferCreateInfo _bufferInfo, vma::AllocationCreateInfo _allocInfo)
	{
		return allocator->createBufferUnique(_bufferInfo, _allocInfo);
	}

	Pair<vma::UniqueImage, vma::UniqueAllocation> VulkanRenderDevice::CreateImage(vk::ImageCreateInfo _imageInfo, vma::AllocationCreateInfo _allocInfo)
	{
		return allocator->createImageUnique(_imageInfo, _allocInfo);
	}

	vk::UniqueImageView VulkanRenderDevice::CreateImageView(vk::Image _image, vk::Format _format, vk::ImageAspectFlags _aspectMask)
	{
		vk::ImageViewCreateInfo viewInfo{};
		viewInfo.image = _image;
		viewInfo.viewType = vk::ImageViewType::e2D;
		viewInfo.format = _format;
		viewInfo.subresourceRange.aspectMask = _aspectMask;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		return device->createImageViewUnique(viewInfo);
	}

	vk::UniqueImageView VulkanRenderDevice::CreateImageView(vk::ImageViewCreateInfo _viewCreateInfo)
	{
		return device->createImageViewUnique(_viewCreateInfo);
	}

	void VulkanRenderDevice::CopyBuffer(vk::Buffer _src, vk::Buffer _dst, vk::DeviceSize _size)
	{
		vk::CommandBuffer copyCommandBuffer = BeginSingleTimeCommands();

		vk::BufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = _size;

		copyCommandBuffer.copyBuffer(_src, _dst, 1, &copyRegion);

		EndSingleTimeCommands(copyCommandBuffer);
	}

	void VulkanRenderDevice::CopyBufferToImage(vk::Buffer _src, vk::Image _dst, UInt32 _width, UInt32 _height)
	{
		vk::CommandBuffer copyCommandBuffer = BeginSingleTimeCommands();

		vk::BufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = vk::Offset3D();
		region.imageExtent = vk::Extent3D(
			_width,
			_height,
			1
		);

		copyCommandBuffer.copyBufferToImage(_src, _dst, vk::ImageLayout::eTransferDstOptimal, 1, &region);

		EndSingleTimeCommands(copyCommandBuffer);
	}

	void VulkanRenderDevice::CopyBufferToImage(vk::Buffer _src, vk::Image _dst, Array<vk::BufferImageCopy> _imageCopyRegion)
	{
		vk::CommandBuffer copyCommandBuffer = BeginSingleTimeCommands();

		copyCommandBuffer.copyBufferToImage(_src, _dst, vk::ImageLayout::eTransferDstOptimal, _imageCopyRegion);

		EndSingleTimeCommands(copyCommandBuffer);

	}

	void VulkanRenderDevice::TransitionImageLayoutImmediate(vk::ImageMemoryBarrier _barrier)
	{
		vk::CommandBuffer transCommandBuffer = BeginSingleTimeCommands();

		vk::AccessFlags srcAccess;
		vk::AccessFlags dstAccess;

		vk::PipelineStageFlags srcStage{};
		vk::PipelineStageFlags dstStage{};

		switch (_barrier.oldLayout)
		{
		case vk::ImageLayout::eUndefined:
			srcAccess = {};
			srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
			break;
		case vk::ImageLayout::eTransferSrcOptimal:
			srcAccess = vk::AccessFlagBits::eTransferRead;
			srcStage = vk::PipelineStageFlagBits::eTransfer;
			break;
		case vk::ImageLayout::eTransferDstOptimal:
			srcAccess = vk::AccessFlagBits::eTransferWrite;
			srcStage = vk::PipelineStageFlagBits::eTransfer;
			break;
		case vk::ImageLayout::eColorAttachmentOptimal:
			srcAccess = vk::AccessFlagBits::eColorAttachmentWrite;
			srcStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			break;
		case vk::ImageLayout::eShaderReadOnlyOptimal:
			srcAccess = vk::AccessFlagBits::eShaderRead;
			srcStage = vk::PipelineStageFlagBits::eFragmentShader;
			break;
		default:
			throw std::invalid_argument("Unsupported old layout!");
		}

		switch (_barrier.newLayout)
		{
		case vk::ImageLayout::eTransferSrcOptimal:
			dstAccess = vk::AccessFlagBits::eTransferRead;
			dstStage = vk::PipelineStageFlagBits::eTransfer;
			break;
		case vk::ImageLayout::eTransferDstOptimal:
			dstAccess = vk::AccessFlagBits::eTransferWrite;
			dstStage = vk::PipelineStageFlagBits::eTransfer;
			break;
		case vk::ImageLayout::eShaderReadOnlyOptimal:
			dstAccess = vk::AccessFlagBits::eShaderRead;
			dstStage = vk::PipelineStageFlagBits::eFragmentShader;
			break;
		case vk::ImageLayout::eColorAttachmentOptimal:
			dstAccess = vk::AccessFlagBits::eColorAttachmentWrite;
			dstStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
			break;
		case vk::ImageLayout::eDepthStencilAttachmentOptimal:
			dstAccess = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
			dstStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
			break;
		case vk::ImageLayout::ePresentSrcKHR:
			dstAccess = {}; // 또는 VK_ACCESS_NONE (Vulkan 1.3 이상)
			dstStage = vk::PipelineStageFlagBits::eBottomOfPipe; // 또는 VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT
			break;
		default:
			throw std::invalid_argument("Unsupported new layout!");
		}
		_barrier.srcAccessMask = srcAccess;
		_barrier.dstAccessMask = dstAccess;

		transCommandBuffer.pipelineBarrier(
			srcStage, dstStage,
			{},
			0, nullptr,
			0, nullptr,
			1, &_barrier
		);

		EndSingleTimeCommands(transCommandBuffer);
	}


	void VulkanRenderDevice::CreateInstance()
	{
		UInt32 res = putenv("VK_LOADER_LAYERS_DISABLE=VK_LAYER_OBS_HOOK");
		vk::detail::defaultDispatchLoaderDynamic.init();
		if (enableValidationLayers == true && CheckValidationLayerSupport() == false)
		{
			DAYDREAM_CORE_ERROR("Validation layers requested, but not available!");
			throw std::runtime_error("Validation layers not supported!");
		}

		vk::ApplicationInfo appInfo(
			"My Vulkan App",
			VK_VERSION_1_4,
			"DaydreamEngine",
			VK_VERSION_1_4,
			VK_API_VERSION_1_4
		);

		Array<const char*> extensions = GetRequiredExtensions();

		vk::ValidationFeatureEnableEXT enables[] = {
			vk::ValidationFeatureEnableEXT::eGpuAssisted,
			vk::ValidationFeatureEnableEXT::eSynchronizationValidation,
			vk::ValidationFeatureEnableEXT::eBestPractices
		};

		vk::ValidationFeaturesEXT features = {};
		features.enabledValidationFeatureCount = 3;
		features.pEnabledValidationFeatures = enables;


		vk::InstanceCreateInfo createInfo{};
		createInfo.flags = {};
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = {};
		createInfo.enabledExtensionCount = (UInt32)extensions.size();
		createInfo.ppEnabledExtensionNames = extensions.data();


		vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<UInt32>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
			PopulateDebugMessengerCreateInfo(debugCreateInfo);
			debugCreateInfo.pNext = nullptr;
			createInfo.pNext = &debugCreateInfo;
		}

		instance = vk::createInstanceUnique(createInfo);
		vk::detail::defaultDispatchLoaderDynamic.init(*instance);
	}
	void VulkanRenderDevice::SetupDebugMessenger()
	{
		if (!enableValidationLayers) return;

		vk::DebugUtilsMessengerCreateInfoEXT createInfo;
		PopulateDebugMessengerCreateInfo(createInfo);
		debugMessenger = instance->createDebugUtilsMessengerEXTUnique(createInfo);
	}
	void VulkanRenderDevice::PickPhysicalDevice()
	{
		Array<vk::PhysicalDevice> physicalDevices = instance->enumeratePhysicalDevices();
		if (physicalDevices.empty())
		{
			DAYDREAM_CORE_ERROR("Failed to find GPUs with Vulkan support!");
		}

		for (const vk::PhysicalDevice& device : physicalDevices)
		{
			if (IsDeviceSuitable(device))
			{
				physicalDevice = device;
				break;
			}
		}

		if (physicalDevice == VK_NULL_HANDLE)
		{
			DAYDREAM_CORE_ERROR("Failed to find a suitable GPU!");
		}
	}
	void VulkanRenderDevice::CreateLogicalDevice()
	{
		Float32 queuePriority = 1.0f;
		queueFamilyIndices = FindQueueFamilies(physicalDevice);
		vk::DeviceQueueCreateInfo queueCreateInfo(
			{},
			queueFamilyIndices.graphicsFamily.value(),
			1,
			&queuePriority
		);

		vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};
		dynamicRenderingFeatures.dynamicRendering = VK_TRUE;

		vk::PhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures;
		descriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
		descriptorIndexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
		descriptorIndexingFeatures.descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE;
		descriptorIndexingFeatures.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
		descriptorIndexingFeatures.pNext = &dynamicRenderingFeatures;

		vk::PhysicalDeviceFeatures2 deviceFeatures{};
		deviceFeatures.features.samplerAnisotropy = VK_TRUE;
		deviceFeatures.pNext = &descriptorIndexingFeatures;

		vk::DeviceCreateInfo createInfo{};
		createInfo.queueCreateInfoCount = 1;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.enabledLayerCount = Cast<UInt32>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.empty() ? nullptr : validationLayers.data();
		createInfo.enabledExtensionCount = Cast<UInt32>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.empty() ? nullptr : deviceExtensions.data();
		createInfo.pEnabledFeatures = nullptr;
		createInfo.pNext = &deviceFeatures;

		device = physicalDevice.createDeviceUnique(createInfo);
		graphicsQueue = device->getQueue(queueFamilyIndices.graphicsFamily.value(), 0);
	}

	bool VulkanRenderDevice::CheckValidationLayerSupport()
	{
		Array<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}
	Array<const char*> VulkanRenderDevice::GetRequiredExtensions()
	{
		UInt32 glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		Array<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}
	void VulkanRenderDevice::PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& _createInfo)
	{
		using enum vk::DebugUtilsMessageSeverityFlagBitsEXT;
		using enum vk::DebugUtilsMessageTypeFlagBitsEXT;

		_createInfo.messageSeverity = eVerbose | eWarning | eError;
		_createInfo.messageType = eGeneral | eValidation | ePerformance;
		_createInfo.pfnUserCallback = DebugCallback;
		_createInfo.pUserData = nullptr; // 원한다면 this 포인터 등을 전달할 수 있음
	}
	bool VulkanRenderDevice::IsDeviceSuitable(vk::PhysicalDevice _physicalDevice)
	{
		vk::PhysicalDeviceProperties deviceProperties;
		vk::PhysicalDeviceFeatures deviceFeatures;
		vk::PhysicalDeviceProperties properties = _physicalDevice.getProperties();
		auto feautures = _physicalDevice.getFeatures();

		//이 GPU가 swapchain을 지원할 수 있는지 확인?
		bool extensionSupported = CheckDeviceExtensionSupport(_physicalDevice);

		QueueFamilyIndices indices = FindQueueFamilies(_physicalDevice);
		//return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		//	deviceFeatures.geometryShader;
		vk::PhysicalDeviceFeatures supportedFeatures = _physicalDevice.getFeatures();

		return indices.IsComplete() && extensionSupported && supportedFeatures.samplerAnisotropy;
	}
	QueueFamilyIndices VulkanRenderDevice::FindQueueFamilies(vk::PhysicalDevice _physicalDevice)
	{
		QueueFamilyIndices indices;

		UInt32 queueFamilyCount = 0;
		Array<vk::QueueFamilyProperties> queueFamilies = _physicalDevice.getQueueFamilyProperties();

		int i = 0;
		for (UInt32 i = 0; i < (UInt32)queueFamilies.size(); i++)
		{
			if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
			{
				indices.graphicsFamily = i;
			}
			if (indices.IsComplete())
			{
				break;
			}
		}

		return indices;
	}

	bool VulkanRenderDevice::CheckDeviceExtensionSupport(vk::PhysicalDevice _physicalDevice)
	{
		UInt32 extensionCount = 0;
		Array<vk::ExtensionProperties> availableExtensions = _physicalDevice.enumerateDeviceExtensionProperties();

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const vk::ExtensionProperties& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}



}
