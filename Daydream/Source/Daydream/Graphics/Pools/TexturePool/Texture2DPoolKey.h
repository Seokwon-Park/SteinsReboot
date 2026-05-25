#pragma once

namespace Daydream
{
	struct Texture2DPoolKey
	{
		UInt32 width;
		UInt32 height;
		RenderFormat format;

		bool operator==(const Texture2DPoolKey& _other) const = default;
	};
}

template <>
struct std::hash<Daydream::Texture2DPoolKey>
{
	inline size_t operator()(const Daydream::Texture2DPoolKey& _key) const
	{
		uint64_t packed =
			(static_cast<uint64_t>(_key.width) << 32) |
			(static_cast<uint64_t>(_key.height) << 16) |
			static_cast<uint64_t>(_key.format);

		return std::hash<uint64_t>{}(packed);
	}
};
