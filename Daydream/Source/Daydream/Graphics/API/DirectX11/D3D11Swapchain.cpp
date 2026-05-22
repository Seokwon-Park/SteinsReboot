#include "DaydreamPCH.h"
#include "D3D11Swapchain.h"

#include "Daydream/Core/Window.h"

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "D3D11Utility.h"

namespace Daydream
{

	D3D11Swapchain::D3D11Swapchain(D3D11RenderDevice* _device, const DaydreamWindow& _window, const SwapchainDesc& _desc)
		:Swapchain(_desc)
	{
		device = _device;
		DAYDREAM_CORE_ASSERT(device, "Device is nullptr");

		DXGI_SAMPLE_DESC sampleDesc = {};
		sampleDesc.Count = 1;    // 샘플 수 (1이면 MSAA 비활성화)
		sampleDesc.Quality = 0;  // 품질 레벨 (0이면 기본값)

		DXGI_MODE_DESC bufferDesc;
		bufferDesc.Width = _desc.width;
		bufferDesc.Height = _desc.height;
		//bufferDesc.RefreshRate 
		//bufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
		//bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // 프로그레시브로 설정
		bufferDesc.Format = GraphicsUtility::DirectX::ConvertToDXGIFormat(_desc.format);

		DXGI_SWAP_CHAIN_DESC swapchainDesc;
		swapchainDesc.BufferDesc = bufferDesc;
		swapchainDesc.SampleDesc = sampleDesc;
		swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapchainDesc.BufferCount = _desc.imageCount;
		swapchainDesc.OutputWindow = glfwGetWin32Window((GLFWwindow*)_window.GetNativeWindow());
		swapchainDesc.Windowed = true;
		swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		HRESULT hr = device->GetFactory()->CreateSwapChain(device->GetDevice(), &swapchainDesc, swapchain.GetAddressOf());
		DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed To Create Swapchain!");

		CreateBackBufferView();
	}

	D3D11Swapchain::~D3D11Swapchain()
	{
		device = nullptr;
	}
	void D3D11Swapchain::SetVSync(bool _enabled)
	{
	}
	void D3D11Swapchain::Present()
	{
		swapchain->Present(desc.isVSync, 0);
	}

	//void D3D11Swapchain::ResizeSwapchain(UInt32 _width, UInt32 _height)
	//{
	//	framebuffer.reset();
	//	swapChain->ResizeBuffers(0, _width, _height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	//	framebuffer = MakeShared<D3D11Framebuffer>(device, mainRenderPass.get(), this);
	//}

	void D3D11Swapchain::BeginFrame()
	{
		if (isSwapchainResized)
		{
			//이전 백버퍼를 붙잡고 있던 렌더타겟뷰, 텍스쳐배열을 놓아줘야 리사이즈 할 수 있음
			backBufferRTV = nullptr;
			backBufferTexture = nullptr;

			swapchain->ResizeBuffers(0, desc.width, desc.height, GraphicsUtility::DirectX::ConvertToDXGIFormat(desc.format), 0);
			
			CreateBackBufferView();
			isSwapchainResized = false;
		}

		//Array<ID3D11RenderTargetView*> rtvs = framebuffer->GetRenderTargetViews();
		//for (auto rtv : rtvs)
		//{
		//	device->GetContext()->ClearRenderTargetView(rtv, mainRenderPass->GetClearColor().color);
		//}
		//if (framebuffer->HasDepthAttachment())
		//{
		//	device->GetContext()->ClearDepthStencilView(framebuffer->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		//	device->GetContext()->OMSetRenderTargets((UInt32)rtvs.size(), rtvs.data(), framebuffer->GetDepthStencilView());
		//}
		//else
		//{
		//	device->GetContext()->OMSetRenderTargets((UInt32)rtvs.size(), rtvs.data(), nullptr);
		//}
	}

	void D3D11Swapchain::EndFrame()
	{
		//device->GetContext()->OMSetRenderTargets(0, nullptr, nullptr);
	}

	void D3D11Swapchain::CreateBackBufferView()
	{
		ComPtr<ID3D11Texture2D> backBuffer;
		swapchain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));

		TextureDesc textureDesc{};
		textureDesc.width = desc.width;
		textureDesc.height = desc.height;
		textureDesc.mipLevels = 1;
		textureDesc.sampleCount = 1;
		textureDesc.format = desc.format;
		textureDesc.textureUsage = TextureUsage::RenderTarget;
		textureDesc.type = TextureType::Texture2D;

		backBufferTexture = MakeShared<D3D11GPUTexture>(device, textureDesc, backBuffer);

		TextureViewDesc viewDesc;
		viewDesc.format = desc.format;
		viewDesc.type = TextureViewType::RenderTarget;

		backBufferRTV = MakeShared<D3D11TextureView>(device, backBufferTexture.get(), viewDesc);
	}

}