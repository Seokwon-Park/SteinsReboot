#pragma once

#include "Steins/Graphics/Resources/Framebuffer.h"

namespace Steins
{
	class  OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(const FramebufferDesc& _desc);
		virtual ~OpenGLFramebuffer() override;
	private:
		UInt32 framebufferID;
		Array<UInt32> colorAttachments;
		UInt32 depthAttachment;
	};
}