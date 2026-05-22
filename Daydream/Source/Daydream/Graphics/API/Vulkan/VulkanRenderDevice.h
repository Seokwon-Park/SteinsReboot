#pragma once

#include "vulkan/vulkan.h"

//#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#include "vulkan/vulkan.hpp"

#include "Daydream/Graphics/Core/RenderDevice.h"

#include "vk_mem_alloc.hpp"

namespace Daydream
{
	struct QueueFamilyIndices
	{
		std::optional<UInt32> graphicsFamily;
		//std::optional<UInt32> presentFamily; // 지금은 필요없는듯.

		bool IsComplete()
		{
			return graphicsFamily.has_value();
		}
	};

	struct SwapchainSupportDetails
	{
		//gpu와 surface 조합에서 지원되는 최소/최대 이미지 크기정보
		vk::SurfaceCapabilitiesKHR capabilities;
		//색상 format
		Array<vk::SurfaceFormatKHR> formats;
		//present 방식
		Array<vk::PresentModeKHR> presentModes;
	};

	class VulkanRenderDevice :public RenderDevice
	{
	public:
		VulkanRenderDevice();
		virtual ~VulkanRenderDevice() override;

		virtual void Init() override;
		virtual void Shutdown() override;

		virtual Unique<RenderContext> CreateContext() override;
		virtual Shared<RenderCommandList> CreateRenderCommandList() override;
		virtual Shared<GPUBuffer> CreateGPUBuffer(const BufferDesc& _desc) override;
		virtual Shared<GPUTexture> CreateGPUTexture(const TextureDesc & _desc) override;
		virtual Shared<TextureView> CreateTextureView(Texture* _texture, const TextureViewDesc& _desc) override;

		//virtual Shared<VertexBuffer> CreateDynamicVertexBuffer(UInt32 _size, UInt32 _stride, UInt32 _initialDataSize = 0, const void* _initialData = nullptr) override;
		//virtual Shared<VertexBuffer> CreateStaticVertexBuffer(UInt32 _size, UInt32 _stride, const void * _initialData) override;
		//virtual Shared<IndexBuffer> CreateIndexBuffer(const UInt32 * _indices, UInt32 _count) override;
		//virtual Shared<ConstantBuffer> CreateConstantBuffer(UInt32 _size) override;
		//virtual Shared<RenderPass> CreateRenderPass(const RenderPassDesc& _desc) override;
		//virtual Shared<Framebuffer> CreateFramebuffer(Shared<RenderPass> _renderPass, const FramebufferDesc& _desc) override;
		virtual Shared<GraphicsPipelineState> CreatePipelineState(const GraphicsPipelineStateDesc& _desc)override;
		virtual Shared<Shader> CreateShader(const std::string& _src, const ShaderType& _type, ShaderLoadMode _mode) override;
		virtual Shared<Swapchain> CreateSwapchain(const DaydreamWindow& _window, const SwapchainDesc& _desc)override;
		//virtual Shared<Texture2D> CreateTexture2D(const void* _imageData, const TextureDesc& _desc)override;
		//virtual Shared<Texture2D> CreateEmptyTexture2D(const TextureDesc& _desc)override;
		//virtual Shared<TextureCube> CreateTextureCube(Array<const void*>& _imagePixels, const TextureDesc& _desc)override;
		//virtual Shared<TextureCube> CreateEmptyTextureCube(const TextureDesc& _desc) override;
		virtual Shared<Sampler> CreateSampler(const SamplerDesc& _desc)override;
		virtual Unique<ImGuiRenderer> CreateImGuiRenderer() override;

		vk::Instance GetInstance() const { return instance.get(); }
		vk::PhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
		vk::Device GetDevice() const { return device.get(); }
		vma::Allocator GetAllocator() const { return allocator.get(); }
		UInt32 GetGraphicsFamilyIndex() const { return queueFamilyIndices.graphicsFamily.value(); }
		vk::Queue GetGraphicsQueue() const { return graphicsQueue; }
		vk::DescriptorPool GetDescriptorPool() const { return descriptorPool.get(); }
		vk::CommandBuffer GetCommandBuffer() const { return currentCommandBuffer; }
		void SetCommandBuffer(const vk::CommandBuffer& _commandBuffer) { currentCommandBuffer = _commandBuffer; }
		vk::CommandPool GetCommandPool() const { return commandPool.get(); }
		void SetCurrentRenderPass(vk::RenderPass _renderPass) { currentRenderPass = _renderPass; }
		vk::RenderPass GetCurrentRenderPass() const { return currentRenderPass; }

		vk::CommandBuffer BeginSingleTimeCommands();
		void EndSingleTimeCommands(vk::CommandBuffer _commandBuffer);

		SwapchainSupportDetails QuerySwapchainSupport(vk::SurfaceKHR _surface);
		UInt32 FindMemoryType(UInt32 _typeFilter, vk::MemoryPropertyFlags _properties);
		//Create a VkBuffer
		Pair<vma::UniqueBuffer, vma::UniqueAllocation> CreateBuffer(vk::DeviceSize _size, vk::BufferUsageFlags _usage,
			vma::MemoryUsage _memoryUsage, vma::AllocationCreateFlags _flags);
		Pair<vma::UniqueBuffer, vma::UniqueAllocation> CreateBuffer(vk::BufferCreateInfo _bufferInfo, vma::AllocationCreateInfo _allocInfo);
		Pair<vma::UniqueImage, vma::UniqueAllocation> CreateImage(vk::ImageCreateInfo _imageInfo, vma::AllocationCreateInfo _allocInfo);

		vk::UniqueImageView CreateImageView(vk::Image _image, vk::Format _format, vk::ImageAspectFlags _aspectMask);
		vk::UniqueImageView CreateImageView(vk::ImageViewCreateInfo _viewCreateInfo);
		void CopyBuffer(vk::Buffer _src, vk::Buffer _dst, vk::DeviceSize _size);
		void CopyBufferToImage(vk::Buffer _src, vk::Image _dst, UInt32 _width, UInt32 _height);
		void CopyBufferToImage(vk::Buffer _src, vk::Image _dst, Array<vk::BufferImageCopy> _imageCopyRegion);
		void TransitionImageLayoutImmediate(vk::ImageMemoryBarrier _barrier);

		//SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface);
	private:
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateLogicalDevice();
		void PickPhysicalDevice();

		bool CheckValidationLayerSupport();
		Array<const char*> GetRequiredExtensions();
		void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& _createInfo);
		bool IsDeviceSuitable(vk::PhysicalDevice _physicalDevice);
		QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice _physicalDevice);
		bool CheckDeviceExtensionSupport(vk::PhysicalDevice _physicalDevice);

#if defined(DAYDREAM_DEBUG) || defined(_DEBUG)
		const bool enableValidationLayers = true;
		const Array<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
#else
		const bool enableValidationLayers = false;
		const Array<const char*> validationLayers = {};
#endif
		const Array<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_GOOGLE_HLSL_FUNCTIONALITY_1_EXTENSION_NAME,
			VK_GOOGLE_USER_TYPE_EXTENSION_NAME,
			VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
			VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,			
		};
		vk::UniqueInstance instance; // Vulkan library handle
		vk::UniqueDebugUtilsMessengerEXT debugMessenger;
		vk::PhysicalDevice physicalDevice = VK_NULL_HANDLE; // GPU chosen as the default device
		vk::UniqueDevice device; // Vulkan device for commands
		QueueFamilyIndices queueFamilyIndices;
		vk::Queue graphicsQueue; // vulkan graphics Queue
		vk::UniqueCommandPool commandPool;
		vk::CommandBuffer currentCommandBuffer;
		vk::UniqueDescriptorPool descriptorPool;

		vk::RenderPass currentRenderPass;

		vma::UniqueAllocator allocator;

		

	};
}

