#pragma once

#include "Daydream/Graphics/API/OpenGL/OpenGLTexture.h"
#include "Daydream/Graphics/Resources/Texture/TextureView.h"

namespace Daydream
{
	class OpenGLTextureView : public TextureView
	{
	public:
		OpenGLTextureView(OpenGLGPUTexture* _texture, const TextureViewDesc& _desc);
		~OpenGLTextureView() override = default;

		virtual void* GetUIHandle() const override { return reinterpret_cast<void*>(static_cast<UInt64>(textureViewID)); }

		UInt32 GetTextureViewID() const { return textureViewID; }

	private:
		UInt32 textureViewID = 0;
	};
}