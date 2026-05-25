#pragma once

#include "Daydream/Graphics/Resources/Texture/TextureView.h"

namespace Daydream
{
	struct RenderArea
	{
		Int32 x = 0;
		Int32 y = 0;
		UInt32 width = 0;
		UInt32 height = 0;
	};

	struct ClearValue
	{
		Color colorClearValue = Color::Blue;
		Float32 depthClearValue{};
		UInt8 stencilClearValue{};
	};

	struct AttachmentDesc
	{
		TextureView* view; // 여기에 포맷이 이미 내장되어 있음
		AttachmentLoadOp  loadOp = AttachmentLoadOp::Clear;
		AttachmentStoreOp storeOp = AttachmentStoreOp::Store;
		ClearValue clearValue = ClearValue();
	};

	struct RenderingInfo
	{
		RenderArea renderArea;

		Array<AttachmentDesc> colorAttachments;
		AttachmentDesc depthAttachment;
	};
}
