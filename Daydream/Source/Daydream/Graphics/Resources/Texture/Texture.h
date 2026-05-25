#pragma once

#include "Daydream/Enum/RendererEnums.h"
#include "Daydream/Graphics/Resources/Sampler.h"
#include "Daydream/Graphics/Resources/GPUResource.h"
#include "Daydream/Asset/Asset.h"

namespace Daydream
{
	class TextureView;

	struct Texture2DDesc
	{
		UInt32 width = 1;
		UInt32 height = 1;
		UInt32 mipLevels = 1;
		UInt32 sampleCount = 1;
		RenderFormat format;
		TextureUsage textureUsage;
	};


	struct TextureDesc
	{
		UInt32 width = 1;
		UInt32 height = 1;
		UInt32 layerCount = 1;
		UInt32 mipLevels = 1;
		UInt32 sampleCount = 1;
		RenderFormat format;
		TextureUsage textureUsage;
		TextureType type = TextureType::Unknown;
	};

	class GPUTexture : public GPUResource
	{
	public:
		GPUTexture(const TextureDesc& _desc);
		virtual ~GPUTexture() = default;

		inline RenderFormat GetFormat() const { return desc.format; }
		inline UInt32 GetWidth() const { return desc.width; }
		inline UInt32 GetHeight() const { return desc.height; }
		inline UInt32 GetMipLevels() const { return desc.mipLevels; }
		inline UInt32 GetLayerCount() const { return desc.layerCount; }

		const TextureDesc& GetDesc() const { return desc; }
		inline TextureType GetType() const { return desc.type; }
	protected:
		TextureDesc desc;
	};

	class Texture : public Asset
	{
	public:
		Texture(Shared<GPUTexture> _texture);
		virtual ~Texture();

		inline RenderFormat GetFormat() const { return gpuTexture->GetDesc().format; }
		inline UInt32 GetWidth() const { return gpuTexture->GetDesc().width; }
		inline UInt32 GetHeight() const { return gpuTexture->GetDesc().height; }
		inline UInt32 GetMipLevels() const { return gpuTexture->GetDesc().mipLevels; }
		inline UInt32 GetLayerCount() const { return gpuTexture->GetDesc().layerCount; }

		inline GPUTexture* GetGPUTexture() const { return gpuTexture.get(); }
		TextureView* GetOrCreateDefaultSRV();

		inline TextureDesc GetDesc() const { return gpuTexture->GetDesc(); }
		inline TextureType GetType() const { return gpuTexture->GetDesc().type; }
	protected:
		Shared<GPUTexture> gpuTexture = nullptr;
		Shared<TextureView> defaultSRV;
	};




}