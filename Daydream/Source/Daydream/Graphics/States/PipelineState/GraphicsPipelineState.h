#pragma once

#include "Daydream/Graphics/Resources/Shader.h"
#include "Daydream/Graphics/Resources/ShaderPipeline.h"
#include "Daydream/Graphics/Resources/Buffer.h"
#include "Daydream/Graphics/Resources/Texture/Texture.h"
#include "Daydream/Graphics/Resources/Texture/TextureView.h"
#include "Daydream/Graphics/Resources/Material.h"
#include "Daydream/Graphics/States/BlendState.h"
#include "Daydream/Graphics/States/RasterizerState.h"
#include "Daydream/Graphics/States/DepthStencilState.h"

namespace Daydream
{
	struct GraphicsPipelineStateDesc
	{
		ShaderPipeline* shaderPipeline = nullptr;

		RasterizerStateDesc rasterizerState{};
		DepthStencilStateDesc depthStencilState{};
		BlendStateDesc blendState{};

		Array<RenderFormat> renderTargetFormats; // RTV 포맷들
		RenderFormat depthStencilFormat = RenderFormat::UNKNOWN; // DSV 포맷
		UInt32 sampleCount = 1;

		void InitWithShaderPipeline(ShaderPipeline* _shaderPipeline)
		{
			shaderPipeline = _shaderPipeline;
			rasterizerState = _shaderPipeline->GetRS();
			depthStencilState = _shaderPipeline->GetDSS();
			blendState = _shaderPipeline->GetBS();
		}

		void InitWithMaterial(Material* _material)
		{

		}

		bool operator==(const GraphicsPipelineStateDesc& other) const
		{
			if (shaderPipeline != other.shaderPipeline) return false;
			if (depthStencilFormat != other.depthStencilFormat) return false;
			if (sampleCount != other.sampleCount) return false;

			if (renderTargetFormats.size() != other.renderTargetFormats.size()) return false;
			for (size_t i = 0; i < renderTargetFormats.size(); ++i) {
				if (renderTargetFormats[i] != other.renderTargetFormats[i]) return false;
			}
			return true;
		}
	};

	class GraphicsPipelineState
	{
	public:
		GraphicsPipelineState(const GraphicsPipelineStateDesc& _desc);
		virtual ~GraphicsPipelineState() = default;

		inline const ShaderPipeline* GetShaderPipeline() const { return shaderPipeline; }
		inline const Array<Shader*>& GetShaders() const { return shaderPipeline->GetShaders(); };
		inline const ShaderReflectionData* GetBindingInfo(const String& _name) const
		{
			return shaderPipeline->GetShaderBindingInfo(_name);
		}

		static Shared<GraphicsPipelineState> Create(const GraphicsPipelineStateDesc& _desc);
	protected:
		ShaderPipeline* shaderPipeline;

		GraphicsPipelineStateDesc desc;
		//rtv, dsv;
		//blend, rast, ds;
	};
}