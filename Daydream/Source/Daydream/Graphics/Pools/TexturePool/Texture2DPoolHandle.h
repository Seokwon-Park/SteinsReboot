#pragma once

#include "Daydream/Graphics/Resources/Texture/Texture2D.h"
#include "Daydream/Graphics/Resources/Texture/TextureView.h"

namespace Daydream
{
	class Texture2DPoolHandle
	{
		friend class Texture2DPool;
	public:
		Texture2D* GetTexture() const { return texture.get(); }
		TextureView* GetRenderTargetView() const { return renderTargetView.get(); }
		TextureView* GetShaderResourceView() const { return shaderResourceView.get(); }
		TextureView* GetDepthStencilView() const { return depthStencilView.get(); }

		Texture2DPoolHandle() = default;
		~Texture2DPoolHandle() = default;

		Texture2DPoolHandle(const Texture2DPoolHandle&) = delete;
		Texture2DPoolHandle& operator=(const Texture2DPoolHandle&) = delete;
		Texture2DPoolHandle(Texture2DPoolHandle&&) noexcept = default;
		Texture2DPoolHandle& operator=(Texture2DPoolHandle&&) noexcept = default;
	private:
		UInt64 lastUsedLoop = 0;
		Shared<Texture2D> texture;
		Shared<TextureView> renderTargetView;
		Shared<TextureView> depthStencilView;
		Shared<TextureView> shaderResourceView;
	};
}
