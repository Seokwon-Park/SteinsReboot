#pragma once

#include "OpenGLSampler.h"
#include "Daydream/Graphics/Resources/Texture/Texture.h"

namespace Daydream
{
	class OpenGLGPUTexture : public GPUTexture
	{
	public:
		OpenGLGPUTexture(const TextureDesc& _desc);
		virtual ~OpenGLGPUTexture();

		inline UInt32 GetTextureID() const { return textureID; }
	private:
		UInt32 textureID;
	};

//	class OpenGLTexture2D : public Texture2D
//	{
//	public:
//		OpenGLTexture2D(const TextureDesc& _desc, const void* _initialData = nullptr);
//		virtual ~OpenGLTexture2D() override;
//
//		virtual void SetSampler(Shared<Sampler> _sampler) override;
//		virtual bool HasSampler() override { return textureSampler != nullptr; }
//
//		virtual void* GetImGuiHandle() { return  reinterpret_cast<void*>(static_cast<UInt64>(textureID)); }
//
//		UInt32 GetTextureID() const { return textureID; }
//		UInt32 GetSamplerID() const { return textureSampler->GetSamplerID(); }
//
//	private:
//		UInt32 textureID;
//		OpenGLSampler* textureSampler = nullptr;
//	};
}