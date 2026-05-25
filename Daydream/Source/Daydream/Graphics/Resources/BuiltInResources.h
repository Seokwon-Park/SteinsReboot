#pragma once

#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"
#include "Daydream/Graphics/Resources/Sampler.h"

namespace Daydream
{
	namespace BuiltIn
	{
		namespace PSO
		{
			const GraphicsPipelineState* Depth();
			const GraphicsPipelineState* GBuffer();
			const GraphicsPipelineState* Deferred();
		}

		namespace Samplers
		{
			const Sampler* LinearRepeat();
			const Sampler* LinearClampToEdge();
			const Sampler* NearestRepeat();
			const Sampler* NearestClampToEdge();
		}
	}
}
