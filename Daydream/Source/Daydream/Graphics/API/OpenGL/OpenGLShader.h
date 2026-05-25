#pragma once

#include "Daydream/Graphics/Resources/Shader.h"
#include "glad/glad.h"

namespace Daydream
{
	class OpenGLShader :public Shader
	{
	public:
		OpenGLShader(const std::string& _src, const ShaderType& _type, const ShaderLoadMode& _mode);

		void ReflectTextures();

		void ReflectUniformBlocks();

		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		UInt32 GetShaderID() const { return shaderProgramID; }

		//virtual void SetMat4(const std::string& _name, const Matrix4x4& _value) override;
	private:
		void Compile(const std::string& _src);
	private:
		GLuint shaderProgramID;
	};
}

