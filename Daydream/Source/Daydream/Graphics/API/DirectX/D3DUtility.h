#pragma once

#include "Daydream/Enum/RendererEnums.h"

namespace Daydream::GraphicsUtility::DirectX
{
	DXGI_FORMAT ConvertToDXGIFormat(ShaderDataType type);
	DXGI_FORMAT ConvertToDXGIFormat(RenderFormat _format);
}