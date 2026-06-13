#include "DaydreamPCH.h"
#include "D3D11Utility.h"

namespace Daydream::GraphicsUtility::DirectX11
{
	D3D11_BUFFER_DESC ConvertToD3D11BufferDesc(const BufferDesc& _desc)
	{
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));

		bufferDesc.ByteWidth = (UInt32)_desc.size;

		bufferDesc.Usage = ConvertToD3D11Usage(_desc.memoryUsage);
		bufferDesc.CPUAccessFlags = ConvertToD3D11CPUAccessFlags(_desc.memoryUsage);
		
		if (_desc.memoryUsage == MemoryUsage::Readback || _desc.memoryUsage == MemoryUsage::Upload)
		{
			bufferDesc.BindFlags = 0;
		}
		else
		{
			bufferDesc.BindFlags = ConvertToD3D11BufferBindFlags(_desc.bufferUsage);
		}

		bufferDesc.MiscFlags = 0;
		if (HasFlag(_desc.bufferUsage, BufferUsage::Indirect))	bufferDesc.MiscFlags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
		if (HasFlag(_desc.bufferUsage, BufferUsage::Storage))	bufferDesc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

		bufferDesc.StructureByteStride = 0;

		return bufferDesc;
	}

	D3D11_USAGE ConvertToD3D11Usage(const MemoryUsage& _usage)
	{
		switch (_usage)
		{
		case MemoryUsage::Static:
			return D3D11_USAGE_DEFAULT;
		case MemoryUsage::Dynamic:
			return D3D11_USAGE_DYNAMIC;
		case MemoryUsage::Readback:
		case MemoryUsage::Upload:
			return D3D11_USAGE_STAGING;
		default:
			return D3D11_USAGE_DEFAULT;
		}
	}

	UInt32 ConvertToD3D11CPUAccessFlags(const MemoryUsage& _usage)
	{
		UInt32 d3d11Flags = 0;


		switch (_usage)
		{
		case MemoryUsage::Static:	break;
		case MemoryUsage::Dynamic:	d3d11Flags |= D3D11_CPU_ACCESS_WRITE; break;
		case MemoryUsage::Readback:	d3d11Flags |= D3D11_CPU_ACCESS_READ; break;
		case MemoryUsage::Upload:
		{
			d3d11Flags |= D3D11_CPU_ACCESS_WRITE;
			d3d11Flags |= D3D11_CPU_ACCESS_READ;
			break;
		}
		default:
			break;
		}
		return d3d11Flags;
	}

	UInt32 ConvertToD3D11BufferBindFlags(const BufferUsage& _usage)
	{
		UInt32 d3d11Flags = 0;

		if (HasFlag(_usage, BufferUsage::Vertex)) d3d11Flags |= D3D11_BIND_VERTEX_BUFFER;
		if (HasFlag(_usage, BufferUsage::Index)) d3d11Flags |= D3D11_BIND_INDEX_BUFFER;
		if (HasFlag(_usage, BufferUsage::Constant)) d3d11Flags |= D3D11_BIND_CONSTANT_BUFFER;
		if (HasFlag(_usage, BufferUsage::Storage)) d3d11Flags |= D3D11_BIND_UNORDERED_ACCESS;
		
		return d3d11Flags;
	}

	UInt32 ConvertToD3D11TextureBindFlags(const TextureUsage& _flags)
	{
		UInt32 d3d11Flags = 0;

		if (HasFlag(_flags, TextureUsage::ShaderResource)) d3d11Flags |= D3D11_BIND_SHADER_RESOURCE;
		if (HasFlag(_flags, TextureUsage::RenderTarget)) d3d11Flags |= D3D11_BIND_RENDER_TARGET;
		if (HasFlag(_flags, TextureUsage::DepthStencil)) d3d11Flags |= D3D11_BIND_DEPTH_STENCIL;
		if (HasFlag(_flags, TextureUsage::Storage)) d3d11Flags |= D3D11_BIND_UNORDERED_ACCESS;

		return d3d11Flags;
	}
	D3D11_FILTER ConvertToD3D11Filter(FilterMode _minFilter, FilterMode _magFilter, FilterMode _mipFilter)
	{
		if (_minFilter == FilterMode::Nearest && _magFilter == FilterMode::Nearest && _mipFilter == FilterMode::Nearest)
			return D3D11_FILTER_MIN_MAG_MIP_POINT;

		if (_minFilter == FilterMode::Nearest && _magFilter == FilterMode::Nearest && _mipFilter == FilterMode::Linear)
			return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;

		if (_minFilter == FilterMode::Nearest && _magFilter == FilterMode::Linear && _mipFilter == FilterMode::Nearest)
			return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;

		if (_minFilter == FilterMode::Nearest && _magFilter == FilterMode::Linear && _mipFilter == FilterMode::Linear)
			return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;

		if (_minFilter == FilterMode::Linear && _magFilter == FilterMode::Nearest && _mipFilter == FilterMode::Nearest)
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;

		if (_minFilter == FilterMode::Linear && _magFilter == FilterMode::Nearest && _mipFilter == FilterMode::Linear)
			return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;

		if (_minFilter == FilterMode::Linear && _magFilter == FilterMode::Linear && _mipFilter == FilterMode::Nearest)
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;

		if (_minFilter == FilterMode::Linear && _magFilter == FilterMode::Linear && _mipFilter == FilterMode::Linear)
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;

		return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	}

	D3D11_TEXTURE_ADDRESS_MODE ConvertToD3D11WrapMode(WrapMode _wrapMode)
	{
		switch (_wrapMode)
		{
		case WrapMode::Repeat:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		case WrapMode::ClampToEdge:
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		case WrapMode::ClampToBorder:
			return D3D11_TEXTURE_ADDRESS_BORDER;
		case WrapMode::MirrorRepeat:
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		default:
			break;
		}
		return D3D11_TEXTURE_ADDRESS_WRAP;
	}

	D3D11_COMPARISON_FUNC ConvertToD3D11ComparisonFunc(ComparisonFunc _func)
	{
		switch (_func)
		{
		case ComparisonFunc::Never:
			return D3D11_COMPARISON_NEVER;
		case ComparisonFunc::Less:
			return D3D11_COMPARISON_LESS;
		case ComparisonFunc::Equal:
			return D3D11_COMPARISON_EQUAL;
		case ComparisonFunc::LessEqual:
			return D3D11_COMPARISON_LESS_EQUAL;
		case ComparisonFunc::Greater:
			return D3D11_COMPARISON_GREATER;
		case ComparisonFunc::NotEqual:
			return D3D11_COMPARISON_NOT_EQUAL;
		case ComparisonFunc::GreaterEqual:
			return D3D11_COMPARISON_GREATER_EQUAL;
		case ComparisonFunc::Always:
			return D3D11_COMPARISON_ALWAYS;
		default:
			break;
		}
		return D3D11_COMPARISON_NEVER;
	}

	D3D11_SAMPLER_DESC ConvertToD3D11SamplerDesc(const SamplerDesc& _desc)
	{
		//D3D11_SAMPLER_DESC samplerDesc;
		//ZeroMemory(&samplerDesc, sizeof(samplerDesc));
		//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		//samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		//samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		//samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		//samplerDesc.MinLOD = 0;
		//samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		D3D11_SAMPLER_DESC desc{};
		desc.Filter = ConvertToD3D11Filter(_desc.minFilter, _desc.magFilter, _desc.mipFilter);
		desc.AddressU = ConvertToD3D11WrapMode(_desc.wrapU);
		desc.AddressV = ConvertToD3D11WrapMode(_desc.wrapV);
		desc.AddressW = ConvertToD3D11WrapMode(_desc.wrapW);
		desc.MipLODBias = _desc.lodBias;
		desc.MaxAnisotropy = _desc.maxAnisotropy;
		desc.ComparisonFunc = ConvertToD3D11ComparisonFunc(_desc.comparisonFunc);
		for (int i = 0; i < 4; i++)
		{
			desc.BorderColor[i] = _desc.borderColor[i];
		}
		desc.MinLOD = _desc.minLod;
		desc.MaxLOD = _desc.maxLod;
		return desc;
	}

	D3D11_CULL_MODE ConvertToD3D11CullMode(const CullMode& _cullMode)
	{
		switch (_cullMode)
		{
		case CullMode::None:
			return D3D11_CULL_NONE;
		case CullMode::Front:
			return D3D11_CULL_FRONT;
		case CullMode::Back:
			return D3D11_CULL_BACK;
		default:
			return D3D11_CULL_NONE;
		}
		return D3D11_CULL_NONE;
	}

	D3D11_FILL_MODE ConvertToD3D11FillMode(const FillMode& _fillMode)
	{
		switch (_fillMode)
		{
		case FillMode::Solid:
			return D3D11_FILL_SOLID;
		case FillMode::Wireframe:
			return D3D11_FILL_WIREFRAME;
		default:
			return D3D11_FILL_SOLID;
		}
		return D3D11_FILL_SOLID;
	}

	D3D11_COMPARISON_FUNC ConvertToD3D11ComparisonFunc(const CompareFunction& _compareFunc)
	{
		switch (_compareFunc)
		{
		case CompareFunction::Never:
			return D3D11_COMPARISON_NEVER;
		case CompareFunction::Less:
			return D3D11_COMPARISON_LESS;
		case CompareFunction::Equal:
			return D3D11_COMPARISON_EQUAL;
		case CompareFunction::LessEqual:
			return D3D11_COMPARISON_LESS_EQUAL;
		case CompareFunction::Greater:
			return D3D11_COMPARISON_GREATER;
		case CompareFunction::NotEqual:
			return D3D11_COMPARISON_NOT_EQUAL;
		case CompareFunction::GreaterEqual:
			return D3D11_COMPARISON_GREATER_EQUAL;
		case CompareFunction::Always:
			return D3D11_COMPARISON_ALWAYS;
		default:
			return D3D11_COMPARISON_LESS;
		}
	}

	D3D11_STENCIL_OP ConvertToD3D11StencilOperation(const StencilOperation& _stencilOp)
	{
		switch (_stencilOp)
		{
		case StencilOperation::Keep:
			return D3D11_STENCIL_OP_KEEP;
		case StencilOperation::Zero:
			return D3D11_STENCIL_OP_ZERO;
		case StencilOperation::Replace:
			return D3D11_STENCIL_OP_REPLACE;
		case StencilOperation::IncrementSaturate:
			return D3D11_STENCIL_OP_INCR_SAT;
		case StencilOperation::DecrementSaturate:
			return D3D11_STENCIL_OP_DECR_SAT;
		case StencilOperation::Invert:
			return D3D11_STENCIL_OP_INVERT;
		case StencilOperation::IncrementWrap:
			return D3D11_STENCIL_OP_INCR;
		case StencilOperation::DecrementWrap:
			return D3D11_STENCIL_OP_DECR;
		default:
			return D3D11_STENCIL_OP_KEEP;
		}
	}

	D3D11_DEPTH_STENCILOP_DESC ConvertToD3D11StencilOperationDesc(const StencilOperationDesc& _opDesc)
	{
		D3D11_DEPTH_STENCILOP_DESC desc;
		desc.StencilFailOp = ConvertToD3D11StencilOperation(_opDesc.failOp);
		desc.StencilDepthFailOp = ConvertToD3D11StencilOperation(_opDesc.depthFailOp);
		desc.StencilPassOp = ConvertToD3D11StencilOperation(_opDesc.passOp);
		desc.StencilFunc = ConvertToD3D11ComparisonFunc(_opDesc.compareFunc);

		return desc;
	}

	D3D11_RASTERIZER_DESC ConvertToD3D11RasterizerDesc(const RasterizerStateDesc& _desc)
	{
		D3D11_RASTERIZER_DESC desc{};
		desc.CullMode = ConvertToD3D11CullMode(_desc.cullMode);
		desc.FillMode = ConvertToD3D11FillMode(_desc.fillMode);
		desc.FrontCounterClockwise = _desc.frontCounterClockwise;
		desc.DepthBias = _desc.depthBias;
		desc.DepthBiasClamp = _desc.depthBiasClamp;
		desc.SlopeScaledDepthBias = _desc.slopeScaledDepthBias;
		desc.DepthClipEnable = _desc.depthClipEnable;
		desc.ScissorEnable = _desc.scissorEnable;
		desc.MultisampleEnable = _desc.multisampleEnable;
		desc.AntialiasedLineEnable = _desc.antialiasedLineEnable;

		return desc;
	}

	D3D11_DEPTH_STENCIL_DESC ConvertToD3D11DepthStencilDesc(const DepthStencilStateDesc& _desc)
	{
		D3D11_DEPTH_STENCIL_DESC desc{};

		desc.DepthEnable = _desc.depthEnable;
		desc.DepthWriteMask = _desc.depthWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = ConvertToD3D11ComparisonFunc(_desc.depthFunc);
		desc.StencilEnable = _desc.stencilEnable;
		desc.StencilReadMask = _desc.stencilReadMask;
		desc.StencilWriteMask = _desc.stencilWriteMask;
		desc.FrontFace = ConvertToD3D11StencilOperationDesc(_desc.frontFace);
		desc.BackFace = ConvertToD3D11StencilOperationDesc(_desc.backFace);

		return desc;
	}

}