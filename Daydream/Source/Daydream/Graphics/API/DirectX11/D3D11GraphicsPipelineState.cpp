#include "DaydreamPCH.h"
#include "D3D11GraphicsPipelineState.h"

#include "D3D11Utility.h"

#include "D3D11VertexShader.h"

namespace Daydream
{
	D3D11GraphicsPipelineState::D3D11GraphicsPipelineState(D3D11RenderDevice* _device, GraphicsPipelineStateDesc _desc)
		:GraphicsPipelineState(_desc)
	{
		device = _device;

		// 입력 파라미터 정보를 저장할 벡터
		Array<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;

		for (const auto& info : shaderGroup->GetInputLayoutData())
		{
			D3D11_INPUT_ELEMENT_DESC elementDesc;
			elementDesc.SemanticName = info.name.c_str();
			elementDesc.SemanticIndex = info.location;
			elementDesc.InputSlot = 0;
			elementDesc.Format = GraphicsUtility::DirectX::ConvertToDXGIFormat(info.format);
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;

			inputLayoutDesc.push_back(elementDesc);
		}

		for (const auto& shader : shaderGroup->GetShaders())
		{
			switch (shader->GetType())
			{
			case ShaderType::Vertex:
			{
				D3D11VertexShader* vs = Cast<D3D11VertexShader*>(shaderGroup->GetShader(ShaderType::Vertex));
				DAYDREAM_CORE_ASSERT(vs, "Vertex Shader is nullptr!");

				// 입력 레이아웃 생성
				HRESULT hr = device->GetDevice()->CreateInputLayout(
					inputLayoutDesc.data(),
					(UINT)inputLayoutDesc.size(),
					vs->GetBytecode().data(),
					vs->GetBytecode().size(),
					inputLayout.GetAddressOf()
				);
				DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create inputlayout!");

				vertexShader = vs->GetID3D11VertexShader();
				break;
			}
			case ShaderType::Pixel:
			{
				D3D11PixelShader* ps = Cast<D3D11PixelShader*>(shaderGroup->GetShader(ShaderType::Pixel));
				pixelShader = ps->GetID3D11PixelShader();
				break;
			}
			default:
			{
				break;
			}
			}
		}

		//hullShader = (ID3D11HullShader*)shaderGroup->GetShader(ShaderType::Hull)->GetNativeHandle();
		//domainShader = (ID3D11DomainShader*)shaderGroup->GetShader(ShaderType::Domain)->GetNativeHandle();
		//geometryShader = (ID3D11GeometryShader*)shaderGroup->GetShader(ShaderType::Geometry)->GetNativeHandle();

		//CW
		D3D11_RASTERIZER_DESC rastDesc = GraphicsUtility::DirectX11::ConvertToD3D11RasterizerDesc(_desc.rasterizerState);
		_device->GetDevice()->CreateRasterizerState(&rastDesc, rasterizer.GetAddressOf());

		D3D11_DEPTH_STENCIL_DESC dsDesc = GraphicsUtility::DirectX11::ConvertToD3D11DepthStencilDesc(_desc.depthStencilState);
		_device->GetDevice()->CreateDepthStencilState(&dsDesc, depthStencil.GetAddressOf());

		//D3D11_BLEND_DESC blendDesc;
	}

	void D3D11GraphicsPipelineState::Bind() const
	{

	}
	void D3D11GraphicsPipelineState::BindPipelineState() const 
	{
		device->GetContext()->VSSetShader(vertexShader, nullptr, 0);
		device->GetContext()->PSSetShader(pixelShader, nullptr, 0);
		device->GetContext()->HSSetShader(hullShader, nullptr, 0);
		device->GetContext()->DSSetShader(domainShader, nullptr, 0);
		device->GetContext()->GSSetShader(geometryShader, nullptr, 0);

		device->GetContext()->OMSetDepthStencilState(depthStencil.Get(), 0);
		//device->GetContext()->OMSetBlendState(nullptr, nullptr, 0);

		device->GetContext()->IASetInputLayout(inputLayout.Get());
		device->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		device->GetContext()->RSSetState(rasterizer.Get());
	}
	//Shared<Material> D3D11PipelineState::CreateMaterial()
	//{
	//	return MakeShared<D3D11Material>(device, this);
	//}
}
