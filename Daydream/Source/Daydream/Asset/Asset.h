#pragma once

#include "Daydream/Core/UUID.h"

namespace Daydream
{
#define ASSET_CLASS_TYPE(type) static AssetType GetStaticType() { return AssetType::##type; }\
								virtual AssetType GetAssetType() const override {return GetStaticType();}\

	enum class AssetType
	{
		None,
		Texture2D,
		TextureCube,
		Shader,
		ShaderPipeline,
		Scene,
		Model,
		Mesh,
		Material,
		Script
	};

	class Asset
	{
	public:
		Asset();
		~Asset();

		virtual AssetType GetAssetType() const = 0;

		inline void SetAssetHandle(AssetHandle _id) { id = _id; }
		inline AssetHandle GetAssetHandle() { return id; }

		inline void SetAssetName(String _name) { name = _name; }
		inline const String& GetAssetName() const
		{
			if (name.empty())
			{
				DAYDREAM_CORE_INFO("No Name");
			}
			return name;
		}
	protected:

	private:
		AssetType type = AssetType::None;
		AssetHandle id = AssetHandle();
		String name = "";
	};
}
