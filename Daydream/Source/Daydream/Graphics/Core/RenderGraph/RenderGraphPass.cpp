#include "DaydreamPCH.h"
#include "RenderGraphPass.h"

namespace Daydream
{
	RenderGraphPass::RenderGraphPass()
	{
	}
	RenderGraphPass::~RenderGraphPass()
	{
	}
	void RenderGraphPass::CompilePass(ShaderPipeline* _pipeline)
	{
		pipeline = _pipeline;
		auto outputs = pipeline->GetOutputLayoutData();
		colorWrites.clear();
		colorWrites.resize(outputs.size());
	}
	void RenderGraphPass::SetReadResource(UInt32 _slot, UInt32 _resourceHandle)
	{
	}
	void RenderGraphPass::SetWriteResource(UInt32 _slot, UInt32 _resourceHandle)
	{
	}
}

