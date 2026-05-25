#include "DaydreamPCH.h"
#include "D3D11VertexShader.h"

#include "Daydream/Graphics/Utility/GraphicsUtility.h"

namespace Daydream
{
	namespace
	{
		RenderFormat ConvertToRenderFormat(const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc)
		{
			int componentCount = Math::BitCount(paramDesc.Mask); // 비트 수 세는 함수

			switch (componentCount)
			{
			case 1:
				switch (paramDesc.ComponentType)
				{
				case D3D_REGISTER_COMPONENT_UINT32:  return RenderFormat::R32_UINT;
				case D3D_REGISTER_COMPONENT_SINT32:  return RenderFormat::R32_SINT;
				case D3D_REGISTER_COMPONENT_FLOAT32: return RenderFormat::R32_FLOAT;
				default: return RenderFormat::UNKNOWN;
				}

			case 2:
				switch (paramDesc.ComponentType)
				{
				case D3D_REGISTER_COMPONENT_UINT32:  return RenderFormat::R32G32_UINT;
				case D3D_REGISTER_COMPONENT_SINT32:  return RenderFormat::R32G32_SINT;
				case D3D_REGISTER_COMPONENT_FLOAT32: return RenderFormat::R32G32_FLOAT;
				default: return RenderFormat::UNKNOWN;
				}

			case 3:
				switch (paramDesc.ComponentType)
				{
				case D3D_REGISTER_COMPONENT_UINT32:  return RenderFormat::R32G32B32_UINT;
				case D3D_REGISTER_COMPONENT_SINT32:  return RenderFormat::R32G32B32_SINT;
				case D3D_REGISTER_COMPONENT_FLOAT32: return RenderFormat::R32G32B32_FLOAT;
				default: return RenderFormat::UNKNOWN;
				}

			case 4:
				switch (paramDesc.ComponentType)
				{
				case D3D_REGISTER_COMPONENT_UINT32:  return RenderFormat::R32G32B32A32_UINT;
				case D3D_REGISTER_COMPONENT_SINT32:  return RenderFormat::R32G32B32A32_SINT;
				case D3D_REGISTER_COMPONENT_FLOAT32: return RenderFormat::R32G32B32A32_FLOAT;
				default: return RenderFormat::UNKNOWN;
				}

			default:
				return RenderFormat::UNKNOWN;
			}
		}
	}

	D3D11VertexShader::D3D11VertexShader(D3D11RenderDevice* _device, const String& _src, const ShaderLoadMode& _mode)
		:D3D11Shader(_device, _src, ShaderType::Vertex, _mode)
	{
		device->GetDevice()->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, vertexShader.GetAddressOf());

		// 셰이더 입력 시그니처 정보 얻기
		D3D11_SHADER_DESC shaderDesc;
		reflection->GetDesc(&shaderDesc);

		// 각 입력 파라미터에 대해 순회
		for (UINT i = 0; i < shaderDesc.InputParameters; i++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
			reflection->GetInputParameterDesc(i, &paramDesc);

			ShaderReflectionData sr{};
			sr.name = paramDesc.SemanticName;
			sr.set = 0;
			sr.binding = paramDesc.SemanticIndex;
			sr.shaderResourceType = ShaderResourceType::Input;
			sr.format = ConvertToRenderFormat(paramDesc);
			sr.size = GraphicsUtility::GetRenderFormatSize(sr.format);
			sr.shaderType = shaderType;

			reflectionDatas.push_back(sr);
		}
	}

	void D3D11VertexShader::Bind() const
	{
		device->GetContext()->VSSetShader(vertexShader.Get(), nullptr, 0);
	}
	void D3D11VertexShader::Unbind() const
	{
		device->GetContext()->VSSetShader(nullptr, nullptr, 0);
	}

}