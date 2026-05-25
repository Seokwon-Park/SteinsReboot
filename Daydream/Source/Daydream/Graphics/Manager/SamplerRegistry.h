#pragma once

#include "Daydream/Graphics/Resources/Sampler.h"
#include "ResourceRegistry.h"

namespace Daydream
{
	class SamplerRegistry : public ResourceRegistry<Sampler>
	{
	public:
		SamplerRegistry();
		virtual ~SamplerRegistry();

		virtual void CreateBuiltinResources() override;
	};
}