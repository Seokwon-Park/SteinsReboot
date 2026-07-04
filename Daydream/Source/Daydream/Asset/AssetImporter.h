#pragma once

#include "AssetManager.h"
#include "Daydream/Graphics/Resources/Texture/Texture.h"
#include "Daydream/Graphics/Resources/Model.h"
#include "Daydream/Graphics/Resources/Shader.h"
#include "Daydream/Graphics/Resources/ShaderPipeline.h"


namespace Daydream
{
	class AssetImporter
	{
	public:
		static Shared<Texture2D> LoadTexture2D(const AssetMetadata& _metaData);
		static Shared<Model> LoadModel(const AssetMetadata& _metaData);
		static Shared<Shader> LoadShader(const AssetMetadata& _metaData);
		static Shared<Material> LoadMaterial(const AssetMetadata& _metaData);
	protected:

	private:

	};
}
