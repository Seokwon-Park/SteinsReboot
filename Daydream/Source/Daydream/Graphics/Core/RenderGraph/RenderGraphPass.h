#pragma once

#include "RenderGraphResource.h"
#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"

namespace Daydream
{
	struct RenderGraphWriteBinding
	{
		String name;
		AttachmentLoadOp  loadOp = AttachmentLoadOp::Clear;
		AttachmentStoreOp storeOp = AttachmentStoreOp::Store;
	};

	class RenderGraphPass
	{
	public:
		RenderGraphPass();
		~RenderGraphPass();

		void CompilePass(ShaderPipeline* _pipeline);
		void SetReadResource(UInt32 _slot, UInt32 _resourceHandle);
		void SetWriteResource(UInt32 _slot, UInt32 _resourceHandle);

			

	protected:

	private:
		String name;
		ShaderPipeline* pipeline;
		Array<UInt32> reads;
		Array<UInt32> colorWrites;
		UInt32 depthWrite;

		UInt64 nameHash;
	};
}
