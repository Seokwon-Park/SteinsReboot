#include "DaydreamPCH.h"
#include "D3D12Utility.h"

namespace Daydream::GraphicsUtility::DirectX12
{
	UInt32 CalcSubresource(UInt32 _mipSlice, UInt32 _arraySlice, UInt32 _planeSlice, UInt32 _mipLevels, UInt32 _arraySize)
	{
		return _mipSlice + (_arraySlice * _mipLevels) + (_planeSlice * _mipLevels * _arraySize);
	}
	D3D12_RESOURCE_STATES ConvertToD3D12ResourceStates(const ResourceState& _state)
	{
		switch (_state)
		{
		case ResourceState::Undefined:
			return D3D12_RESOURCE_STATE_COMMON;

		case ResourceState::CopyDest:
			return D3D12_RESOURCE_STATE_COPY_DEST;

		case ResourceState::CopySource:
			return D3D12_RESOURCE_STATE_COPY_SOURCE;

		case ResourceState::VertexBuffer:
		case ResourceState::ConstantBuffer:
			return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

		case ResourceState::IndexBuffer:
			return D3D12_RESOURCE_STATE_INDEX_BUFFER;

		case ResourceState::RenderTarget:
			return D3D12_RESOURCE_STATE_RENDER_TARGET;

		case ResourceState::DepthWrite:
			return D3D12_RESOURCE_STATE_DEPTH_WRITE;

		case ResourceState::DepthRead:
			return D3D12_RESOURCE_STATE_DEPTH_READ;

		case ResourceState::ShaderResource:
			return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;

		case ResourceState::UnorderedAccess:
			return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;

		case ResourceState::Present:
			return D3D12_RESOURCE_STATE_PRESENT;

		default:
			DAYDREAM_CORE_ASSERT(false, "Unknown Resource State!");
			return D3D12_RESOURCE_STATE_COMMON;
		}
	}
	D3D12MA::ALLOCATION_DESC ConvertToD3D12MemoryAllocationDesc(const BufferDesc& _desc)
	{
		D3D12MA::ALLOCATION_DESC allocationDesc{};

		switch (_desc.memoryUsage)
		{
		case MemoryUsage::Static:	allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT; break;
		case MemoryUsage::Dynamic:	allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;  break;
		case MemoryUsage::Readback:	allocationDesc.HeapType = D3D12_HEAP_TYPE_READBACK; break;
		case MemoryUsage::Upload:	allocationDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD; break;
		default:					allocationDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT; break;
		}

		return allocationDesc;
	}
	D3D12_HEAP_PROPERTIES ConvertToD3D12HeapProperties(const BufferDesc& _desc)
	{
		D3D12_HEAP_PROPERTIES heapProperties{};

		switch (_desc.memoryUsage)
		{
		case MemoryUsage::Static:	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; break;
		case MemoryUsage::Dynamic:	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;  break;
		case MemoryUsage::Readback:	heapProperties.Type = D3D12_HEAP_TYPE_READBACK; break;
		case MemoryUsage::Upload:	heapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; break;
		default:					heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; break;
		}

		heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		return heapProperties;
	}
	D3D12_RESOURCE_DESC ConvertToD3D12ResourceDesc(const BufferDesc& _desc)
	{
		D3D12_RESOURCE_DESC resourceDesc{};
		resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resourceDesc.Alignment = 0;
		resourceDesc.Width = (Bool)(_desc.bufferUsage & BufferUsage::Constant) ? ((_desc.size + 255) & ~255) : _desc.size;
		resourceDesc.Height = 1;
		resourceDesc.DepthOrArraySize = 1;
		resourceDesc.MipLevels = 1;
		resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
		resourceDesc.SampleDesc.Count = 1;
		resourceDesc.SampleDesc.Quality = 0;
		resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		resourceDesc.Flags = ConvertToD3D12ResourceFlags(_desc.bufferUsage);

		return resourceDesc;
	}

	D3D12_RESOURCE_FLAGS ConvertToD3D12ResourceFlags(BufferUsage _bufferUsage)
	{
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

		if (HasFlag(_bufferUsage, BufferUsage::Storage))
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		return flags;
	}

	D3D12_RESOURCE_STATES ConvertToD3D12InitialState(MemoryUsage _memoryUsage)
	{
		switch (_memoryUsage)
		{
		case MemoryUsage::Static: return D3D12_RESOURCE_STATE_COMMON;
		case MemoryUsage::Dynamic: return D3D12_RESOURCE_STATE_GENERIC_READ;
		case MemoryUsage::Readback: return D3D12_RESOURCE_STATE_COPY_DEST;
		case MemoryUsage::Upload: return D3D12_RESOURCE_STATE_GENERIC_READ;
		default: return D3D12_RESOURCE_STATE_COMMON;
		}
	}

	D3D12_SHADER_VISIBILITY GetD3D12ShaderVisibility(ShaderType _type)
	{
		switch (_type)
		{
		case ShaderType::Vertex:
			return D3D12_SHADER_VISIBILITY_VERTEX;
			break;
		case ShaderType::Hull:
			return D3D12_SHADER_VISIBILITY_HULL;
			break;
		case ShaderType::Domain:
			return D3D12_SHADER_VISIBILITY_DOMAIN;
			break;
		case ShaderType::Geometry:
			return D3D12_SHADER_VISIBILITY_GEOMETRY;
			break;
		case ShaderType::Pixel:
			return D3D12_SHADER_VISIBILITY_PIXEL;
			break;
		};
		return D3D12_SHADER_VISIBILITY_ALL;
	}


	D3D12_RESOURCE_FLAGS ConvertToD3D12BindFlags(TextureUsage _flags)
	{
		D3D12_RESOURCE_FLAGS d3d12Flags = D3D12_RESOURCE_FLAG_NONE;

		if (HasFlag(_flags, TextureUsage::RenderTarget)) d3d12Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		if (HasFlag(_flags, TextureUsage::DepthStencil)) d3d12Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		if (HasFlag(_flags, TextureUsage::Storage)) d3d12Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		return d3d12Flags;
	}

	D3D12_FILTER ConvertToD3D12Filter(FilterMode _minFilterMode, FilterMode _magFilterMode, FilterMode _mipFilterMode)
	{
		if (_minFilterMode == FilterMode::Nearest && _magFilterMode == FilterMode::Nearest && _mipFilterMode == FilterMode::Nearest)
			return D3D12_FILTER_MIN_MAG_MIP_POINT;

		if (_minFilterMode == FilterMode::Nearest && _magFilterMode == FilterMode::Nearest && _mipFilterMode == FilterMode::Linear)
			return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;

		if (_minFilterMode == FilterMode::Nearest && _magFilterMode == FilterMode::Linear && _mipFilterMode == FilterMode::Nearest)
			return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;

		if (_minFilterMode == FilterMode::Nearest && _magFilterMode == FilterMode::Linear && _mipFilterMode == FilterMode::Linear)
			return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;

		if (_minFilterMode == FilterMode::Linear && _magFilterMode == FilterMode::Nearest && _mipFilterMode == FilterMode::Nearest)
			return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;

		if (_minFilterMode == FilterMode::Linear && _magFilterMode == FilterMode::Nearest && _mipFilterMode == FilterMode::Linear)
			return D3D12_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;

		if (_minFilterMode == FilterMode::Linear && _magFilterMode == FilterMode::Linear && _mipFilterMode == FilterMode::Nearest)
			return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;

		if (_minFilterMode == FilterMode::Linear && _magFilterMode == FilterMode::Linear && _mipFilterMode == FilterMode::Linear)
			return D3D12_FILTER_MIN_MAG_MIP_LINEAR;

		return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	}

	D3D12_TEXTURE_ADDRESS_MODE ConvertToD3D12WrapMode(WrapMode _wrapMode)
	{
		switch (_wrapMode)
		{
		case WrapMode::Repeat:
			return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		case WrapMode::ClampToEdge:
			return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case WrapMode::ClampToBorder:
			return D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		case WrapMode::MirrorRepeat:
			return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
		default:
			break;
		}
		return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	}

	D3D12_COMPARISON_FUNC ConvertToD3D12ComparisonFunc(ComparisonFunc _func)
	{
		switch (_func)
		{
		case ComparisonFunc::Never:
			return D3D12_COMPARISON_FUNC_NEVER;
		case ComparisonFunc::Less:
			return D3D12_COMPARISON_FUNC_LESS;
		case ComparisonFunc::Equal:
			return D3D12_COMPARISON_FUNC_EQUAL;
		case ComparisonFunc::LessEqual:
			return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case ComparisonFunc::Greater:
			return D3D12_COMPARISON_FUNC_GREATER;
		case ComparisonFunc::NotEqual:
			return D3D12_COMPARISON_FUNC_NOT_EQUAL;
		case ComparisonFunc::GreaterEqual:
			return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case ComparisonFunc::Always:
			return D3D12_COMPARISON_FUNC_ALWAYS;
		default:
			break;
		}
		return D3D12_COMPARISON_FUNC_NEVER;
	}


	constexpr D3D12_CULL_MODE ConvertToD3D12CullMode(const CullMode& _cullMode)
	{
		switch (_cullMode)
		{
		case CullMode::None:
			return D3D12_CULL_MODE_NONE;
		case CullMode::Front:
			return D3D12_CULL_MODE_FRONT;
		case CullMode::Back:
			return D3D12_CULL_MODE_BACK;
		default:
			return D3D12_CULL_MODE_NONE;
		}
		return D3D12_CULL_MODE_NONE;
	}
	constexpr D3D12_FILL_MODE ConvertToD3D12FillMode(const FillMode& _fillMode)
	{
		switch (_fillMode)
		{
		case FillMode::Solid:
			return D3D12_FILL_MODE_SOLID;
		case FillMode::Wireframe:
			return D3D12_FILL_MODE_WIREFRAME;
		default:
			return D3D12_FILL_MODE_SOLID;
		}
		return D3D12_FILL_MODE_SOLID;
	}

	D3D12_COMPARISON_FUNC ConvertToD3D12ComparisonFunc(const CompareFunction& _compareFunc)
	{
		switch (_compareFunc)
		{
		case CompareFunction::Never:
			return D3D12_COMPARISON_FUNC_NEVER;
		case CompareFunction::Less:
			return D3D12_COMPARISON_FUNC_LESS;
		case CompareFunction::Equal:
			return D3D12_COMPARISON_FUNC_EQUAL;
		case CompareFunction::LessEqual:
			return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case CompareFunction::Greater:
			return D3D12_COMPARISON_FUNC_GREATER;
		case CompareFunction::NotEqual:
			return D3D12_COMPARISON_FUNC_NOT_EQUAL;
		case CompareFunction::GreaterEqual:
			return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case CompareFunction::Always:
			return D3D12_COMPARISON_FUNC_ALWAYS;
		default:
			return D3D12_COMPARISON_FUNC_LESS;
		}
	}

	D3D12_STENCIL_OP ConvertToD3D12StencilOperation(const StencilOperation& _stencilOp)
	{
		switch (_stencilOp)
		{
		case StencilOperation::Keep:
			return D3D12_STENCIL_OP_KEEP;
		case StencilOperation::Zero:
			return D3D12_STENCIL_OP_ZERO;
		case StencilOperation::Replace:
			return D3D12_STENCIL_OP_REPLACE;
		case StencilOperation::IncrementSaturate:
			return D3D12_STENCIL_OP_INCR_SAT;
		case StencilOperation::DecrementSaturate:
			return D3D12_STENCIL_OP_DECR_SAT;
		case StencilOperation::Invert:
			return D3D12_STENCIL_OP_INVERT;
		case StencilOperation::IncrementWrap:
			return D3D12_STENCIL_OP_INCR;
		case StencilOperation::DecrementWrap:
			return D3D12_STENCIL_OP_DECR;
		default:
			return D3D12_STENCIL_OP_KEEP;
		}
	}

	D3D12_DEPTH_STENCILOP_DESC ConvertToD3D12StencilOperationDesc(const StencilOperationDesc& _opDesc)
	{
		D3D12_DEPTH_STENCILOP_DESC desc;
		desc.StencilFailOp = ConvertToD3D12StencilOperation(_opDesc.failOp);
		desc.StencilDepthFailOp = ConvertToD3D12StencilOperation(_opDesc.depthFailOp);
		desc.StencilPassOp = ConvertToD3D12StencilOperation(_opDesc.passOp);
		desc.StencilFunc = ConvertToD3D12ComparisonFunc(_opDesc.compareFunc);

		return desc;
	}

	D3D12_SAMPLER_DESC ConvertToD3D12SamplerDesc(const SamplerDesc& _desc)
	{
		//D3D12_SAMPLER_DESC samplerDesc;
		//ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		//samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		//samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_WRAP;
		//samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_WRAP;
		//samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_WRAP;
		//samplerDesc.ComparisonFunc = D3D12_COMPARISON_NEVER;
		//samplerDesc.MinLOD = 0;
		//samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;

		D3D12_SAMPLER_DESC desc{};
		desc.Filter = ConvertToD3D12Filter(_desc.minFilter, _desc.magFilter, _desc.mipFilter);
		desc.AddressU = ConvertToD3D12WrapMode(_desc.wrapU);
		desc.AddressV = ConvertToD3D12WrapMode(_desc.wrapV);
		desc.AddressW = ConvertToD3D12WrapMode(_desc.wrapW);
		desc.MipLODBias = _desc.lodBias;
		desc.MaxAnisotropy = _desc.maxAnisotropy;
		desc.ComparisonFunc = ConvertToD3D12ComparisonFunc(_desc.comparisonFunc);
		for (int i = 0; i < 4; i++)
		{
			desc.BorderColor[i] = _desc.borderColor[i];
		}
		desc.MinLOD = _desc.minLod;
		desc.MaxLOD = _desc.maxLod;
		return desc;
	}

	D3D12_RASTERIZER_DESC ConvertToD3D12RasterizerDesc(const RasterizerStateDesc& _desc)
	{
		D3D12_RASTERIZER_DESC desc{};
		desc.FillMode = ConvertToD3D12FillMode(_desc.fillMode);
		desc.CullMode = ConvertToD3D12CullMode(_desc.cullMode);
		desc.FrontCounterClockwise = _desc.frontCounterClockwise;
		desc.DepthBias = _desc.depthBias;
		desc.DepthBiasClamp = _desc.depthBiasClamp;
		desc.SlopeScaledDepthBias = _desc.slopeScaledDepthBias;
		desc.DepthClipEnable = _desc.depthClipEnable;
		desc.MultisampleEnable = _desc.multisampleEnable;
		desc.AntialiasedLineEnable = _desc.antialiasedLineEnable;
		return desc;
	}
	D3D12_DEPTH_STENCIL_DESC ConvertToD3D12DepthStencilDesc(const DepthStencilStateDesc& _desc)
	{
		D3D12_DEPTH_STENCIL_DESC desc;
		desc.DepthEnable = _desc.depthEnable;
		desc.DepthWriteMask = _desc.depthWriteEnable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = ConvertToD3D12ComparisonFunc(_desc.depthFunc);
		desc.StencilEnable = _desc.stencilEnable;
		desc.StencilReadMask = _desc.stencilReadMask;
		desc.StencilWriteMask = _desc.stencilWriteMask;
		desc.FrontFace = ConvertToD3D12StencilOperationDesc(_desc.frontFace);
		desc.BackFace = ConvertToD3D12StencilOperationDesc(_desc.backFace);

		return desc;
	}

}