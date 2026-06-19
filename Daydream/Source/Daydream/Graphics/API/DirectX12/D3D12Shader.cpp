#include "DaydreamPCH.h"
#include "D3D12Shader.h"

#include "Daydream/Graphics/Utility/GraphicsUtility.h"
#include "Daydream/Graphics/Utility/ShaderCompileHelper.h"

namespace Daydream
{


	D3D12Shader::D3D12Shader(D3D12RenderDevice* _device, const ShaderType& _type)
	{
		device = _device;
		shaderType = _type;
		//String target = GraphicsUtility::GetShaderTargetName(_type, "6_0");
		//String entryPoint = GraphicsUtility::GetShaderEntryPointName(_type);
		//HRESULT hr;

		//ComPtr<ID3D12ShaderReflection> reflection = nullptr;

		//Path path(_src);
		//ShaderCompileHelper::CompileToDXIL(path, _type, shaderBlob);
		//ShaderCompileHelper::GetDX12Reflection(shaderBlob, reflection);

		//// 셰이더 입력 시그니처 정보 얻기
		//D3D12_SHADER_DESC shaderDesc;
		//hr = reflection->GetDesc(&shaderDesc);
		//DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to get shader description")

		//// 각 입력 파라미터에 대해 순회
		//if (shaderType == ShaderType::Vertex)
		//{
		//	for (UInt32 i = 0; i < shaderDesc.InputParameters; i++)
		//	{
		//		D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
		//		reflection->GetInputParameterDesc(i, &paramDesc);

		//		ShaderReflectionData sr{};
		//		sr.name = paramDesc.SemanticName;
		//		sr.set = 0;
		//		sr.binding = paramDesc.SemanticIndex;
		//		sr.shaderResourceType = ShaderReflectionDataType::Input;
		//		sr.format = ConvertToRenderFormat(paramDesc);
		//		sr.size = GraphicsUtility::GetRenderFormatSize(sr.format);
		//		sr.shaderType = shaderType;

		//		reflectionDatas.push_back(sr);
		//	}
		//}

		//// cbuffer의 바인딩 정보 찾기
		//D3D12_SHADER_INPUT_BIND_DESC bindDesc;
		//for (UInt32 i = 0; i < shaderDesc.BoundResources; i++)
		//{
		//	reflection->GetResourceBindingDesc(i, &bindDesc);

		//	String name = bindDesc.Name;

		//	D3D_SHADER_INPUT_TYPE type = bindDesc.Type;

		//	ShaderReflectionData sr{};
		//	switch (type)
		//	{
		//	case D3D_SIT_CBUFFER:
		//	{
		//		ID3D12ShaderReflectionConstantBuffer* cbuffer = reflection->GetConstantBufferByIndex(i);
		//		D3D12_SHADER_BUFFER_DESC cbufferDesc;
		//		cbuffer->GetDesc(&cbufferDesc);

		//		sr.name = name;
		//		sr.shaderResourceType = ShaderReflectionDataType::ConstantBuffer;
		//		sr.set = bindDesc.Space;
		//		sr.binding = bindDesc.BindPoint;
		//		sr.count = bindDesc.BindCount;
		//		sr.size = cbufferDesc.Size;
		//		sr.shaderType = shaderType;
		//		reflectionDatas.push_back(sr);
		//		break;
		//	}
		//	case D3D_SIT_TEXTURE:
		//	{
		//		sr.name = name;
		//		sr.shaderResourceType = ShaderReflectionDataType::Texture;
		//		sr.set = bindDesc.Space;
		//		sr.binding = bindDesc.BindPoint;
		//		sr.count = bindDesc.BindCount;
		//		reflectionDatas.push_back(sr);
		//		break;

		//	}
		//	case D3D_SIT_SAMPLER:
		//	{
		//		sr.name = name;
		//		sr.shaderResourceType = ShaderReflectionDataType::Sampler;
		//		sr.set = bindDesc.Space;
		//		sr.binding = bindDesc.BindPoint;
		//		sr.count = bindDesc.BindCount;
		//		reflectionDatas.push_back(sr);
		//		break;
		//	}
		//	}
		//}

		//shaderByteCode.pShaderBytecode = shaderBlob->GetBufferPointer();
		//shaderByteCode.BytecodeLength = shaderBlob->GetBufferSize();

	}

	bool D3D12Shader::CreateNativeShader(const Array<UInt8>& _bytecode) 
	{

	}

}
