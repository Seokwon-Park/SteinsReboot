#pragma once

#include "D3D11Shader.h"

namespace Daydream
{
	class D3D11VertexShader : public D3D11Shader
	{
	public:
		D3D11VertexShader(D3D11RenderDevice* _device, const String& _src, const ShaderLoadMode& _mode);

		virtual void Bind() const override;
		virtual void Unbind() const override;

		ID3D11VertexShader* GetID3D11VertexShader() const { return vertexShader.Get(); }
	private:
		ComPtr<ID3D11VertexShader> vertexShader;
	};
}