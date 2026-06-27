#pragma once

#include "Daydream/Graphics/Resources/Shader.h"
#include "D3D12RenderDevice.h"
#include "dxc/dxcapi.h"

namespace Daydream
{
	class D3D12Shader : public Shader
	{
	public:
		D3D12Shader(D3D12RenderDevice* _device, const ShaderType& _type);
				
		virtual bool CreateNativeShader(const Array<UInt8>& _bytecode) override;

		D3D12_SHADER_BYTECODE GetShaderBytecode() const { return shaderBytecode; }
	private:
		D3D12RenderDevice* device;
		D3D12_SHADER_BYTECODE shaderBytecode;
		Array<UInt8> bytecode;
	};
}