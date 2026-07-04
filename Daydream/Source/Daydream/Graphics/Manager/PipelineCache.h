#pragma once

#include "ResourceRegistry.h"
#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"

namespace Daydream
{
	struct GraphicsPipelineStateHash
	{
		UInt64 operator()(const GraphicsPipelineStateDesc& _desc) const;
	};

	class PipelineStateCache
	{
	public:
		PipelineStateCache();
		virtual ~PipelineStateCache();

		GraphicsPipelineState* RequestPipelineState(const GraphicsPipelineStateDesc& _desc);

	private:
		HashMap<GraphicsPipelineStateDesc,
			Shared<GraphicsPipelineState>,
			GraphicsPipelineStateHash> cache;
	};
}