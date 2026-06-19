#include "DaydreamPCH.h"
#include "D3D11VertexShader.h"

#include "Daydream/Graphics/Utility/GraphicsUtility.h"

namespace Daydream
{
	D3D11VertexShader::D3D11VertexShader(D3D11RenderDevice* _device)
		:D3D11Shader(_device, ShaderType::Vertex)
	{

		//// 셰이더 입력 시그니처 정보 얻기
		//D3D11_SHADER_DESC shaderDesc;
		//reflection->GetDesc(&shaderDesc);

		//// 각 입력 파라미터에 대해 순회
		//for (UINT i = 0; i < shaderDesc.InputParameters; i++)
		//{
		//	D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		//	reflection->GetInputParameterDesc(i, &paramDesc);

		//	ShaderReflectionData sr{};
		//	sr.name = paramDesc.SemanticName;
		//	sr.set = 0;
		//	sr.binding = paramDesc.SemanticIndex;
		//	sr.shaderResourceType = ShaderReflectionDataType::Input;
		//	sr.format = ConvertToRenderFormat(paramDesc);
		//	sr.size = GraphicsUtility::GetRenderFormatSize(sr.format);
		//	sr.shaderType = shaderType;

		//	reflectionDatas.push_back(sr);
		//}
	}

	bool D3D11VertexShader::CreateNativeShader(const Array<UInt8>& _bytecode)
	{
		bytecode = _bytecode;
		HRESULT hr = device->GetDevice()->CreateVertexShader(_bytecode.data(), _bytecode.size(), nullptr, vertexShader.GetAddressOf());

		return SUCCEEDED(hr);
	}
}