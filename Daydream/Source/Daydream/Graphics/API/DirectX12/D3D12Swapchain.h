#pragma once

#include "Daydream/Graphics/Core/Swapchain.h"
#include "D3D12RenderDevice.h"
#include "D3D12TextureView.h"
#include "D3D12Framebuffer.h"
#include "D3D12RenderCommandList.h"


namespace Daydream
{
	class D3D12Swapchain : public Swapchain
	{
	public:
		D3D12Swapchain(D3D12RenderDevice* _device, const DaydreamWindow& _window, const SwapchainDesc& _desc);
		void CreateBackBufferView();
		virtual ~D3D12Swapchain() override;

		virtual void SetVSync(bool _enabled) override;
		virtual void Present() override;
		//virtual void ResizeSwapchain(UInt32 _width, UInt32 height) override;

		virtual void BeginFrame() override;
		virtual void EndFrame() override;

		virtual TextureView* GetCurrentRenderTargetView() const { return backBufferRTVs[frameIndex].get(); };
		virtual RenderCommandList* GetCurrentCommandList() const { return commandLists[frameIndex].get(); };

		void WaitForGPU();
		void MoveToNextFrame();

		UInt32 GetBackbufferIndex() { return frameIndex; }

		inline IDXGISwapChain3* GetDXGISwapchain() { return swapchain.Get(); }
	private:
		D3D12RenderDevice* device;
		ComPtr<IDXGISwapChain3> swapchain;
		Array<Shared<D3D12RenderCommandList>> commandLists;
		ID3D12GraphicsCommandList* currentCommandList;
		//Array<ComPtr<ID3D12GraphicsCommandList>> commandLists;
		//Array<ComPtr<ID3D12CommandAllocator>> commandAllocators;

		Array<Shared<D3D12GPUTexture>> backBufferTextures;
		Array<Shared<D3D12TextureView>> backBufferRTVs;

		Int32 frameIndex = 0;
		UInt32 bufferCount = 0;
		UInt64 currentFenceValue = 0;
		DXGI_FORMAT format;

		Array<ComPtr<ID3D12Resource>> d3d12Backbuffers;

		ComPtr<ID3D12Fence> fence;
		Array<UInt64> fenceValues;
		Wrappers::Event fenceEvent;

		HWND windowHandle;
	};
}