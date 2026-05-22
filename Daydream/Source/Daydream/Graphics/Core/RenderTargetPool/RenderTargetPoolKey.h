#pragma once

namespace Daydream
{
	struct RenderTargetPoolKey
	{
		UInt32 width;
		UInt32 height;
		RenderFormat format;

		bool operator==(const RenderTargetPoolKey& _other) const = default;
	};
}

template <>
struct std::hash<Daydream::RenderTargetPoolKey>
{
	inline size_t operator()(const Daydream::RenderTargetPoolKey& _key) const
	{
		uint64_t packed =
			(static_cast<uint64_t>(_key.width) << 32) |
			(static_cast<uint64_t>(_key.height) << 16) |
			static_cast<uint64_t>(_key.format);

		return std::hash<uint64_t>{}(packed);
	}
};
