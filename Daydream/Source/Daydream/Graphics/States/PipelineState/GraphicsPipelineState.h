#pragma once

#include "Daydream/Graphics/Resources/Shader.h"
#include "Daydream/Graphics/Resources/ShaderGroup.h"
#include "Daydream/Graphics/Resources/Buffer.h"
#include "Daydream/Graphics/Resources/Texture/Texture.h"
#include "Daydream/Graphics/Resources/Texture/TextureView.h"
#include "Daydream/Graphics/Resources/Material.h"
#include "Daydream/Graphics/States/BlendState.h"
#include "Daydream/Graphics/States/RasterizerState.h"
#include "Daydream/Graphics/States/DepthStencilState.h"


namespace Daydream
{
	enum class PrimitiveTopologyType
	{
		TriangleList,
	};

	enum class AttachmentType
	{
		None,
		EntityHandle,
	};



	struct GraphicsPipelineStateDesc
	{
		ShaderGroup* shaderGroup;
		//Shared<Shader> computeShader; //??

		//BufferLayout inputLayout;
		//InputLayoutDesc inputLayout;
		//Shared<ResourceBindingLayout> resourceBindingLayout; // RootSignature/PipelineLayout
		RasterizerStateDesc rasterizerState;
		//BlendDesc blendState;
		//DepthStencilDesc depthStencilState;
		Array<RenderFormat> renderTargetFormats; // RTV 포맷들
		RenderFormat depthStencilFormat = RenderFormat::UNKNOWN; // DSV 포맷

		UInt32 sampleCount = 1;
		//GraphicsFormat depthStencilFormat = GraphicsFormat::Unknown; // DSV 포맷
		PrimitiveTopologyType topologyType = PrimitiveTopologyType::TriangleList;
	};

	class GraphicsPipelineState
	{
	public:
		GraphicsPipelineState(const GraphicsPipelineStateDesc& _desc);
		virtual ~GraphicsPipelineState() = default;

		virtual void Bind() const = 0;
		//virtual Shared<Material> CreateMaterial() = 0;
		 
		//ShaderGroup Functions
		inline const ShaderGroup* GetShaderGroup() const { return shaderGroup; }
		inline const Array<Shader*>& GetShaders() const { return shaderGroup->GetShaders(); };
		inline const ShaderReflectionData* GetBindingInfo(const String& _name) const {
			return shaderGroup->GetShaderBindingInfo(_name);
		}

		static Shared<GraphicsPipelineState> Create(const GraphicsPipelineStateDesc& _desc);
	protected:

		ShaderGroup* shaderGroup;

		GraphicsPipelineStateDesc desc;
		//rtv, dsv;
		//blend, rast, ds;
	};
}