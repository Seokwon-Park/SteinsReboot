#include "DaydreamPCH.h"
#include "OpenGLTextureView.h"

#include "glad/glad.h"
#include "OpenGLUtility.h"

namespace Daydream
{
	OpenGLTextureView::OpenGLTextureView(OpenGLGPUTexture* _texture, const TextureViewDesc& _desc)
		:TextureView(_texture, _desc)
	{
		glGenTextures(1, &textureViewID);

		GLenum target = GraphicsUtility::OpenGL::ConvertToOpenGLTextureTarget(_texture->GetDesc().type);
		GLenum internalFormat = GraphicsUtility::OpenGL::ConvertRenderFormatToGLFormat(_texture->GetDesc().format);

		if (_texture->GetDesc().type == TextureType::TextureCube && desc.layerCount == 1)
		{
			target = GL_TEXTURE_2D;
		}

		glTextureView(textureViewID, target, _texture->GetTextureID(), internalFormat, _desc.baseMip, _desc.mipLevels, _desc.baseLayer, _desc.layerCount);

		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
		{
			DAYDREAM_CORE_ASSERT(false, "Failed To Create TextureView : {0x%x}!", err);
		}
	}
}

