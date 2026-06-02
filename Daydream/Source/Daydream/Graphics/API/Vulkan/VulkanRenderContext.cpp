#include "DaydreamPCH.h"
#include "VulkanRenderContext.h"

#include "VulkanGraphicsPipelineState.h"
#include "VulkanTexture.h"
#include "VulkanTextureView.h"
#include "VulkanBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanUtility.h"
#include "Daydream/Graphics/Resources/Mesh.h"


namespace Daydream
{
	VulkanRenderContext::VulkanRenderContext(VulkanRenderDevice* _device)
	{
		device = _device;

		//commandBuffers.resize(_framesInFlight);
		//vk::CommandBufferAllocateInfo allocInfo{};
		//allocInfo.commandPool = device->GetCommandPool();
		//allocInfo.level = vk::CommandBufferLevel::ePrimary;
		//allocInfo.commandBufferCount = (UInt32)commandBuffers.size();

		//commandBuffers = device->GetDevice().allocateCommandBuffersUnique(allocInfo);
		//commandBufferIndex = 0;

		//vk::FenceCreateInfo fenceInfo{};
		//fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;

		//waitFences.resize(_framesInFlight);
		//for (UInt32 i = 0; i < _framesInFlight; i++)
		//{
		//	waitFences[i] = device->GetDevice().createFenceUnique(fenceInfo);
		//}
	}
	VulkanRenderContext::~VulkanRenderContext()
	{
		device->GetGraphicsQueue().waitIdle();
	}
	void VulkanRenderContext::BeginCommandList()
	{
		////activeCommandBuffer = commandBuffers[commandBufferIndex].get();
		//vk::Result result = device->GetDevice().waitForFences(1, &waitFences[commandBufferIndex].get(), VK_FALSE, UINT64_MAX);
		//result = device->GetDevice().resetFences(1, &waitFences[commandBufferIndex].get());

		//GetActiveCommandBuffer().reset({});

		//vk::CommandBufferBeginInfo beginInfo{};
		//GetActiveCommandBuffer().begin(beginInfo);
	}

	void VulkanRenderContext::EndCommandList()
	{
		/*GetActiveCommandBuffer().end();

		vk::SubmitInfo submitInfo{};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[commandBufferIndex].get();

		vk::Result result = device->GetGraphicsQueue().submit(1, &submitInfo, waitFences[commandBufferIndex].get());

		commandBufferIndex = (commandBufferIndex + 1) % 3;

		device->GetGraphicsQueue().waitIdle();*/
	}

	void VulkanRenderContext::SetViewport(UInt32 _x, UInt32 _y, UInt32 _width, UInt32 _height)
	{
		vk::Viewport viewport{};
		//viewport.x = 0.0f;
		//viewport.y = (float)extent.height;
		//viewport.width = (float)extent.width;
		//viewport.height = -(float)extent.height;
		viewport.x = (Float32)_x;
		viewport.y = (Float32)_y;
		viewport.width = (Float32)_width;
		viewport.height = (Float32)_height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		GetActiveCommandBuffer().setViewport(0, 1, &viewport);

		vk::Rect2D scissor{};
		scissor.offset = vk::Offset2D(_x, _y);
		scissor.extent = vk::Extent2D(_width, _height);
		GetActiveCommandBuffer().setScissor(0, 1, &scissor);
	}

	void VulkanRenderContext::DrawIndexed(UInt32 _indexCount, UInt32 _startIndex, UInt32 _baseVertex)
	{
		GetActiveCommandBuffer().drawIndexed(_indexCount, 1, _startIndex, _baseVertex, 0);
	}

	void VulkanRenderContext::BeginRendering(const RenderingInfo& _renderingInfo)
	{
		Array<vk::RenderingAttachmentInfo> colorAttachmentInfos;
		for (auto renderingDesc : _renderingInfo.colorAttachments)
		{
			vk::RenderingAttachmentInfo attachmentInfo{};
			VulkanTextureView* textureView = Cast<VulkanTextureView*>(renderingDesc.view);
			attachmentInfo.imageView = textureView->GetVkImageView();
			attachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
			attachmentInfo.resolveMode = vk::ResolveModeFlagBits::eNone;
			attachmentInfo.resolveImageView = VK_NULL_HANDLE;
			attachmentInfo.resolveImageLayout = vk::ImageLayout::eUndefined;
			attachmentInfo.loadOp = GraphicsUtility::Vulkan::ConvertToLoadOp(renderingDesc.loadOp);
			attachmentInfo.storeOp = GraphicsUtility::Vulkan::ConvertToStoreOp(renderingDesc.storeOp);
			// Clear Value 매핑 (std::array로 변환)
			vk::ClearColorValue clearColor;
			clearColor.setFloat32({
				renderingDesc.clearValue.colorClearValue.r,
				renderingDesc.clearValue.colorClearValue.g,
				renderingDesc.clearValue.colorClearValue.b,
				renderingDesc.clearValue.colorClearValue.a
				});
			attachmentInfo.clearValue.color = clearColor;
			attachmentInfo.pNext = nullptr;
			colorAttachmentInfos.push_back(attachmentInfo);
		}

		// 2. Depth/Stencil Attachment 설정 (사용하는 경우)
		vk::RenderingAttachmentInfo depthAttachmentInfo{};
		bool useDepth = _renderingInfo.depthAttachment.view != nullptr;

		if (useDepth)
		{
			VulkanTextureView* depthView = Cast<VulkanTextureView*>(_renderingInfo.depthAttachment.view);
			depthAttachmentInfo.imageView = depthView->GetVkImageView();

			// 깊이 버퍼 렌더링용 레이아웃
			depthAttachmentInfo.imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

			depthAttachmentInfo.resolveMode = vk::ResolveModeFlagBits::eNone;
			depthAttachmentInfo.resolveImageView = VK_NULL_HANDLE;
			depthAttachmentInfo.resolveImageLayout = vk::ImageLayout::eUndefined;

			depthAttachmentInfo.loadOp = GraphicsUtility::Vulkan::ConvertToLoadOp(_renderingInfo.depthAttachment.loadOp);
			depthAttachmentInfo.storeOp = GraphicsUtility::Vulkan::ConvertToStoreOp(_renderingInfo.depthAttachment.storeOp);

			vk::ClearDepthStencilValue clearDepthStencil(
				_renderingInfo.depthAttachment.clearValue.depthClearValue,
				_renderingInfo.depthAttachment.clearValue.stencilClearValue
			);
			depthAttachmentInfo.clearValue.depthStencil = clearDepthStencil;
		}

		vk::RenderingInfo renderingInfo{};
		renderingInfo.flags = {};
		renderingInfo.renderArea.extent.width = _renderingInfo.renderArea.width;
		renderingInfo.renderArea.extent.height = _renderingInfo.renderArea.height;
		renderingInfo.layerCount = 1;
		renderingInfo.viewMask;
		renderingInfo.colorAttachmentCount = (UInt32)colorAttachmentInfos.size();
		renderingInfo.pColorAttachments = colorAttachmentInfos.data();
		if (useDepth)
		{
			renderingInfo.pDepthAttachment = &depthAttachmentInfo;
			renderingInfo.pStencilAttachment = &depthAttachmentInfo;
		}
		renderingInfo.pNext = nullptr;

		SetViewport(
			_renderingInfo.renderArea.x,
			_renderingInfo.renderArea.y,
			_renderingInfo.renderArea.width,
			_renderingInfo.renderArea.height
		);

		GetActiveCommandBuffer().beginRendering(renderingInfo);
	}
	void VulkanRenderContext::EndRendering(const RenderingInfo& _renderingInfo)
	{
		GetActiveCommandBuffer().endRendering();
	}
	//void VulkanRenderContext::BeginRenderPass(Shared<RenderPass> _renderPass, Shared<Framebuffer> _framebuffer)
	//{
	//	Shared<VulkanFramebuffer> currentFramebuffer = static_pointer_cast<VulkanFramebuffer>(_framebuffer);
	//	Shared<VulkanRenderPass> renderPass = static_pointer_cast<VulkanRenderPass>(_renderPass);

	//	vk::RenderPassBeginInfo renderPassInfo{};
	//	renderPassInfo.renderPass = renderPass->GetVkRenderPass();
	//	renderPassInfo.framebuffer = currentFramebuffer->GetFramebuffer();
	//	renderPassInfo.renderArea.offset = vk::Offset2D(0, 0);
	//	renderPassInfo.renderArea.extent = currentFramebuffer->GetExtent();

	//	Array<vk::ClearValue> colors{};
	//	for (UInt32 i = 0; i < _framebuffer->GetColorAttachmentSize(); i++)
	//	{
	//		vk::ClearValue vulkanClearColor;
	//		memcpy(vulkanClearColor.color.float32, _renderPass->GetClearColor().color, sizeof(Color));
	//		colors.push_back(vulkanClearColor);
	//	}

	//	if (currentFramebuffer->HasDepthAttachment())
	//	{
	//		vk::ClearValue vulkanClearDepthStencil;
	//		vulkanClearDepthStencil.depthStencil.depth = 1.0f; // 또는 0.0f
	//		vulkanClearDepthStencil.depthStencil.stencil = 0;   // 스텐실 값도 함께 초기화
	//		colors.push_back(vulkanClearDepthStencil);
	//	}

	//	renderPassInfo.clearValueCount = (UInt32)colors.size();
	//	renderPassInfo.pClearValues = colors.data();

	//	SetViewport(0, 0, currentFramebuffer->GetWidth(), currentFramebuffer->GetHeight());

	//	GetActiveCommandBuffer().beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
	//}
	//void VulkanRenderContext::EndRenderPass(Shared<RenderPass> _renderPass)
	//{
	//	GetActiveCommandBuffer().endRenderPass();
	//}
	void VulkanRenderContext::BindPipelineState(const GraphicsPipelineState* _pipelineState)
	{
		RenderContext::BindPipelineState(_pipelineState);
		currentGraphicsPipelineState = _pipelineState;
		const VulkanGraphicsPipelineState* pipelineState = Cast<const VulkanGraphicsPipelineState*>(_pipelineState);

		GetActiveCommandBuffer().bindPipeline(vk::PipelineBindPoint::eGraphics, pipelineState->GetPipeline());
	}

	void VulkanRenderContext::BindVertexBuffer(const GPUBuffer* _vertexBuffer, UInt32 _stride)
	{
		vk::DeviceSize offset = 0;
		const VulkanGPUBuffer* vertexBuffer = Cast<const VulkanGPUBuffer*>(_vertexBuffer);
		GetActiveCommandBuffer().bindVertexBuffers(0, { vertexBuffer->GetVkBuffer() }, { offset });
	}
	void Daydream::VulkanRenderContext::BindIndexBuffer(const GPUBuffer* _indexBuffer)
	{
		const VulkanGPUBuffer* indexBuffer = Cast<const VulkanGPUBuffer*>(_indexBuffer);
		GetActiveCommandBuffer().bindIndexBuffer(indexBuffer->GetVkBuffer(), 0, vk::IndexType::eUint32);
	}

	//void VulkanRenderContext::SetTexture2D(const String& _name, Shared<Texture2D> _texture)
	//{
	//	if (_texture == nullptr) return;
	//	RenderContext::SetTexture2D(_name, _texture);
	//	const ShaderReflectionData* resourceInfo = activePipelineState->GetBindingInfo(_name);
	//	if (resourceInfo == nullptr) return;

	//	Shared<VulkanTexture2D> vulkanTexture = Cast<VulkanTexture2D>(_texture);
	//	Shared<VulkanPipelineState> vulkanPSO = Cast<VulkanPipelineState>(activePipelineState);

	//	vk::DescriptorImageInfo imageInfo{};
	//	imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	//	imageInfo.imageView = vulkanTexture->GetImageView();
	//	imageInfo.sampler = vulkanTexture->GetSampler();

	//	vk::WriteDescriptorSet writeSet = {};
	//	//writeSet.dstSet = sets[resourceInfo.set].get();
	//	writeSet.dstBinding = resourceInfo->binding;  // 특정 binding만 업데이트
	//	writeSet.descriptorCount = 1;
	//	writeSet.descriptorType = vk::DescriptorType::eCombinedImageSampler;
	//	writeSet.pImageInfo = &imageInfo;

	//	GetActiveCommandBuffer().pushDescriptorSet(
	//		vk::PipelineBindPoint::eGraphics,
	//		vulkanPSO->GetPipelineLayout(),
	//		resourceInfo->set,
	//		1,
	//		&writeSet
	//	);
	//}
	//void VulkanRenderContext::SetTextureCube(const String& _name, Shared<TextureCube> _textureCube)
	//{
	//	if (_textureCube == nullptr) return;
	//	const ShaderReflectionData* resourceInfo = activePipelineState->GetBindingInfo(_name);
	//	if (resourceInfo == nullptr) return;

	//	Shared<VulkanTextureCube> vulkanTexture = std::static_pointer_cast<VulkanTextureCube>(_textureCube);
	//	Shared<VulkanPipelineState> vulkanPSO = std::static_pointer_cast<VulkanPipelineState>(activePipelineState);

	//	vk::DescriptorImageInfo imageInfo{};
	//	imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	//	imageInfo.imageView = vulkanTexture->GetImageView();
	//	imageInfo.sampler = vulkanTexture->GetSampler();

	//	vk::WriteDescriptorSet writeSet = {};
	//	//writeSet.dstSet = sets[resourceInfo.set].get();
	//	writeSet.dstBinding = resourceInfo->binding;  // 특정 binding만 업데이트
	//	writeSet.descriptorCount = 1;
	//	writeSet.descriptorType = vk::DescriptorType::eCombinedImageSampler;
	//	writeSet.pImageInfo = &imageInfo;

	void VulkanRenderContext::BindShaderResourceView(const String& _name, const TextureView* _textureView, const Sampler* _sampler)
	{
		const ShaderReflectionData* resourceInfo = currentGraphicsPipelineState->GetBindingInfo(_name);
		if (resourceInfo == nullptr) return;

		const VulkanTextureView* vulkanTextureView = Cast<const VulkanTextureView*>(_textureView);
		const VulkanSampler* vulkanSampler = Cast<const VulkanSampler*>(_sampler);
		const VulkanGraphicsPipelineState* vulkanPSO = Cast<const VulkanGraphicsPipelineState*>(currentGraphicsPipelineState);

		vk::DescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
		imageInfo.imageView = vulkanTextureView->GetVkImageView();
		imageInfo.sampler = vulkanSampler->GetVkSampler();

		vk::WriteDescriptorSet writeSet = {};
		//writeSet.dstSet = sets[resourceInfo.set].get();
		writeSet.dstBinding = resourceInfo->binding;  // 특정 binding만 업데이트
		writeSet.descriptorCount = 1;
		writeSet.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		writeSet.pImageInfo = &imageInfo;

		GetActiveCommandBuffer().pushDescriptorSet(
			vk::PipelineBindPoint::eGraphics,
			vulkanPSO->GetPipelineLayout(),
			resourceInfo->set,
			1,
			&writeSet
		);
	}

	void Daydream::VulkanRenderContext::BindConstantBuffer(const String& _name, const GPUBuffer* _buffer)
	{
		if (_buffer == nullptr) return;
		const ShaderReflectionData* resourceInfo = currentGraphicsPipelineState->GetBindingInfo(_name);
		if (resourceInfo == nullptr) return;

		const VulkanGPUBuffer* constantBuffer = Cast<const VulkanGPUBuffer*>(_buffer);
		const VulkanGraphicsPipelineState* vulkanPSO = Cast<const VulkanGraphicsPipelineState*>(currentGraphicsPipelineState);

		vk::DescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = constantBuffer->GetVkBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = constantBuffer->GetSize();

		vk::WriteDescriptorSet writeSet = {};
		//writeSet.dstSet = sets[resourceInfo.set].get();
		writeSet.dstBinding = resourceInfo->binding;  // 특정 binding만 업데이트
		writeSet.descriptorCount = 1;
		writeSet.descriptorType = vk::DescriptorType::eUniformBuffer;
		writeSet.pBufferInfo = &bufferInfo;

		GetActiveCommandBuffer().pushDescriptorSet(
			vk::PipelineBindPoint::eGraphics,
			vulkanPSO->GetPipelineLayout(),
			resourceInfo->set,
			1,
			&writeSet
		);
	}

	void VulkanRenderContext::CopyBuffer(const GPUBuffer* _src, const GPUBuffer* _dst, UInt32 _copySize, UInt32 _srcOffset, UInt32 _dstOffset)
	{
		const VulkanGPUBuffer* src = Cast<const VulkanGPUBuffer*>(_src);
		const VulkanGPUBuffer* dst = Cast<const VulkanGPUBuffer*>(_dst);

		vk::BufferCopy copyRegion{};
		copyRegion.srcOffset = _srcOffset; // Optional
		copyRegion.dstOffset = _dstOffset; // Optional
		copyRegion.size = _copySize;

		GetActiveCommandBuffer().copyBuffer(src->GetVkBuffer(), dst->GetVkBuffer(), 1, &copyRegion);
	}

	void VulkanRenderContext::CopyBufferToTexture(const GPUBuffer* _src, const GPUTexture* _dst)
	{
		const VulkanGPUBuffer* src = Cast<const VulkanGPUBuffer*>(_src);
		const VulkanGPUTexture* dst = Cast<const VulkanGPUTexture*>(_dst);

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
			dst->GetWidth(),
			dst->GetHeight(),
			1
		);

		GetActiveCommandBuffer().copyBufferToImage(src->GetVkBuffer(), dst->GetVkImage(), vk::ImageLayout::eTransferDstOptimal, 1, &region);
	}

	void VulkanRenderContext::CopyTexture(const GPUTexture* _src, const GPUTexture* _dst, const TextureCopyRegion& _region)
	{
		const VulkanGPUTexture* dst = Cast<const VulkanGPUTexture*>(_dst);
		const VulkanGPUTexture* src = Cast<const VulkanGPUTexture*>(_src);

		vk::ImageCopy copyRegion = {};
		copyRegion.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		copyRegion.srcSubresource.mipLevel = _region.srcSubresource.mipLevel;
		copyRegion.srcSubresource.baseArrayLayer = _region.srcSubresource.baseLayer;
		copyRegion.srcSubresource.layerCount = _region.srcSubresource.layerCount;

		copyRegion.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		copyRegion.dstSubresource.mipLevel = _region.dstSubresource.mipLevel;
		copyRegion.dstSubresource.baseArrayLayer = _region.dstSubresource.baseLayer;
		copyRegion.dstSubresource.layerCount = _region.dstSubresource.layerCount;

		copyRegion.extent.width = _region.extent[0];
		copyRegion.extent.height = _region.extent[1];
		copyRegion.extent.depth = _region.extent[2];

		GetActiveCommandBuffer().copyImage(
			src->GetVkImage(), vk::ImageLayout::eTransferSrcOptimal,
			dst->GetVkImage(), vk::ImageLayout::eTransferDstOptimal,
			1, &copyRegion
		);
	}

	void Daydream::VulkanRenderContext::GenerateMips(GPUTexture* _texture)
	{
		//vk::CommandBuffer commandBuffer = device->BeginSingleTimeCommands(); // 이 함수는 vk::CommandBuffer를 반환한다고 가정

		Int32 mipWidth = _texture->GetWidth();
		Int32 mipHeight = _texture->GetHeight();
		UInt32 layerCount = _texture->GetLayerCount();
		UInt32 mipLevels = _texture->GetMipLevels();
		vk::Image image;

		image = Cast<VulkanGPUTexture*>(_texture)->GetVkImage();

		//input texture state will be RenderTarget, but vulkan need to use blitCopy to generate mips easy.
		//then Transition layout should be eTransferDstOptimal
		vk::ImageMemoryBarrier barrier{};
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = layerCount;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mipLevels;
		barrier.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
		barrier.newLayout = vk::ImageLayout::eTransferDstOptimal;
		barrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		// commandBuffer의 멤버 함수 pipelineBarrier 호출
		GetActiveCommandBuffer().pipelineBarrier(
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			vk::PipelineStageFlagBits::eTransfer,
			{},
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		for (UInt32 i = 1; i < _texture->GetMipLevels(); i++)
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.subresourceRange.levelCount = 1;
			barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
			barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
			barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

			// commandBuffer의 멤버 함수 pipelineBarrier 호출
			GetActiveCommandBuffer().pipelineBarrier(
				vk::PipelineStageFlagBits::eTransfer,
				vk::PipelineStageFlagBits::eTransfer,
				{}, // vk::DependencyFlags
				0, nullptr,
				0, nullptr,
				1, &barrier
			);

			vk::ImageBlit blit{};
			blit.srcOffsets[0] = vk::Offset3D{ 0, 0, 0 };
			blit.srcOffsets[1] = vk::Offset3D{ mipWidth, mipHeight, 1 };
			blit.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = layerCount;
			blit.dstOffsets[0] = vk::Offset3D{ 0, 0, 0 };
			blit.dstOffsets[1] = vk::Offset3D{ mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = layerCount;

			// commandBuffer의 멤버 함수 blitImage 호출
			GetActiveCommandBuffer().blitImage(
				image, vk::ImageLayout::eTransferSrcOptimal,
				image, vk::ImageLayout::eTransferDstOptimal,
				1, &blit,
				vk::Filter::eLinear
			);

			if (mipWidth > 1) mipWidth /= 2;
			if (mipHeight > 1) mipHeight /= 2;
		}

		Array<vk::ImageMemoryBarrier> barriers(2);
		barriers[0].image = image;
		barriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barriers[0].subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barriers[0].subresourceRange.baseArrayLayer = 0;
		barriers[0].subresourceRange.layerCount = layerCount;
		barriers[0].subresourceRange.baseMipLevel = 0;
		barriers[0].subresourceRange.levelCount = mipLevels - 1;
		barriers[0].oldLayout = vk::ImageLayout::eTransferSrcOptimal;
		barriers[0].newLayout = vk::ImageLayout::eColorAttachmentOptimal;
		barriers[0].srcAccessMask = vk::AccessFlagBits::eTransferRead;
		barriers[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;


		barriers[1].image = image;
		barriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barriers[1].subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barriers[1].subresourceRange.baseArrayLayer = 0;
		barriers[1].subresourceRange.layerCount = layerCount;
		barriers[1].subresourceRange.baseMipLevel = mipLevels - 1;
		barriers[1].subresourceRange.levelCount = 1;
		barriers[1].oldLayout = vk::ImageLayout::eTransferDstOptimal;
		barriers[1].newLayout = vk::ImageLayout::eColorAttachmentOptimal;
		barriers[1].srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barriers[1].dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;


		GetActiveCommandBuffer().pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,
			vk::PipelineStageFlagBits::eColorAttachmentOutput,
			{},
			0, nullptr,
			0, nullptr,
			2, barriers.data()
		);
	}

	void VulkanRenderContext::TransitionTextureState(const GPUTexture* _texture, ResourceState _beforeState, ResourceState _afterState, UInt32 _baseMip, UInt32 _mipLevels, UInt32 _baseLayer, UInt32 _layerCount)
	{
		if (_beforeState == _afterState)
		{
			DAYDREAM_RENDERER_WARN("Before State == After State");
			return;
		}

		const VulkanGPUTexture* vkTexture = Cast<const VulkanGPUTexture*>(_texture);

		vk::PipelineStageFlags srcStage;
		vk::AccessFlags srcAccess;
		std::tie(srcStage, srcAccess) = GraphicsUtility::Vulkan::ConvertToVulkanStageAndAccess(_beforeState);

		vk::PipelineStageFlags dstStage;
		vk::AccessFlags dstAccess;
		std::tie(dstStage, dstAccess) = GraphicsUtility::Vulkan::ConvertToVulkanStageAndAccess(_afterState);

		vk::ImageMemoryBarrier barrier{};
		barrier.image = vkTexture->GetVkImage();
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = GraphicsUtility::Vulkan::GetImageAspectFlags(vkTexture->GetFormat());
		barrier.subresourceRange.baseArrayLayer = _baseLayer;
		barrier.subresourceRange.layerCount = _layerCount;
		barrier.subresourceRange.baseMipLevel = _baseMip;
		barrier.subresourceRange.levelCount = _mipLevels;
		barrier.oldLayout = GraphicsUtility::Vulkan::ConvertToVulkanImageLayout(_beforeState);
		barrier.newLayout = GraphicsUtility::Vulkan::ConvertToVulkanImageLayout(_afterState);
		barrier.srcAccessMask = srcAccess;
		barrier.dstAccessMask = dstAccess;

		GetActiveCommandBuffer().pipelineBarrier
		(
			srcStage,               // srcStageMask
			dstStage,               // dstStageMask
			{}, // dependencyFlags
			0, nullptr,             // memoryBarriers (전역)
			0, nullptr,            // bufferMemoryBarriers
			1, &barrier              // imageMemoryBarriers
		);
		return;
	}


	void VulkanRenderContext::TransitionBufferState(const GPUBuffer* _buffer, ResourceState _beforeState, ResourceState _afterState)
	{
		if (_beforeState == _afterState)
		{
			DAYDREAM_RENDERER_WARN("Before State == After State");
			return;
		}

		const VulkanGPUBuffer* vkBuffer = Cast<const VulkanGPUBuffer*>(_buffer);

		vk::PipelineStageFlags srcStage;
		vk::AccessFlags srcAccess;
		std::tie(srcStage, srcAccess) = GraphicsUtility::Vulkan::ConvertToVulkanStageAndAccess(_beforeState);

		vk::PipelineStageFlags dstStage;
		vk::AccessFlags dstAccess;
		std::tie(dstStage, dstAccess) = GraphicsUtility::Vulkan::ConvertToVulkanStageAndAccess(_afterState);

		vk::BufferMemoryBarrier barrier;
		barrier.srcAccessMask = srcAccess;
		barrier.dstAccessMask = dstAccess;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.buffer = vkBuffer->GetVkBuffer();
		barrier.offset = 0;
		barrier.size = VK_WHOLE_SIZE;
		barrier.pNext = nullptr;

		GetActiveCommandBuffer().pipelineBarrier
		(
			srcStage,               // srcStageMask
			dstStage,               // dstStageMask
			{}, // dependencyFlags
			0, nullptr,             // memoryBarriers (전역)
			1, &barrier,            // bufferMemoryBarriers
			0, nullptr              // imageMemoryBarriers
		);

	}
	void Daydream::VulkanRenderContext::SetActiveCommandList(RenderCommandList* _commandList)
	{
		activeCommandList = _commandList;
		activeCommandBuffer = Cast<VulkanRenderCommandList*>(_commandList)->GetVkCommandBuffer();
	}

	vk::CommandBuffer VulkanRenderContext::GetActiveCommandBuffer()
	{
		return activeCommandBuffer;
	}
}
