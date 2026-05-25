#pragma once

#include "Daydream/Graphics/Resources/Shader.h"
#include "D3D12RenderDevice.h"
#include "dxc/dxcapi.h"

namespace Daydream
{
	class D3D12Shader : public Shader
	{
	public:
		D3D12Shader(D3D12RenderDevice* _device, const std::string& _src, const ShaderType& _type, const ShaderLoadMode& _mode);

		void Bind() const override;
		void Unbind() const override;
		
		D3D12_SHADER_BYTECODE GetShaderBytecode() const { return shaderByteCode; }
		IDxcBlob* GetIDxcBlob() const { return shaderBlob.Get(); }
	private:
		D3D12RenderDevice* device;
		D3D12_SHADER_BYTECODE shaderByteCode;
		ComPtr<IDxcBlob> shaderBlob;
	};
}