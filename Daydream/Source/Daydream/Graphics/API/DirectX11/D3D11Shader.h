#pragma once

#include "Daydream/Graphics/Resources/Shader.h"
#include "D3D11RenderDevice.h"

#include <dxc/dxcapi.h>

namespace Daydream
{
	class D3D11Shader : public Shader
	{
	public:
		D3D11Shader(D3D11RenderDevice* _device, const std::string& _src, const ShaderType& _type, const ShaderLoadMode& _mode);
		virtual ~D3D11Shader() override ;

		virtual void Bind() const override;
		virtual void Unbind() const override;
		
		ID3DBlob* GetShaderBlob() const { return shaderBlob.Get(); }
	protected:
		D3D11RenderDevice* device;
		ComPtr<ID3DBlob> shaderBlob;
		ComPtr<ID3DBlob> errorBlob;
		ComPtr<ID3D11ShaderReflection> reflection;
	private:
	};
	

	class D3D11PixelShader : public D3D11Shader
	{
	public:
		D3D11PixelShader(D3D11RenderDevice* _device, const std::string& _src, const ShaderLoadMode& _mode);

		virtual void Bind() const override;
		virtual void Unbind() const override;

		ID3D11PixelShader* GetID3D11PixelShader() const { return pixelShader.Get(); }

	private:
		ComPtr<ID3D11PixelShader> pixelShader;
	};

	class D3D11HullShader : public D3D11Shader
	{
	public:
		D3D11HullShader(const Path& _filepath);
		D3D11HullShader(const std::string& _src);

		virtual void Bind() const override;
		virtual void Unbind() const override;

		//virtual void SetMat4(const std::string& _name, const Matrix4x4& _value) override;

	private:
		ComPtr<ID3D11HullShader> hullShader;
	};

	class D3D11DomainShader : public D3D11Shader
	{
	public:
		D3D11DomainShader(const Path& _filepath);
		D3D11DomainShader(const std::string& _src);

		virtual void Bind() const override;
		virtual void Unbind() const override;

		//virtual void SetMat4(const std::string& _name, const Matrix4x4& _value) override;

	private:
		ComPtr<ID3D11DomainShader> domainShader;
	};

	class D3D11GeometryShader : public D3D11Shader
	{
	public:
		D3D11GeometryShader(const Path& _filepath);
		D3D11GeometryShader(const std::string& _src);

		virtual void Bind() const override;
		virtual void Unbind() const override;

		//virtual void SetMat4(const std::string& _name, const Matrix4x4& _value) override;

	private:
		ComPtr<ID3D11GeometryShader> geometryShader;
	};
}


