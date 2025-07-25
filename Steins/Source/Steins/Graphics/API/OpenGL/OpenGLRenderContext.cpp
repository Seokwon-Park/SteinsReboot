#include "SteinsPCH.h"
#include "OpenGLRenderContext.h"

#include "glad/glad.h"

namespace Steins
{
	void OpenGLGraphicsContext::Init()
	{
	}
	void OpenGLGraphicsContext::SetViewport(UInt32 _x, UInt32 _y, UInt32 _width, UInt32 _height)
	{
		//glViewport(_x, _y, _width, _height);
	}
	void OpenGLGraphicsContext::SetClearColor(const Color& _color)
	{
		glClearColor(_color.r, _color.g, _color.b, _color.a);
	}
	void OpenGLGraphicsContext::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void OpenGLGraphicsContext::DrawIndexed(UInt32 _indexCount, UInt32 _startIndex, UInt32 _baseVertex)
	{
		glDrawElementsBaseVertex(GL_TRIANGLES, _indexCount, GL_UNSIGNED_INT, (void*)(_startIndex * sizeof(uint32_t)), _baseVertex);
	}
}