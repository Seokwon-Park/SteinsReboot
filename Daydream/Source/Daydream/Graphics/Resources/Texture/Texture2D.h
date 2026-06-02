#pragma once

#include "Texture.h"

namespace Daydream
{
	class Texture2D : public Texture
	{
	public:
		ASSET_CLASS_TYPE(Texture2D)
		Texture2D(Shared<GPUTexture> _texture);
		virtual ~Texture2D() = default;

		virtual void* GetImGuiHandle() { return nullptr; }

		static Shared<Texture2D> Create(const Texture2DDesc& _desc, const void* _initialData = {});
	protected:
		Texture2D() = default;
	};
}
