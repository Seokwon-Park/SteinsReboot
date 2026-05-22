#include "DaydreamPCH.h"
#include "TextureView.h"

#include "Daydream/Graphics/Core/Renderer.h"

namespace Daydream
{

	TextureView::TextureView(GPUTexture* _texture, const TextureViewDesc& _desc)
	{
		desc = _desc;
		originTexture = _texture;
	}

	TextureView::~TextureView()
	{
		DAYDREAM_CORE_WARN("Why?");
		originTexture = nullptr;
	}

	Shared<TextureView> TextureView::Create(Texture* _texture, const TextureViewDesc& _desc)
	{
		return Renderer::GetRenderDevice()->CreateTextureView(_texture, _desc);
	}

	Shared<TextureView> TextureView::Create(const Shared<Texture>& _texture, const TextureViewDesc& _desc)
	{
		return Create(_texture.get(), _desc);
	}
}

