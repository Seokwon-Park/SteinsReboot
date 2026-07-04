#pragma once

#include "Daydream/Graphics/States/PipelineState/GraphicsPipelineState.h"
#include "glad/glad.h"

namespace Daydream
{
	class OpenGLGraphicsPipelineState : public GraphicsPipelineState
	{
	public:
		OpenGLGraphicsPipelineState(GraphicsPipelineStateDesc _desc);
		virtual ~OpenGLGraphicsPipelineState();

		void BindPipelineState() const;

		//virtual Shared<Material> CreateMaterial() override;
	private:
		GLuint inputLayoutID;
		GLuint pipeline;
	};
}
