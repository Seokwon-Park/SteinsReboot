#pragma once

#include "GraphicsResourceCache.h"
#include "Daydream/Graphics/Resources/Sampler.h"

namespace Daydream
{
	struct SamplerHash
	{
		UInt64 operator()(const SamplerDesc& _desc) const;
	};

	class SamplerCache : public GraphicsResourceCache<SamplerDesc, Sampler, SamplerHash>
	{
	public:
		SamplerCache();
		virtual ~SamplerCache();

		virtual Shared<Sampler> CreateResource(const SamplerDesc& _key) override;


	};
}