#pragma once

#include "Daydream/Graphics/Resources/Shader.h"

#include "Daydream/Graphics/States/BlendState.h"
#include "Daydream/Graphics/States/RasterizerState.h"
#include "Daydream/Graphics/States/DepthStencilState.h"

namespace Daydream
{
	enum class PrimitiveTopologyType
	{
		TriangleList,
	};

	struct ShaderPipelineDesc
	{
		Shader* vertexShader = nullptr;
		Shader* hullShader = nullptr;
		Shader* domainShader = nullptr;
		Shader* geometryShader = nullptr;
		Shader* pixelShader = nullptr;

		//BufferLayout inputLayout;
		//InputLayoutDesc inputLayout;
		//Shared<ResourceBindingLayout> resourceBindingLayout; // RootSignature/PipelineLayout
		RasterizerStateDesc rasterizerState{};
		DepthStencilStateDesc depthStencilState{};
		BlendStateDesc blendState{};

		UInt32 sampleCount = 1;
		//GraphicsFormat depthStencilFormat = GraphicsFormat::Unknown; // DSV Æ÷¸Ë
		PrimitiveTopologyType topologyType = PrimitiveTopologyType::TriangleList;
	};

	class ShaderPipeline : public Asset
	{
	public:
		ASSET_CLASS_TYPE(ShaderPipeline);
		ShaderPipeline(const ShaderPipelineDesc& _desc);

		const Array<Shader*>& GetShaders() const;
		Shader* GetShader(ShaderType _type) const;

		static Shared<ShaderPipeline> Create(const ShaderPipelineDesc& _desc);

		const Array<ShaderLayoutData>& GetInputLayoutData() const { return pipelineInputData; }
		const Array<ShaderLayoutData>& GetOutputLayoutData() const { return pipelineOutputData; }
		const HashMap<String, ShaderReflectionData>& GetShaderBindingMap() const { return shaderBindingMap; }
		const HashMap<String, ShaderReflectionDataType>& GetMaterialMap() const { return materialMap; }
		const ShaderReflectionData* GetShaderBindingInfo(const String& _name) const;

		const RasterizerStateDesc& GetRS() const { return rasterizerState; }
		const DepthStencilStateDesc& GetDSS() const { return depthStencilState; }
		const BlendStateDesc& GetBS() const { return blendState; }

		UInt32 GetSetCount() const { return setCount; }
	private:

		void SetIOLayoutData();
		void CreateShaderBindingMap();
		void CreateMaterialMap();

		void ValidateInterface() { /* ... */ }
		void GenerateUnifiedReflection() { /* ... */ }

		UInt32 setCount = 0;

		Shader* vertexShader = nullptr;
		Shader* hullShader = nullptr;
		Shader* domainShader = nullptr;
		Shader* geometryShader = nullptr;
		Shader* pixelShader = nullptr;

		Array<Shader*> shaders;

		Array<ShaderLayoutData> pipelineInputData;
		Array<ShaderLayoutData> pipelineOutputData;
		HashMap<String, ShaderReflectionData> shaderBindingMap;
		HashMap<String, ShaderReflectionDataType> materialMap;

		RasterizerStateDesc rasterizerState{};
		DepthStencilStateDesc depthStencilState{};
		BlendStateDesc blendState{};
	};
}
