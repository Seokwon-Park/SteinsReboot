#pragma once

#include "Daydream/Graphics/Resources/Shader.h"
#include "D3D11RenderDevice.h"

#include <dxc/dxcapi.h>

namespace Daydream
{
	class D3D11Shader : public Shader
	{
	public:
		D3D11Shader(D3D11RenderDevice* _device, const ShaderType& _type);
		virtual ~D3D11Shader() override ;
	protected:
		virtual bool CreateNativeShader(const Array<UInt8>& _bytecode) override { return false;  };
		D3D11RenderDevice* device;
	private:
	};
	

	class D3D11PixelShader : public D3D11Shader
	{
	public:
		D3D11PixelShader(D3D11RenderDevice* _device);

		virtual bool CreateNativeShader(const Array<UInt8>& _bytecode) override;

		ID3D11PixelShader* GetID3D11PixelShader() const { return pixelShader.Get(); }

	private:
		ComPtr<ID3D11PixelShader> pixelShader;
	};

	class D3D11HullShader : public D3D11Shader
	{
	public:
		D3D11HullShader(D3D11RenderDevice* _device);



		//virtual void SetMat4(const std::string& _name, const Matrix4x4& _value) override;

	private:
		ComPtr<ID3D11HullShader> hullShader;
	};

	class D3D11DomainShader : public D3D11Shader
	{
	public:
		D3D11DomainShader(D3D11RenderDevice* _device);


		//virtual void SetMat4(const std::string& _name, const Matrix4x4& _value) override;

	private:
		ComPtr<ID3D11DomainShader> domainShader;
	};

	class D3D11GeometryShader : public D3D11Shader
	{
	public:
		D3D11GeometryShader(D3D11RenderDevice* _device);



		//virtual void SetMat4(const std::string& _name, const Matrix4x4& _value) override;

	private:
		ComPtr<ID3D11GeometryShader> geometryShader;
	};
}


