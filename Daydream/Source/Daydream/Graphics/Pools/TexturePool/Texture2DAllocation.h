#pragma once

#include "Daydream/Graphics/Resources/Texture/Texture2D.h"
#include "Daydream/Graphics/Resources/Texture/TextureView.h"

namespace Daydream
{
	struct Texture2DAllocation
	{
		Shared<Texture2D> texture;
		Shared<TextureView> renderTargetView;
		Shared<TextureView> depthStencilView;
		Shared<TextureView> shaderResourceView; // Color or Depth
		Shared<TextureView> stencilShaderResourceView;
	};
}