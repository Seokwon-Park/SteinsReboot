#pragma once

#include "Daydream/Graphics/Resources/Texture/Texture2D.h"
#include "Daydream/Graphics/Resources/Texture/TextureView.h"

namespace Daydream
{
	class RenderTargetPoolHandle
	{
		friend class RenderTargetPool;
	public:
		TextureView* GetRenderTargetView() { return renderTargetView.get(); }
		TextureView* GetShaderResourceView() { return shaderResourceView.get(); }

		RenderTargetPoolHandle() = default;
		~RenderTargetPoolHandle() = default;

		RenderTargetPoolHandle(const RenderTargetPoolHandle&) = delete;
		RenderTargetPoolHandle& operator=(const RenderTargetPoolHandle&) = delete;
		RenderTargetPoolHandle(RenderTargetPoolHandle&&) noexcept = default;
		RenderTargetPoolHandle& operator=(RenderTargetPoolHandle&&) noexcept = default;
	private:
		UInt32 lastUsedLoop = 0;
		Shared<Texture2D> texture;
		Shared<TextureView> renderTargetView;
		Shared<TextureView> shaderResourceView;
	};

}
