#pragma once

#include "Daydream/Graphics/Resources/Sampler.h"

namespace Daydream
{
	class SamplerCache
	{
	public:
		SamplerCache();
		virtual ~SamplerCache();

		Sampler* RequestSampler(const SamplerDesc& _desc);

		HashMap<String, Shared<Sampler>> cache;

	};
}