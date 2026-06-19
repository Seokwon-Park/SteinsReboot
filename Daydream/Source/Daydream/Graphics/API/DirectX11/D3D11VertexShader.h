#pragma once

#include "D3D11Shader.h"

namespace Daydream
{
	class D3D11VertexShader : public D3D11Shader
	{
	public:
		D3D11VertexShader(D3D11RenderDevice* _device);

		virtual bool CreateNativeShader(const Array<UInt8>& _bytecode) override;

		ID3D11VertexShader* GetID3D11VertexShader() const { return vertexShader.Get(); }
		const Array<UInt8>& GetBytecode() const { return bytecode; }
	private:
		ComPtr<ID3D11VertexShader> vertexShader;
		Array<UInt8> bytecode;
	};
}