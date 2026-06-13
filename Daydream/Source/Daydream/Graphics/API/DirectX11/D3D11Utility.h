#pragma once

#include "Daydream/Enum/RendererEnums.h"
#include "Daydream/Graphics/API/DirectX/D3DUtility.h"
#include "Daydream/Graphics/Utility/GraphicsUtility.h"

#include "Daydream/Graphics/States/RasterizerState.h"
#include "Daydream/Graphics/States/DepthStencilState.h"
#include "Daydream/Graphics/Resources/Buffer.h"
#include "Daydream/Graphics/Resources/Sampler.h"

namespace Daydream::GraphicsUtility::DirectX11
{
	D3D11_BUFFER_DESC ConvertToD3D11BufferDesc(const BufferDesc& _desc);
	D3D11_USAGE ConvertToD3D11Usage(const MemoryUsage& _usage);
	UInt32 ConvertToD3D11CPUAccessFlags(const MemoryUsage& _usage);

	UInt32 ConvertToD3D11BufferBindFlags(const BufferUsage& _usage);
	UInt32 ConvertToD3D11TextureBindFlags(const TextureUsage& _flags);

	D3D11_TEXTURE_ADDRESS_MODE ConvertToD3D11WrapMode(WrapMode _wrapMode);
	D3D11_FILTER ConvertToD3D11Filter(FilterMode _minFilter, FilterMode _magFilter, FilterMode _mipFilter);
	D3D11_COMPARISON_FUNC ConvertToD3D11ComparisonFunc(ComparisonFunc _func);

	D3D11_CULL_MODE ConvertToD3D11CullMode(const CullMode& _cullMode);
	D3D11_FILL_MODE ConvertToD3D11FillMode(const FillMode& _fillMode);

	D3D11_COMPARISON_FUNC ConvertToD3D11ComparisonFunc(const CompareFunction& _compareFunc);
	D3D11_STENCIL_OP ConvertToD3D11StencilOperation(const StencilOperation& _stencilOp);
	D3D11_DEPTH_STENCILOP_DESC ConvertToD3D11StencilOperationDesc(const StencilOperationDesc& _opDesc);

	D3D11_SAMPLER_DESC ConvertToD3D11SamplerDesc(const SamplerDesc& _desc);
	D3D11_RASTERIZER_DESC ConvertToD3D11RasterizerDesc(const RasterizerStateDesc& _desc);
	D3D11_DEPTH_STENCIL_DESC ConvertToD3D11DepthStencilDesc(const DepthStencilStateDesc& _desc);

	
}