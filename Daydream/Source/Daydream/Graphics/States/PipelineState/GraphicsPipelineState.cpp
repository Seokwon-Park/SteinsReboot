#include "DaydreamPCH.h"
#include "GraphicsPipelineState.h"

#include "Daydream/Core/Application.h"

#include "Daydream/Graphics/Core/Renderer.h"

namespace Daydream
{
	GraphicsPipelineState::GraphicsPipelineState(const GraphicsPipelineStateDesc& _desc)
	{
		desc = _desc;
		shaderGroup = _desc.shaderGroup;
	}

	Shared<GraphicsPipelineState> GraphicsPipelineState::Create(const GraphicsPipelineStateDesc& _desc)
	{
		return Renderer::GetRenderDevice()->CreatePipelineState(_desc);
	}
}
