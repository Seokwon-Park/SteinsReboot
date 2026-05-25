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


	private:
	};
}