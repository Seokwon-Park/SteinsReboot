#pragma once

#include "Daydream/Enum/RendererEnums.h"
#include "Daydream/Graphics/Utility/GraphicsUtility.h"

namespace Daydream::GraphicsUtility::DirectX
{
	DXGI_FORMAT ConvertToDXGIFormat(ShaderDataType type);
	DXGI_FORMAT ConvertToDXGIFormat(RenderFormat _format);
	DXGI_FORMAT ConvertToDSVFormat(RenderFormat _format);
	DXGI_FORMAT ConvertToDepthSRVFormat(RenderFormat _format);
	DXGI_FORMAT ConvertToStencilSRVFormat(RenderFormat _format);
}