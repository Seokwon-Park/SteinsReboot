#pragma once

#define UUID_SYSTEM_GENERATOR
#include "stduuid.h"

namespace Daydream
{
	class AssetHandle
	{
	public:
		AssetHandle();
		explicit AssetHandle(const String& _string);
		AssetHandle(const uuids::uuid& _other);

		String ToString() const;
		AssetHandle StringToUUID(const String& _string) const;
		bool IsValid() const;

		bool operator==(const AssetHandle& _other) const;
		bool operator!=(const AssetHandle& _other) const;
		bool operator<(const AssetHandle& _other) const;

		static AssetHandle Generate();
	private:
		static uuids::uuid GenerateUUID();
		uuids::uuid uuid;

		friend struct std::hash<Daydream::AssetHandle>;
	};
}

template <>
struct std::hash<Daydream::AssetHandle>
{
	Daydream::UInt64 operator()(const Daydream::AssetHandle& _id) const
	{
		return std::hash<uuids::uuid>{}(_id.uuid);
	}
};
