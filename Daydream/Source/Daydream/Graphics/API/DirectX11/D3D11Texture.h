#pragma once

#include "Daydream/Graphics/Resources/Texture/Texture.h"
#include "D3D11RenderDevice.h"
#include "D3D11Sampler.h"
#include "D3D11Common.h"

namespace Daydream
{
	class D3D11GPUTexture : public GPUTexture
	{
	public:
		D3D11GPUTexture(D3D11RenderDevice* _device, const TextureDesc& _desc);
		D3D11GPUTexture(D3D11RenderDevice* _device, const TextureDesc& _desc, ComPtr<ID3D11Texture2D> _d3d11BackBuffer);
		virtual ~D3D11GPUTexture() = default;

		//For Swapchain 
		void SetID3D11Resource(ComPtr<ID3D11Resource> _backBuffer) {};
		inline ID3D11Resource* GetID3D11Resource() const { return texture.Get(); }
	private:
		ComPtr<ID3D11Resource> texture;
	};

	//class D3D11Texture2D :public Texture2D
	//{
	//public:
	//	// constrcuter destructer
	//	D3D11Texture2D(D3D11RenderDevice* _device, const TextureDesc& _desc, const void* _initialData = nullptr);
	//	//D3D11Texture2D(D3D11RenderDevice* _device, const FilePath& _path, const TextureDesc& _desc);
	//	//D3D11Texture2D(D3D11RenderDevice* _device, ComPtr<ID3D11Texture2D> _texture);
	//	virtual ~D3D11Texture2D();

	//	virtual void SetSampler(Shared<Sampler> _sampler) override;
	//	virtual bool HasSampler() override { return textureSampler != nullptr; }

	//	inline virtual void* GetImGuiHandle() { return views.srv.Get(); }

	//	inline ID3D11Texture2D* GetID3D11Texture2D() { return texture.Get(); }

	//	inline ID3D11ShaderResourceView* GetSRV() { return views.srv.Get(); }
	//	inline ID3D11DepthStencilView* GetDSV() {return views.dsv.Get();}
	//	inline ID3D11RenderTargetView* GetRTV() {return views.rtv.Get();}
	//	inline ID3D11UnorderedAccessView* GetUAV() { return views.uav.Get(); };
	//	inline ID3D11SamplerState* GetSampler() { return textureSampler->GetSampler(); }
	//protected:

	//private:
	//	D3D11RenderDevice* device;
	//	D3D11Sampler* textureSampler = nullptr;
	//	D3D11ResourceViews views;
	//	ComPtr<ID3D11Texture2D> texture;
	//};
}

