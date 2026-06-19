#include "DaydreamPCH.h"
#include "Daydream/Graphics/Resources/Texture/Texture.h"

#include "TextureView.h"

namespace Daydream
{
	GPUTexture::GPUTexture(const TextureDesc& _desc)
		:desc(_desc)
	{
		UInt32 maxMips = (UInt32)std::floor(std::log2(std::max(desc.width, desc.height))) + 1;

		if (desc.mipLevels == 0 || desc.mipLevels > maxMips)
		{
			DAYDREAM_CORE_WARN("mipLevel is to Large {} -> {}", std::max(desc.width, desc.height), maxMips);
			desc.mipLevels = maxMips;
		}
	}
	Texture::Texture(Shared<GPUTexture> _texture)
		: gpuTexture(_texture)
	{
		TextureViewDesc srvDesc{};
		srvDesc.type = TextureViewType::ShaderResource;
		srvDesc.baseMip = 0;
		srvDesc.mipLevels = GetMipLevels();
		srvDesc.baseLayer = 0;
		srvDesc.layerCount = GetLayerCount();

		defaultSRV = TextureView::Create(this, srvDesc);
	}
	Texture::~Texture()
	{
		defaultSRV = nullptr;
	}
}