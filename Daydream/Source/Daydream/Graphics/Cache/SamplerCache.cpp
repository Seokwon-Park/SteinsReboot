#include "DaydreamPCH.h"
#include "SamplerCache.h"

#include "Daydream/Graphics/Resources/BuiltinResources.h"
#include "xxhash.h"

namespace Daydream
{
	namespace
	{
		const Sampler* internalLinearRepeat = nullptr;
		const Sampler* internalLinearClampToEdge = nullptr;
		const Sampler* internalNearestRepeat = nullptr;
		const Sampler* internalNearestClampToEdge = nullptr;
	}

	const Sampler* BuiltIn::Samplers::LinearRepeat() { return internalLinearRepeat; }
	const Sampler* BuiltIn::Samplers::LinearClampToEdge() { return internalLinearClampToEdge; }
	const Sampler* BuiltIn::Samplers::NearestRepeat() { return internalNearestRepeat; }
	const Sampler* BuiltIn::Samplers::NearestClampToEdge() { return internalNearestClampToEdge; }


	SamplerCache::SamplerCache()
	{
	}

	SamplerCache::~SamplerCache()
	{
	
	}

	Shared<Sampler> SamplerCache::CreateResource(const SamplerDesc& _key)
	{
		return Sampler::Create(_key);
	}



	//void SamplerCache::CreateBuiltinResources()
	//{
	//	SamplerDesc samplerDesc;
	//	samplerDesc.minFilter = FilterMode::Linear;
	//	samplerDesc.magFilter = FilterMode::Linear;
	//	samplerDesc.mipFilter = FilterMode::Linear;
	//	samplerDesc.wrapU = WrapMode::Repeat;
	//	samplerDesc.wrapV = WrapMode::Repeat;
	//	samplerDesc.wrapW = WrapMode::Repeat;
	//	registry["LinearRepeat"] = Sampler::Create(samplerDesc);
	//	internalLinearRepeat = registry["LinearRepeat"].get();

	//	// 선형 샘플러 (클램프)
	//	samplerDesc.minFilter = FilterMode::Linear;
	//	samplerDesc.magFilter = FilterMode::Linear;
	//	samplerDesc.mipFilter = FilterMode::Linear;
	//	samplerDesc.wrapU = WrapMode::ClampToEdge;
	//	samplerDesc.wrapV = WrapMode::ClampToEdge;
	//	samplerDesc.wrapW = WrapMode::ClampToEdge;
	//	registry["LinearClampToEdge"] = Sampler::Create(samplerDesc);
	//	internalLinearClampToEdge = registry["LinearRepeat"].get();

	//	// 가장 가까운 샘플러 (반복)
	//	samplerDesc.minFilter = FilterMode::Nearest;
	//	samplerDesc.magFilter = FilterMode::Nearest;
	//	samplerDesc.mipFilter = FilterMode::Nearest;
	//	samplerDesc.wrapU = WrapMode::Repeat;
	//	samplerDesc.wrapV = WrapMode::Repeat;
	//	samplerDesc.wrapW = WrapMode::Repeat;
	//	registry["NearestRepeat"] = Sampler::Create(samplerDesc);
	//	internalNearestRepeat = registry["NearestRepeat"].get();
	//	
	//	// 가장 가까운 샘플러 (클램프)
	//	samplerDesc.minFilter = FilterMode::Nearest;
	//	samplerDesc.magFilter = FilterMode::Nearest;
	//	samplerDesc.mipFilter = FilterMode::Nearest;
	//	samplerDesc.wrapU = WrapMode::ClampToEdge;
	//	samplerDesc.wrapV = WrapMode::ClampToEdge;
	//	samplerDesc.wrapW = WrapMode::ClampToEdge;
	//	registry["NearestClampToEdge"] = Sampler::Create(samplerDesc);
	//	internalNearestClampToEdge = registry["NearestClampToEdge"].get();
	//}

	UInt64 SamplerHash::operator()(const SamplerDesc& _desc) const
	{
		return XXH64(&_desc, sizeof(SamplerDesc), 0);
	}

}