#pragma once

#include "D3D11Common.h"
#include "D3D11RenderDevice.h"
#include "D3D11Texture.h"
#include "Daydream/Graphics/Resources/Texture/TextureView.h"

namespace Daydream
{
	class D3D11TextureView : public TextureView
	{
	public:
		D3D11TextureView(D3D11RenderDevice* _device, D3D11GPUTexture* _texture, const TextureViewDesc& _desc);
		virtual ~D3D11TextureView();

		virtual void* GetUIHandle() const override { return view.Get(); }

		inline ID3D11ShaderResourceView* GetSRV()
		{
			DAYDREAM_CORE_ASSERT(desc.type == TextureViewType::ShaderResource, "View is not SRV!");
			return static_cast<ID3D11ShaderResourceView*>(view.Get());
		}

		inline ID3D11DepthStencilView* GetDSV()
		{
			DAYDREAM_CORE_ASSERT(desc.type == TextureViewType::DepthStencil, "View is not DSV!");
			return static_cast<ID3D11DepthStencilView*>(view.Get());
		}
		inline ID3D11RenderTargetView* GetRTV()
		{
			DAYDREAM_CORE_ASSERT(desc.type == TextureViewType::RenderTarget, "View is not RTV!");
			return static_cast<ID3D11RenderTargetView*>(view.Get());
		}

		inline ID3D11UnorderedAccessView* GetUAV()
		{
			DAYDREAM_CORE_ASSERT(desc.type == TextureViewType::UnorderedAccess, "View is not UAV!");
			return static_cast<ID3D11UnorderedAccessView*>(view.Get());
		}

	protected:

	private:

	private:
		ComPtr<ID3D11View> view;
	};
}
