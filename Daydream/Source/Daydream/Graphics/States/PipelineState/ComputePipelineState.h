#pragma once

namespace Daydream
{
	class ComputePipelineState
	{
	public:
		ComputePipelineState(const ComputePipelineStateDesc& _desc);
		virtual ~ComputePipelineState() = default;

		inline const ShaderPipeline* GetShaderPipeline() const { return shaderPipeline; }
		inline const Array<Shader*>& GetShaders() const { return shaderPipeline->GetShaders(); };
		inline const ShaderReflectionData* GetBindingInfo(const String& _name) const
		{
			return shaderPipeline->GetShaderBindingInfo(_name);
		}

		static Shared<GraphicsPipelineState> Create(const GraphicsPipelineStateDesc& _desc);
	protected:
		ComputePipelineStateDesc desc;
		//rtv, dsv;
		//blend, rast, ds;
	};
}
