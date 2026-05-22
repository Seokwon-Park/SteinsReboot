#pragma once

#include "ResourceRegistry.h"


namespace Daydream
{
	class GraphicsPipelineState;

	class PipelineStateRegistry : public ResourceRegistry<GraphicsPipelineState>
	{
	public:
		PipelineStateRegistry();
		virtual ~PipelineStateRegistry();

		virtual void CreateBuiltinResources() override;

		inline static Shared<GraphicsPipelineState> DepthPSO;
		inline static Shared<GraphicsPipelineState> GBufferPSO;
		inline static Shared<GraphicsPipelineState> DeferredPSO;
	private:
	};
}