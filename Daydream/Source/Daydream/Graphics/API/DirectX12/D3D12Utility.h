#pragma once

#include "Daydream/Enum/RendererEnums.h"
#include "Daydream/Graphics/API/DirectX/D3DUtility.h"
#include "Daydream/Graphics/Utility/GraphicsUtility.h"

#include "Daydream/Graphics/Resources/Buffer.h"
#include "Daydream/Graphics/Resources/Sampler.h"
#include "Daydream/Graphics/States/RasterizerState.h"
#include "D3D12MemAlloc.h"

namespace Daydream::GraphicsUtility::DirectX12
{
	inline UInt32 CalcSubresource(UInt32 MipSlice, UInt32 ArraySlice, UInt32 PlaneSlice, UInt32 MipLevels, UInt32 ArraySize)
	{
		return MipSlice + (ArraySlice * MipLevels) + (PlaneSlice * MipLevels * ArraySize);
	}

	D3D12_RESOURCE_STATES ConvertToD3D12ResourceStates(const ResourceState& _state);

	D3D12MA::ALLOCATION_DESC  ConvertToD3D12MemoryAllocationDesc(const BufferDesc& _desc);
	D3D12_HEAP_PROPERTIES ConvertToD3D12HeapProperties(const BufferDesc& _desc);
	D3D12_RESOURCE_DESC ConvertToD3D12ResourceDesc(const BufferDesc& _desc);
	D3D12_RESOURCE_FLAGS ConvertToD3D12ResourceFlags(BufferUsage _bufferUsage);
	D3D12_RESOURCE_STATES ConvertToD3D12InitialState(MemoryUsage _memoryUsage);

	D3D12_SHADER_VISIBILITY GetD3D12ShaderVisibility(ShaderType _type);
	D3D12_RESOURCE_FLAGS ConvertToD3D12BindFlags(TextureUsage _flags);

	D3D12_TEXTURE_ADDRESS_MODE ConvertToD3D12WrapMode(WrapMode _wrapMode);
	D3D12_FILTER ConvertToD3D12Filter(FilterMode _minFilter, FilterMode _magFilter, FilterMode _mipFilter);
	D3D12_COMPARISON_FUNC ConvertToD3D12ComparisonFunc(ComparisonFunc _func);

	constexpr D3D12_CULL_MODE ConvertToD3D12CullMode(const CullMode& _cullMode);
	constexpr D3D12_FILL_MODE ConvertToD3D12FillMode(const FillMode& _fillMode);

	D3D12_SAMPLER_DESC ConvertToD3D12SamplerDesc(const SamplerDesc& _desc);
	D3D12_RASTERIZER_DESC ConvertToD3D12RasterizerDesc(const RasterizerStateDesc& _desc);
}