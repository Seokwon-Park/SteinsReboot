#pragma once

#include "glad/glad.h"
#include "Daydream/Graphics/Utility/GraphicsUtility.h"

#include "Daydream/Enum/RendererEnums.h"
#include "Daydream/Graphics/Resources/Buffer.h"
#include "Daydream/Graphics/Resources/Sampler.h"
#include "Daydream/Graphics/States/DepthStencilState.h"


namespace Daydream::GraphicsUtility::OpenGL
{
	GLenum ConvertToOpenGLMemoryUsage(MemoryUsage _usage);

	GLenum ConvertToOpenGLTextureTarget(TextureType _type);

	GLenum ConvertToShaderStageBit(ShaderType _type);
	GLenum ConvertToGLShaderType(ShaderType _type);

	UInt32 ConvertRenderFormatToGLFormat(RenderFormat _format);
	UInt32 ConvertRenderFormatToGLDataFormat(RenderFormat _format);
	UInt32 ConvertRenderFormatToGLDataType(RenderFormat _format);

	GLint ConvertToGLMinFilter(FilterMode _minFilter, FilterMode _mipFilter);
	GLint ConvertToGLMagFilter(FilterMode _filterMode);
	GLint ConvertToGLWrapMode(WrapMode _wrapMode);

	GLenum ConvertToGLCompareFunc(const CompareFunction& _compareFunc);
	GLenum ConvertToGLStencilOp(const StencilOperation& _stencilOp);
}