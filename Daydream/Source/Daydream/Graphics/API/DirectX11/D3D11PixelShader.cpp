#include "DaydreamPCH.h"
#include "D3D11Shader.h"

namespace Daydream
{
	D3D11PixelShader::D3D11PixelShader(D3D11RenderDevice* _device)
		:D3D11Shader(_device, ShaderType::Pixel)
	{
	}

	bool D3D11PixelShader::CreateNativeShader(const Array<UInt8>& _bytecode)
	{
		device->GetDevice()->CreatePixelShader(_bytecode.data(), _bytecode.size(), nullptr, pixelShader.GetAddressOf());

		return true;
	}



}