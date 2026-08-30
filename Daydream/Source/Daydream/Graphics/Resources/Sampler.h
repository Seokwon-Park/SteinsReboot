#pragma once

namespace Daydream
{


    struct SamplerDesc
    {
        FilterMode minFilter = FilterMode::Linear;
        FilterMode magFilter = FilterMode::Linear;
        FilterMode mipFilter = FilterMode::Linear;

        WrapMode wrapU = WrapMode::Repeat;
        WrapMode wrapV = WrapMode::Repeat;
        WrapMode wrapW = WrapMode::Repeat;

        float minLod = 0.0f;
        float maxLod = 1000.0f;
        float lodBias = 0.0f;

        float maxAnisotropy = 1.0f;
        bool comparisonEnable = false;
        ComparisonFunc comparisonFunc = ComparisonFunc::Never;
        float borderColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

        bool operator==(const SamplerDesc&) const = default;
    };

	class Sampler
	{
	public:
		virtual ~Sampler() = default;

        static Shared<Sampler> Create(const SamplerDesc& _desc);
	private:

	};
}