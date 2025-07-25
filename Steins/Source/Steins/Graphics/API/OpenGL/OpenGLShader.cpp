#include "SteinsPCH.h"
#include "OpenGLShader.h"
#include "Steins/Enum/RendererEnums.h"

#include "spirv_cross/spirv_cross.hpp"

#include "Steins/Graphics/Utility/GraphicsUtil.h"
#include "Steins/Graphics/Utility/ShaderCompileHelper.h"

namespace Steins
{
	namespace
	{
//		RenderFormat ConvertToRenderFormat(GLenum glType)
//		{
//			switch (glType)
//			{
//				// Float types
//			case GL_FLOAT: return RenderFormat::R32_FLOAT;
//			case GL_FLOAT_VEC2: return RenderFormat::R32G32_FLOAT;
//			case GL_FLOAT_VEC3: return RenderFormat::R32G32B32_FLOAT;
//			case GL_FLOAT_VEC4: return RenderFormat::R32G32B32A32_FLOAT;
//
//				// Signed integer types
//			case GL_INT: return RenderFormat::R32_SINT;
//			case GL_INT_VEC2: return RenderFormat::R32G32_SINT;
//			case GL_INT_VEC3: return RenderFormat::R32G32B32_SINT;
//			case GL_INT_VEC4: return RenderFormat::R32G32B32A32_SINT;
//
//				// Unsigned integer types
//			case GL_UNSIGNED_INT: return RenderFormat::R32_UINT;
//			case GL_UNSIGNED_INT_VEC2: return RenderFormat::R32G32_UINT;
//			case GL_UNSIGNED_INT_VEC3: return RenderFormat::R32G32B32_UINT;
//			case GL_UNSIGNED_INT_VEC4: return RenderFormat::R32G32B32A32_UINT;
//
//				// Matrix types (treated as multiple vec4s)
//			case GL_FLOAT_MAT2: return RenderFormat::R32G32B32A32_FLOAT; // 2x2 matrix as 4 floats
//			case GL_FLOAT_MAT3: return RenderFormat::R32G32B32A32_FLOAT; // 3x3 matrix (usually padded)
//			case GL_FLOAT_MAT4: return RenderFormat::R32G32B32A32_FLOAT; // 4x4 matrix
//
//				// Half precision (if supported)
//#ifdef GL_HALF_FLOAT
//			case GL_HALF_FLOAT: return RenderFormat::R16_FLOAT;
//#endif
//			default: return RenderFormat::UNKNOWN;
//			}
//		}

		UInt32 GetComponentCount(GLenum glType)
		{
			switch (glType)
			{
				// Float types
			case GL_FLOAT: 
			case GL_INT:
			case GL_UNSIGNED_INT:
				return 1;

			case GL_FLOAT_VEC2:
			case GL_INT_VEC2: 
			case GL_UNSIGNED_INT_VEC2:
				return 2;

			case GL_FLOAT_VEC3:
			case GL_INT_VEC3:
			case GL_UNSIGNED_INT_VEC3:
				return 3;

			case GL_FLOAT_VEC4:
			case GL_INT_VEC4: 
			case GL_UNSIGNED_INT_VEC4: 
				return 4;

//			case GL_FLOAT_MAT2: return RenderFormat::R32G32B32A32_FLOAT; // 2x2 matrix as 4 floats
//			case GL_FLOAT_MAT3: return RenderFormat::R32G32B32A32_FLOAT; // 3x3 matrix (usually padded)
//			case GL_FLOAT_MAT4: return RenderFormat::R32G32B32A32_FLOAT; // 4x4 matrix
//
//				// Half precision (if supported)
//#ifdef GL_HALF_FLOAT
//			case GL_HALF_FLOAT: return RenderFormat::R16_FLOAT;
//#endif
			default: return 0;
			}
		}
	}
	OpenGLShader::OpenGLShader(const std::string& _src, const ShaderType& _type, const ShaderLoadMode& _mode)
	{
		shaderType = _type;
		//std::string src = _src;
		//if (_mode == ShaderLoadMode::File)
		//{
		//	src = "";
		//	std::ifstream file(_src);
		//	STEINS_CORE_ASSERT(file.is_open() == true, "Failed to open file!");
		//	std::string readline;
		//	std::stringstream stringStream;

		//	ShaderType currentType = _type;

		//	while (std::getline(file, readline))
		//	{
		//		src += readline + "\n";
		//	}
		//}
		FilePath path(_src);
		String src;
		Array<UInt32> spirvData;
		ShaderCompileHelper::ConvertHLSLtoSPIRV(path, _type, spirvData);
		spirv_cross::Compiler compiler(spirvData);
		spirv_cross::ShaderResources res = compiler.get_shader_resources();
		if (shaderType == ShaderType::Vertex)
		{
			for (const spirv_cross::Resource& resource : res.stage_inputs)
			{
				const spirv_cross::SPIRType& spirType = compiler.get_type(resource.type_id);

				ShaderReflectionInfo sr{};
				sr.name = compiler.get_name(resource.id);
				sr.binding = compiler.get_decoration(resource.id, spv::DecorationLocation);
				sr.shaderResourceType = ShaderResourceType::Input;

				UInt32 componentCount = spirType.vecsize;
				spirv_cross::SPIRType::BaseType baseType = spirType.basetype;
				sr.format = GraphicsUtil::ConvertSPIRVTypeToRenderFormat(baseType, componentCount);
				sr.count = componentCount;
				sr.size = GraphicsUtil::GetRenderFormatSize(sr.format);
				sr.shaderType = shaderType;

				reflectionInfo.push_back(sr);
			}
		}

		for (const spirv_cross::Resource& resource : res.uniform_buffers)
		{
			ShaderReflectionInfo sr{};
			sr.name = compiler.get_name(resource.id);
			sr.shaderResourceType = ShaderResourceType::ConstantBuffer;
			sr.set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			sr.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			sr.size = compiler.get_declared_struct_size(compiler.get_type(resource.type_id));

			reflectionInfo.push_back(sr);
		}

		for (const spirv_cross::Resource& resource : res.sampled_images)
		{
			ShaderReflectionInfo sr{};
			sr.name = compiler.get_name(resource.id);
			sr.shaderResourceType = ShaderResourceType::Texture2D;
			sr.set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			sr.binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

			const auto& type = compiler.get_type(resource.type_id);
			UInt32 count = 1;
			if (!type.array.empty())
			{
				count = type.array[0];
			}

			reflectionInfo.push_back(sr);
		}

		ShaderCompileHelper::ConvertSPIRVtoGLSL(spirvData, _type, src);
		Compile(src);

		//if (shaderType == ShaderType::Vertex)
		//{
		//	GLint numInputs;
		//	glGetProgramInterfaceiv(shaderProgramID, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numInputs);

		//	for (int i = 0; i < numInputs; ++i)
		//	{
		//		// 쿼리할 속성들
		//		GLenum properties[] = { GL_NAME_LENGTH, GL_TYPE, GL_LOCATION };
		//		GLint values[3];

		//		// 속성 정보 가져오기
		//		glGetProgramResourceiv(shaderProgramID, GL_PROGRAM_INPUT, i, 3, properties, 3, nullptr, values);

		//		// 이름 가져오기
		//		GLchar inputName[256];
		//		glGetProgramResourceName(shaderProgramID, GL_PROGRAM_INPUT, i, values[0], nullptr, inputName);

		//		ShaderReflectionInfo sr{};
		//		sr.name = inputName;
		//		sr.binding = values[2];
		//		sr.format = ConvertToRenderFormat(values[1]);
		//		sr.shaderResourceType = ShaderResourceType::Input;
		//		sr.size = GraphicsUtil::GetRenderFormatSize(sr.format);
		//		sr.count = GetComponentCount(values[1]);

		//		reflectionInfo.push_back(sr);
		//	}
		//}

		//ReflectUniformBlocks();
		//ReflectTextures();
	}

	void OpenGLShader::ReflectTextures()
	{
		GLint uniformCount;
		glGetProgramiv(shaderProgramID, GL_ACTIVE_UNIFORMS, &uniformCount);

		for (int i = 0; i < uniformCount; i++)
		{
			GLchar name[256];
			GLsizei length;
			GLint size;
			GLenum type;
			GLuint location;

			glGetProgramResourceName(shaderProgramID, GL_UNIFORM, i, sizeof(name), &length, name);
			glGetActiveUniform(shaderProgramID, i, sizeof(name), &length, &size, &type, name);
			location = glGetUniformLocation(shaderProgramID, name);

			// 샘플러 타입만 처리
			if (type == GL_SAMPLER_2D) {
				ShaderReflectionInfo desc{};
				desc.name = name;
				desc.shaderResourceType = ShaderResourceType::Texture2D;
				desc.set = 0;
				desc.binding = location;
				desc.count = size;
				desc.size = 0;

				reflectionInfo.push_back(desc);
			}
		}
	}

	void OpenGLShader::ReflectUniformBlocks()
	{
		GLint blockCount;
		glGetProgramiv(shaderProgramID, GL_ACTIVE_UNIFORM_BLOCKS, &blockCount);

		for (GLint i = 0; i < blockCount; i++) {
			GLchar name[256];
			GLsizei length;
			glGetActiveUniformBlockName(shaderProgramID, i, sizeof(name), &length, name);

			GLint binding = 0;
			GLint size = 0;
			glGetActiveUniformBlockiv(shaderProgramID, i, GL_UNIFORM_BLOCK_BINDING, &binding);
			glGetActiveUniformBlockiv(shaderProgramID, i, GL_UNIFORM_BLOCK_DATA_SIZE, &size);

			ShaderReflectionInfo desc{};
			desc.name = name;
			desc.shaderResourceType = ShaderResourceType::ConstantBuffer;
			desc.set = 0;
			desc.binding = binding;
			desc.count = 1;
			desc.size = size;

			reflectionInfo.push_back(desc);
			//// Block 내 uniform들 리플렉션
			//GLint activeUniforms;
			//glGetActiveUniformBlockiv(shaderProgramID, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &activeUniforms);

			//std::vector<GLint> uniformIndices(activeUniforms);
			//glGetActiveUniformBlockiv(shaderProgramID, i, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, uniformIndices.data());

			//for (GLint j = 0; j < activeUniforms; j++) {
			//	GLint index = uniformIndices[j];

			//	GLchar uniformName[256];
			//	GLint uniformSize;
			//	GLenum uniformType;
			//	GLint uniformOffset;

			//	glGetActiveUniform(shaderProgramID, index, sizeof(uniformName), nullptr, &uniformSize, &uniformType, uniformName);
			//	glGetActiveUniformsiv(shaderProgramID, 1, (GLuint*)&index, GL_UNIFORM_OFFSET, &uniformOffset);
			//}

			//uniformBlocks.push_back(blockInfo);
		}
	}


	OpenGLShader::~OpenGLShader()
	{
	}
	void OpenGLShader::Bind() const
	{
	}
	void OpenGLShader::Unbind() const
	{
	}

	void OpenGLShader::Compile(const std::string& _src)
	{
		GLuint shaderID = glCreateShader(GraphicsUtil::GetGLShaderType(shaderType));
		const GLchar* source = _src.c_str();

		glShaderSource(shaderID, 1, &source, nullptr);
		glCompileShader(shaderID);

		GLint isCompiled = 0;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

			Array<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shaderID, maxLength, &maxLength, &infoLog[0]);
			glDeleteShader(shaderID);

			STEINS_CORE_ERROR("{0}", infoLog.data());
			STEINS_CORE_ASSERT(false, "Shader compilation failure!");
			return;
		}

		shaderProgramID = glCreateShaderProgramv(GraphicsUtil::GetGLShaderType(shaderType), 1, &source);

		GLint isLinked = 0;
		glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(shaderProgramID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			Array<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(shaderProgramID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(shaderProgramID);
			// Don't leak shaders either.
			glDeleteShader(shaderID);

			STEINS_CORE_ERROR("{0}", infoLog.data());
			STEINS_CORE_ASSERT(false, "Shader link failure!");
			return;
		}

		glDeleteShader(shaderID);
	}
}
