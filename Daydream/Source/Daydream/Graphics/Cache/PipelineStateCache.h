#pragma once

#include "GraphicsResourceCache.h"
#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"

namespace Daydream
{
	struct GraphicsPipelineStateHash
	{
		UInt64 operator()(const GraphicsPipelineStateDesc& _desc) const;
	};

	class PipelineStateCache : public GraphicsResourceCache<GraphicsPipelineStateDesc, GraphicsPipelineState, GraphicsPipelineStateHash>
	{
	public:
		PipelineStateCache();
		virtual ~PipelineStateCache();

		virtual Shared<GraphicsPipelineState> CreateResource(const GraphicsPipelineStateDesc& _key) override;

	private:

	};
}