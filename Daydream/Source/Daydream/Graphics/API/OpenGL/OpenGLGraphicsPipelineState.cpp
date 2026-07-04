#include "DaydreamPCH.h"
#include "OpenGLGraphicsPipelineState.h"
#include "OpenGLShader.h"

#include "Daydream/Graphics/Utility/GraphicsUtility.h"

#include "OpenGLUtility.h"

namespace Daydream
{
	OpenGLGraphicsPipelineState::OpenGLGraphicsPipelineState(GraphicsPipelineStateDesc _desc)
		:GraphicsPipelineState(_desc)
	{
		glCreateVertexArrays(1, &inputLayoutID);
		//for (const BufferElement& element : layout)
		UInt32 offset = 0;
		UInt32 inputDataIndex = 0;
		for(const auto& info : shaderPipeline->GetInputLayoutData())
		{
			glEnableVertexArrayAttrib(inputLayoutID, inputDataIndex);
			glVertexArrayAttribFormat(inputLayoutID, inputDataIndex,
				GraphicsUtility::GetRenderFormatCount(info.format),
				GraphicsUtility::OpenGL::ConvertRenderFormatToGLDataType(info.format),
				GL_FALSE,
				offset);
			glVertexArrayAttribBinding(inputLayoutID, inputDataIndex, 0);
			offset += (UInt32)info.size;
			inputDataIndex++;
		}

		//glVertexArrayBindingDivisor(vao, 0, 0);

		glCreateProgramPipelines(1, &pipeline);
		glBindProgramPipeline(pipeline);

		GLint textureIndex = 0;
		//static GLint uboIndex = 0;
		for (auto shader : GetShaders())
		{
			OpenGLShader* glShader = Cast<OpenGLShader*>(shader);
			GLenum type = GraphicsUtility::OpenGL::ConvertToShaderStageBit(shader->GetType());
			GLuint shaderID = static_cast<GLuint>(glShader->GetShaderID());
			glUseProgramStages(pipeline, type, shaderID);
			for (auto& info : shader->GetShaderReflectionData())
			{
				switch (info.shaderResourceType)
				{
				case ShaderReflectionDataType::ConstantBuffer:
				{
					//info.set = uboIndex;
					//glUniformBlockBinding(shaderID, info.binding, uboIndex++);
					break;
				}
				case ShaderReflectionDataType::Texture:
				{
					break;
				}
				}
			}
		}

		glValidateProgramPipeline(pipeline);
		GLint success;
		char infoLog[512];
		glGetProgramPipelineiv(pipeline, GL_VALIDATE_STATUS, &success);
		if (!success) {
			glGetProgramPipelineInfoLog(pipeline, 512, NULL, infoLog);
			DAYDREAM_CORE_ERROR("pipeline validation failed!\n {0}", infoLog);
		}
	}
	OpenGLGraphicsPipelineState::~OpenGLGraphicsPipelineState()
	{

	}

	void OpenGLGraphicsPipelineState::BindPipelineState() const
	{
		///////////////////////////////// Rasterizer State //////////////////////////////////////////////////
		const RasterizerStateDesc& rsDesc = desc.rasterizerState;
		// Fill Mode (Solid vs Wireframe)
		GLenum polygonMode = (rsDesc.fillMode == FillMode::Wireframe) ? GL_LINE : GL_FILL;
		glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

		
		// Cull Mode
		if (rsDesc.cullMode == CullMode::None)
		{
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glEnable(GL_CULL_FACE);
			// Front: GL_FRONT, Back: GL_BACK
			GLenum face = (rsDesc.cullMode == CullMode::Front) ? GL_FRONT : GL_BACK;
			glCullFace(face);
		}

		// Winding Order (Front Counter Clockwise)
		// true: CCW (반시계), false: CW (시계)
		glFrontFace(rsDesc.frontCounterClockwise ? GL_CCW : GL_CW);

		// Depth Clip Enable
		if (rsDesc.depthClipEnable)
		{
			glDisable(GL_DEPTH_CLAMP); // Clipping 활성화 (기본 동작)
		}
		else
		{
			glEnable(GL_DEPTH_CLAMP);  // Clipping 비활성화 (Clamping)
		}

		///////////////////////////////// Depth Stencil State //////////////////////////////////////////////////
		const DepthStencilStateDesc& dssDesc = desc.depthStencilState;

		if (dssDesc.depthEnable)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthMask(dssDesc.depthWriteEnable ? GL_TRUE : GL_FALSE);
			glDepthFunc(GraphicsUtility::OpenGL::ConvertToGLCompareFunc(dssDesc.depthFunc));
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		// 2. 스텐실 (Stencil) 설정
		if (dssDesc.stencilEnable)
		{
			glEnable(GL_STENCIL_TEST);

			glStencilMask(dssDesc.stencilWriteMask);

			// Front Face
			glStencilOpSeparate(GL_FRONT,
				GraphicsUtility::OpenGL::ConvertToGLStencilOp(dssDesc.frontFace.failOp),
				GraphicsUtility::OpenGL::ConvertToGLStencilOp(dssDesc.frontFace.depthFailOp),
				GraphicsUtility::OpenGL::ConvertToGLStencilOp(dssDesc.frontFace.passOp));

			glStencilFuncSeparate(GL_FRONT,
				GraphicsUtility::OpenGL::ConvertToGLCompareFunc(dssDesc.frontFace.compareFunc),
				1, 
				dssDesc.stencilReadMask);

			// Back Face
			glStencilOpSeparate(GL_BACK,
				GraphicsUtility::OpenGL::ConvertToGLStencilOp(dssDesc.backFace.failOp),
				GraphicsUtility::OpenGL::ConvertToGLStencilOp(dssDesc.backFace.depthFailOp),
				GraphicsUtility::OpenGL::ConvertToGLStencilOp(dssDesc.backFace.passOp));

			glStencilFuncSeparate(GL_BACK,
				GraphicsUtility::OpenGL::ConvertToGLCompareFunc(dssDesc.backFace.compareFunc),
				1,
				dssDesc.stencilReadMask);
		}
		else
		{
			glDisable(GL_STENCIL_TEST);
		}
		//// Scissor Enable
		//if (rsDesc.scissorEnable)
		//	glEnable(GL_SCISSOR_TEST);
		//else
		//	glDisable(GL_SCISSOR_TEST);

		//// Multisample Enable
		//if (rsDesc.multisampleEnable)
		//	glEnable(GL_MULTISAMPLE);
		//else
		//	glDisable(GL_MULTISAMPLE);

		//// Antialiased Line Enable
		//if (rsDesc.antialiasedLineEnable)
		//	glEnable(GL_LINE_SMOOTH);
		//else
		//	glDisable(GL_LINE_SMOOTH);

		//// Depth Bias (Polygon Offset)
		//if (rsDesc.depthBias != 0 || rsDesc.slopeScaledDepthBias != 0.0f)
		//{
		//	glEnable(GL_POLYGON_OFFSET_FILL); // Wireframe일 경우 GL_POLYGON_OFFSET_LINE 고려 필요
		//	glPolygonOffset(rsDesc.slopeScaledDepthBias, static_cast<float>(rsDesc.depthBias));
		//}
		//else
		//{
		//	glDisable(GL_POLYGON_OFFSET_FILL);
		//}
		glBindVertexArray(inputLayoutID);
		glBindProgramPipeline(pipeline);
	}
	//Shared<Material> OpenGLPipelineState::CreateMaterial()
	//{
	//	return MakeShared<OpenGLMaterial>(this);
	//}
}
