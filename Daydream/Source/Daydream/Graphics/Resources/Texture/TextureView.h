#pragma once

#include "Daydream/Graphics/Resources/Texture/Texture.h"

namespace Daydream
{
	struct TextureViewDesc
	{
		TextureViewType type = TextureViewType::ShaderResource;
		RenderFormat format = RenderFormat::UNKNOWN; 

		UInt32 baseMip = 0;
		UInt32 mipLevels = 1;

		UInt32 baseLayer = 0;
		UInt32 layerCount = 1;

		bool readOnlyDepth = false;
		bool readOnlyStencil = false;
	};

	class TextureView
	{
	public:
		TextureView(GPUTexture* _texture, const TextureViewDesc& _desc);
		virtual ~TextureView();

		virtual inline void* GetUIHandle() const { return nullptr; };

		inline UInt32 GetWidth() const { return Math::Max(1u, originTexture->GetWidth() >> desc.baseMip); }
		inline UInt32 GetHeight() const { return Math::Max(1u, originTexture->GetHeight() >> desc.baseMip); }

		inline GPUTexture* GetOriginTexture() const { return originTexture; }
		inline const TextureViewDesc& GetDesc() { return desc; }

		static Shared<TextureView> Create(Texture* _texture, const TextureViewDesc& _desc);
		static Shared<TextureView> Create(const Shared<Texture>& _texture, const TextureViewDesc& _desc);
	protected:
		TextureViewDesc desc;
		GPUTexture* originTexture;
	private:

	};
}
