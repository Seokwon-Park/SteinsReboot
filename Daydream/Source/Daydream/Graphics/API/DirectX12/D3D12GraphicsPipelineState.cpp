#include "DaydreamPCH.h"
#include "D3D12GraphicsPipelineState.h"

#include "D3D12Utility.h"
#include "D3D12Shader.h"

namespace Daydream
{
	D3D12GraphicsPipelineState::D3D12GraphicsPipelineState(D3D12RenderDevice* _device, GraphicsPipelineStateDesc _desc)
		:GraphicsPipelineState(_desc)
	{
		device = _device;

		Array<D3D12_INPUT_ELEMENT_DESC> inputLayoutDesc;

		for (const auto& info : shaderGroup->GetInputData())
		{
			if (info.shaderResourceType != ShaderResourceType::Input) continue;
			D3D12_INPUT_ELEMENT_DESC elementDesc;
			elementDesc.SemanticName = info.name.c_str();
			elementDesc.SemanticIndex = info.binding;
			elementDesc.Format = GraphicsUtility::DirectX::ConvertToDXGIFormat(info.format);
			elementDesc.InputSlot = 0;
			elementDesc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;

			inputLayoutDesc.push_back(elementDesc);
		}

		UInt64 resourceSize = shaderGroup->GetShaderBindingMap().size();
		srvRanges.reserve(resourceSize);
		samplerRanges.reserve(resourceSize);
		
		UInt32 index = 0;
		for (auto [name, data] : shaderGroup->GetShaderBindingMap())
		{
			switch (data.shaderResourceType)
			{
			case ShaderResourceType::ConstantBuffer:
			{
				D3D12_ROOT_PARAMETER rootParam = {};
				rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
				rootParam.Descriptor.ShaderRegister = data.binding;
				rootParam.Descriptor.RegisterSpace = data.set;
				rootParam.ShaderVisibility = GraphicsUtility::DirectX12::GetD3D12ShaderVisibility(data.shaderType);
				rootParameters.push_back(rootParam);				
				break;
			}

			case ShaderResourceType::Texture:
			{
				D3D12_DESCRIPTOR_RANGE srvRange{};
				srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				srvRange.NumDescriptors = 1;
				srvRange.BaseShaderRegister = data.binding;
				srvRange.RegisterSpace = data.set;
				srvRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
				srvRanges.push_back(srvRange);

				D3D12_ROOT_PARAMETER rootParam{};
				rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				rootParam.DescriptorTable.NumDescriptorRanges = 1;
				rootParam.DescriptorTable.pDescriptorRanges = &srvRanges.back();
				rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
				rootParameters.push_back(rootParam);
				break;
			}
			case ShaderResourceType::Sampler:
			{
				D3D12_DESCRIPTOR_RANGE samplerRange{};
				samplerRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
				samplerRange.NumDescriptors = 1;
				samplerRange.BaseShaderRegister = data.binding;
				samplerRange.RegisterSpace = data.set;
				samplerRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
				samplerRanges.push_back(samplerRange);

				D3D12_ROOT_PARAMETER rootParam = {};
				rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				rootParam.DescriptorTable.NumDescriptorRanges = 1;
				rootParam.DescriptorTable.pDescriptorRanges = &samplerRanges[samplerRanges.size() - 1];
				rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
				rootParameters.push_back(rootParam);
				break;
			}
			default:
				break;
			}
			descriptorTable[name] = Cast<UInt32>(rootParameters.size()-1);
		}

		D3D12_ROOT_DESCRIPTOR rootDescriptor;
		rootDescriptor.ShaderRegister = 0;    // HLSL의 b0 레지스터
		rootDescriptor.RegisterSpace = 0;     // 기본 레지스터 공간


		//// 2. 루트 파라미터(Root Parameter) 정의
		//D3D12_ROOT_PARAMETER rootParameters[2];
		//rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; // 루트 CBV 타입
		//rootParameters[0].Descriptor = rootDescriptor;
		//rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; // 예: 정점 셰이더에서만 사용
		//		
		//rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		//rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
		//rootParameters[1].DescriptorTable.pDescriptorRanges = &range;
		//rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		//D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
		//staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		//staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		//staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		//staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		//staticSamplers[0].MipLODBias = 0;
		//staticSamplers[0].MaxAnisotropy = 0;
		//staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		//staticSamplers[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		//staticSamplers[0].MinLOD = 0;
		//staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
		//staticSamplers[0].ShaderRegister = 0;  // s0
		//staticSamplers[0].RegisterSpace = 0;
		//staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
		rootSignatureDesc.NumParameters = (UINT)rootParameters.size();
		rootSignatureDesc.pParameters = rootParameters.data();
		rootSignatureDesc.NumStaticSamplers = 0;
		rootSignatureDesc.pStaticSamplers = nullptr;
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
		DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to serialize root signature");

		hr = _device->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf()));
		DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create root signature");

		/*D3D12_RASTERIZER_DESC rasterizerDesc{};
		rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		rasterizerDesc.FrontCounterClockwise = false;
		rasterizerDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		rasterizerDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterizerDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterizerDesc.DepthClipEnable = TRUE;
		rasterizerDesc.MultisampleEnable = FALSE;
		rasterizerDesc.AntialiasedLineEnable = FALSE;
		rasterizerDesc.ForcedSampleCount = 0;
		rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;*/

		D3D12_RASTERIZER_DESC rasterizerDesc = GraphicsUtility::DirectX12::ConvertToD3D12RasterizerDesc(_desc.rasterizerState);

		DXGI_SAMPLE_DESC sampleDesc{};
		sampleDesc.Count = _desc.sampleCount;
		sampleDesc.Quality = 0;

		/*auto rpDesc = _desc.renderPass->GetDesc();*/
		
		D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
		pipelineStateDesc.pRootSignature = rootSignature.Get();
		pipelineStateDesc.VS = Cast<D3D12Shader*>(shaderGroup->GetShader(ShaderType::Vertex))->GetShaderBytecode();
		Shader* pixelShader = shaderGroup->GetShader(ShaderType::Pixel);
		if (pixelShader)
		{
			// PS가 있으면 바이트코드 설정
			pipelineStateDesc.PS = Cast<D3D12Shader*>(pixelShader)->GetShaderBytecode();
		}
		else
		{
			// PS가 없으면 (Depth Only 등) { nullptr, 0 } 으로 설정
			pipelineStateDesc.PS = { nullptr, 0 };
		}
		//desc.PS = static_cast<D3D12Shader*>(shaderGroup->GetShader(ShaderType::Pixel).get())->GetShaderBytecode();
		pipelineStateDesc.RasterizerState = rasterizerDesc;
		pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateDesc.NumRenderTargets = (UInt32)desc.renderTargetFormats.size();
		pipelineStateDesc.SampleDesc = sampleDesc;
		pipelineStateDesc.InputLayout.NumElements = static_cast<UInt32>(inputLayoutDesc.size());
		pipelineStateDesc.InputLayout.pInputElementDescs = inputLayoutDesc.data();
		for (UInt64 i = 0; i < desc.renderTargetFormats.size(); i++)
		{
			pipelineStateDesc.RTVFormats[i] = GraphicsUtility::DirectX::ConvertToDXGIFormat(desc.renderTargetFormats[i]);
		}
		pipelineStateDesc.SampleMask = UINT_MAX;

		pipelineStateDesc.BlendState.AlphaToCoverageEnable = FALSE;
		pipelineStateDesc.BlendState.IndependentBlendEnable = FALSE;
		const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc = {
			FALSE,FALSE,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL,
		};
		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
			pipelineStateDesc.BlendState.RenderTarget[i] = defaultRenderTargetBlendDesc;

		pipelineStateDesc.DSVFormat = GraphicsUtility::DirectX::ConvertToDXGIFormat(desc.depthStencilFormat); 
		D3D12_DEPTH_STENCIL_DESC dsDesc{};
		// 깊이/스텐실 상태 설정 (d3dx12 없이 직접 - 깊이 테스트 비활성화)
		dsDesc.DepthEnable = pipelineStateDesc.DSVFormat != DXGI_FORMAT_UNKNOWN;
		dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS; // 또는 적절한 함수
		dsDesc.StencilEnable = pipelineStateDesc.DSVFormat != DXGI_FORMAT_UNKNOWN; // 스텐실 테스트 끔
		dsDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;   // 0xFF (모든 비트 읽기)
		dsDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK; // 0xFF (모든 비트 쓰기)

		// FrontFace (기본적으로 DepthFunc과 동일하게 설정)
		dsDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;      // 스텐실 테스트 실패 시 동작 유지
		dsDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP; // 깊이 테스트 실패 시 동작 유지
		dsDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;      // 스텐실 및 깊이 테스트 모두 통과 시 동작 유지
		dsDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS; // 스텐실 함수 항상 통과

		// BackFace (기본적으로 FrontFace와 동일하게 설정)
		dsDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		dsDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		pipelineStateDesc.DepthStencilState = dsDesc;
		//// 아래 값들은 DepthEnable=FALSE 이므로 무시됨
		//desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		//desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;

		hr = device->GetDevice()->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(pipeline.GetAddressOf()));
		if (FAILED(hr))
		{
			// 에러 코드 출력
			DAYDREAM_CORE_ERROR("PSO creation failed with HRESULT: 0x{:x}", hr);
		}
		DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to create pipeline!");
	}

	D3D12GraphicsPipelineState::~D3D12GraphicsPipelineState()
	{
	}
	void D3D12GraphicsPipelineState::Bind() const
	{
		device->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
		device->GetCommandList()->SetPipelineState(pipeline.Get());
	}
	UInt32 D3D12GraphicsPipelineState::GetDescriptorTableIndex(String _resourceName)
	{
		auto itr = descriptorTable.find(_resourceName);
		if (itr == descriptorTable.end())
		{
			DAYDREAM_CORE_WARN("{} is not valid resource name", _resourceName);
		}
		return itr->second;
	}
	//Shared<Material> D3D12PipelineState::CreateMaterial()
	//{
	//	return MakeShared<D3D12Material>(device, this);
	//}
}

