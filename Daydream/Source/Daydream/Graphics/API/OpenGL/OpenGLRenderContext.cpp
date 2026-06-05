#include "DaydreamPCH.h"
#include "OpenGLRenderContext.h"

#include "OpenGLGraphicsPipelineState.h"
#include "OpenGLTexture.h"
#include "OpenGLTextureView.h"
#include "OpenGLTextureCube.h"
#include "OpenGLBuffer.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLUtility.h"

#include "glad/glad.h"

namespace Daydream
{
	OpenGLRenderContext::OpenGLRenderContext()
	{
		glCreateFramebuffers(1, &framebufferID);
	}
	void OpenGLRenderContext::SetViewport(UInt32 _x, UInt32 _y, UInt32 _width, UInt32 _height)
	{
		glViewport(_x, _y, _width, _height);
	}
	void OpenGLRenderContext::DrawIndexed(UInt32 _indexCount, UInt32 _startIndex, UInt32 _baseVertex)
	{
		glDrawElementsBaseVertex(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, (void*)(_startIndex * sizeof(uint32_t)), _baseVertex);
	}
	void OpenGLRenderContext::BeginRendering(const RenderingInfo& _renderingInfo)
	{
		Array<GLenum> drawBuffers;
		for (UInt32 i = 0; i < (UInt32)_renderingInfo.colorAttachments.size(); i++)
		{
			const AttachmentDesc& attachmentDesc = _renderingInfo.colorAttachments[i];
			OpenGLTextureView* openGLTextureView = Cast<OpenGLTextureView*>(attachmentDesc.view);

			glNamedFramebufferTexture(
				framebufferID,
				(GLenum)(GL_COLOR_ATTACHMENT0 + i),
				openGLTextureView->GetTextureViewID(),
				0
			);

			drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		}

		// Depth attachment 연결
		if (_renderingInfo.depthAttachment.view != nullptr)
		{
			OpenGLTextureView* openGLTextureView = Cast<OpenGLTextureView*>(_renderingInfo.depthAttachment.view);

			glNamedFramebufferTexture(framebufferID,
				GL_DEPTH_STENCIL_ATTACHMENT,
				openGLTextureView->GetTextureViewID(),
				0);
		}

		// Draw buffers 설정 (multiple render targets용)
		glNamedFramebufferDrawBuffers(framebufferID,
			(UInt32)drawBuffers.size(),
			drawBuffers.data());


		glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
		DAYDREAM_CORE_ASSERT(glCheckNamedFramebufferStatus(framebufferID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		for (UInt64 i = 0; i < _renderingInfo.colorAttachments.size(); i++)
		{
			const AttachmentDesc& attachmentDesc = _renderingInfo.colorAttachments[i];
			ClearValue rtvClearValue = attachmentDesc.clearValue;

			if (attachmentDesc.loadOp == AttachmentLoadOp::Clear)
			{
				glClearNamedFramebufferfv(framebufferID, GL_COLOR, (GLint)i, rtvClearValue.colorClearValue.color);
			}
		}

		if (_renderingInfo.depthAttachment.view != nullptr)
		{
			ClearValue dsvClearValue = _renderingInfo.depthAttachment.clearValue;
			OpenGLTextureView* openGLTextureView = Cast<OpenGLTextureView*>(_renderingInfo.depthAttachment.view);

			float depthValue = _renderingInfo.depthAttachment.clearValue.depthClearValue;
			int stencilValue = _renderingInfo.depthAttachment.clearValue.stencilClearValue;
			if (_renderingInfo.depthAttachment.loadOp == AttachmentLoadOp::Clear)
			{
				glClearNamedFramebufferfi(framebufferID, GL_DEPTH_STENCIL, 0, depthValue, stencilValue);
			}
		}

		SetViewport(
			_renderingInfo.renderArea.x,
			_renderingInfo.renderArea.y,
			_renderingInfo.renderArea.width,
			_renderingInfo.renderArea.height
		);
		////glNamedFramebufferDrawBuffer(framebufferID, GL_COLOR_ATTACHMENT0);


	}
	void OpenGLRenderContext::EndRendering(const RenderingInfo& _renderingInfo)
	{
		for (UInt64 i = 0; i < _renderingInfo.colorAttachments.size(); i++)
		{
			glNamedFramebufferTexture(framebufferID,
				GL_COLOR_ATTACHMENT0 + (GLenum)i,
				0,
				0);
		}
		glNamedFramebufferTexture(framebufferID, GL_DEPTH_STENCIL_ATTACHMENT, 0, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	//void OpenGLRenderContext::BeginRenderPass(Shared<RenderPass> _renderPass, Shared<Framebuffer> _framebuffer)
	//{
	//	OpenGLFramebuffer* currentFramebuffer = Cast<OpenGLFramebuffer*>(_framebuffer.get());
	//	//if (currentFramebuffer->IsSwapchainBuffer() == true)
	//	//{
	//	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//	//	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	//	//	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	//}
	//	//else
	//	//{
	//	glBindFramebuffer(GL_FRAMEBUFFER, currentFramebuffer->GetFramebufferID());
	//	SetViewport(0, 0, currentFramebuffer->GetWidth(), currentFramebuffer->GetHeight());
	//	if (currentFramebuffer->HasDepthAttachment())
	//	{
	//		glEnable(GL_DEPTH_TEST);
	//		glDepthFunc(GL_LESS);
	//		glClear(GL_DEPTH_BUFFER_BIT);
	//	}
	//	else
	//	{
	//		glDisable(GL_DEPTH_TEST);
	//	}

	//	Color clearColor = _renderPass->GetClearColor();
	//	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	//	glClear(GL_COLOR_BUFFER_BIT);
	//	//}
	//}
	//void OpenGLRenderContext::EndRenderPass(Shared<RenderPass> _renderPass)
	//{
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//}
	void OpenGLRenderContext::BindPipelineState(const GraphicsPipelineState* _pipelineState)
	{
		RenderContext::BindPipelineState(_pipelineState);
		const OpenGLGraphicsPipelineState* pso = Cast<const OpenGLGraphicsPipelineState*>(currentGraphicsPipelineState);
		pso->BindPipelineState();
	}

	void OpenGLRenderContext::BindVertexBuffer(const GPUBuffer* _vertexBuffer, UInt32 _stride)
	{
		const OpenGLGPUBuffer* vertexBuffer = Cast<const OpenGLGPUBuffer*>(_vertexBuffer);

		GLint currentVAO = 0;
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
		glVertexArrayVertexBuffer(currentVAO, 0, vertexBuffer->GetBufferID(), 0, _stride);
	}

	void OpenGLRenderContext::BindIndexBuffer(const GPUBuffer* _indexBuffer)
	{
		const OpenGLGPUBuffer* indexBuffer = Cast<const OpenGLGPUBuffer*>(_indexBuffer);

		GLint currentVAO = 0; // 결과를 저장할 변수
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &currentVAO);
		glVertexArrayElementBuffer(currentVAO, indexBuffer->GetBufferID());
	}
	void OpenGLRenderContext::BindShaderResourceView(const String& _name, const TextureView* _textureView, const Sampler* _sampler)
	{
		const ShaderReflectionData* bindingInfo = currentGraphicsPipelineState->GetBindingInfo(_name);
		if (bindingInfo == nullptr) return;

		const OpenGLTextureView* glView = Cast<const OpenGLTextureView*>(_textureView);
		const OpenGLSampler* glSampler = Cast<const OpenGLSampler*>(_sampler);
		glBindTextureUnit(bindingInfo->binding, glView->GetTextureViewID());
		glBindSampler(bindingInfo->binding, glSampler->GetSamplerID());

	}
	/*void OpenGLRenderContext::SetTexture2D(const String& _name, Shared<Texture2D> _texture)
	{
		RenderContext::SetTexture2D(_name, _texture);

		const ShaderReflectionData* bindingInfo = activePipelineState->GetBindingInfo(_name);
		if (bindingInfo == nullptr) return;

		OpenGLTexture2D* glTexture = Cast<OpenGLTexture2D*>(_texture.get());
		glBindTextureUnit(bindingInfo->binding, glTexture->GetTextureID());
		glBindSampler(bindingInfo->binding, glTexture->GetSamplerID());
	}
	void OpenGLRenderContext::SetTextureCube(const String& _name, Shared<TextureCube> _textureCube)
	{
		const ShaderReflectionData* bindingInfo = activePipelineState->GetBindingInfo(_name);
		if (bindingInfo == nullptr) return;

		OpenGLTextureCube* glTexture = Cast<OpenGLTextureCube*>(_textureCube.get());
		glBindTextureUnit(bindingInfo->binding, glTexture->GetTextureID());
		glBindSampler(bindingInfo->binding, glTexture->GetSamplerID());
	}*/
	void Daydream::OpenGLRenderContext::BindConstantBuffer(const String& _name, const GPUBuffer* _buffer)
	{
		const ShaderReflectionData* bindingInfo = currentGraphicsPipelineState->GetBindingInfo(_name);
		if (bindingInfo == nullptr) return;

		const OpenGLGPUBuffer* constantBuffer = Cast<const OpenGLGPUBuffer*>(_buffer);
		glBindBufferBase(GL_UNIFORM_BUFFER, bindingInfo->binding, constantBuffer->GetBufferID());
	}

	void OpenGLRenderContext::CopyBuffer(const GPUBuffer* _src, const GPUBuffer* _dst, UInt32 _copySize, UInt32 _srcOffset, UInt32 _dstOffset)
	{
		const OpenGLGPUBuffer* src = Cast<const OpenGLGPUBuffer*>(_src);
		const OpenGLGPUBuffer* dst = Cast<const OpenGLGPUBuffer*>(_dst);

		// 소스 버퍼ID, 목적지 버퍼ID, 소스 오프셋, 목적지 오프셋, 복사할 크기
		glCopyNamedBufferSubData(
			src->GetBufferID(),
			dst->GetBufferID(),
			_srcOffset,
			_dstOffset,
			_copySize
		);
	}

	void OpenGLRenderContext::CopyBufferToTexture(const GPUBuffer* _src, const GPUTexture* _dst)
	{
		const OpenGLGPUBuffer* srcBuffer = Cast<const OpenGLGPUBuffer*>(_src);
		const OpenGLGPUTexture* dstTexture = Cast<const OpenGLGPUTexture*>(_dst);

		GLuint bufferID = srcBuffer->GetBufferID();
		GLuint textureID = dstTexture->GetTextureID();

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, bufferID);

		GLenum pixelFormat = GraphicsUtility::OpenGL::ConvertRenderFormatToGLDataFormat(dstTexture->GetFormat());
		GLenum pixelType = GraphicsUtility::OpenGL::ConvertRenderFormatToGLDataType(dstTexture->GetFormat());

		glTextureSubImage2D(
			textureID,
			0,           // 밉맵 레벨
			0,           // X 오프셋
			0,           // Y 오프셋
			_dst->GetWidth(),      // 복사할 너비
			_dst->GetHeight(),     // 복사할 높이
			pixelFormat, // 픽셀 포맷
			pixelType,   // 데이터 타입
			nullptr      // PBO 오프셋
		);

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}

	void OpenGLRenderContext::CopyTexture(const GPUTexture* _src, const GPUTexture* _dst, const TextureCopyRegion& _region)
	{
		const OpenGLGPUTexture* src = Cast<const OpenGLGPUTexture*>(_src);
		const OpenGLGPUTexture* dst = Cast<const OpenGLGPUTexture*>(_dst);

		UInt32 srcZ = _region.srcOffset[2] + _region.srcSubresource.baseLayer;
		UInt32 dstZ = _region.dstOffset[2] + _region.dstSubresource.baseLayer;

		UInt32 depthExtent = std::max(_region.extent[2], _region.srcSubresource.layerCount);

		glCopyImageSubData(
			src->GetTextureID(),								// 원본 텍스처 이름
			GL_TEXTURE_2D,										// 원본 텍스처 타입
			_region.srcSubresource.mipLevel,					// 원본 밉맵 레벨
			_region.srcOffset[0], _region.srcOffset[1], srcZ,	// 원본 오프셋 (x, y, z)
			dst->GetTextureID(),						        // 대상 텍스처 이름
			GL_TEXTURE_2D,										// 대상 텍스처 타입
			_region.dstSubresource.mipLevel,					// 대상 밉맵 레벨
			_region.dstOffset[0], _region.dstOffset[1], dstZ,	// 대상 오프셋 (x, y, z)
			_region.extent[0],									// 복사할 너비
			_region.extent[1],									// 복사할 높이
			depthExtent											// 복사할 깊이 (2D 텍스처는 1)
		);
	}

	void OpenGLRenderContext::GenerateMips(GPUTexture* _texture)
	{
		const OpenGLGPUTexture* glTexture = Cast<const OpenGLGPUTexture*>(_texture);

		UInt32 mipLevels = _texture->GetMipLevels();
		UInt32 layerCount = _texture->GetLayerCount();

		GLenum target = GraphicsUtility::OpenGL::ConvertToOpenGLTextureTarget(_texture->GetType());
		GLenum internalFormat = GraphicsUtility::OpenGL::ConvertRenderFormatToGLFormat(_texture->GetFormat());

		GLuint srvID;
		glGenTextures(1, &srvID);
		glTextureView(srvID, target, glTexture->GetTextureID(), internalFormat, 0, mipLevels, 0, layerCount);

		glGenerateTextureMipmap(srvID);
	}
}