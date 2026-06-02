#pragma once

#include "D3D12MemAlloc.h"
#include "Daydream/Graphics/Core/RenderDevice.h"
#include "D3D12HeapAllocator.h"

namespace Daydream
{
	class D3D12RenderDevice : public RenderDevice
	{
	public:
		D3D12RenderDevice();
		virtual ~D3D12RenderDevice();

		virtual void Init() override;
		virtual void Shutdown() override;

		virtual Unique<RenderContext> CreateContext() override;
		virtual Shared<RenderCommandList> CreateRenderCommandList() override;
		virtual Shared<GPUBuffer> CreateGPUBuffer(const BufferDesc& _desc) override;
		virtual Shared<GPUTexture> CreateGPUTexture(const TextureDesc& _desc) override;
		virtual Shared<TextureView> CreateTextureView(Texture* _texture, const TextureViewDesc& _desc) override;
		virtual Shared<GraphicsPipelineState> CreatePipelineState(const GraphicsPipelineStateDesc& _desc)override;
		virtual Shared<Shader> CreateShader(const std::string& _src, const ShaderType& _type, ShaderLoadMode _mode) override;
		virtual Shared<Swapchain> CreateSwapchain(const DaydreamWindow& _window, const SwapchainDesc& _desc)override;
		virtual Shared<Sampler> CreateSampler(const SamplerDesc& _desc) override;
		virtual Unique<ImGuiRenderer> CreateImGuiRenderer() override;

		virtual UInt32 GetAlignedRowPitch(UInt32 _width, RenderFormat _format) const override; 

		ID3D12Device* GetDevice() const { return device.Get(); }
		ID3D12CommandQueue* GetCommandQueue() const { return commandQueue.Get(); }

		D3D12MA::Allocator* GetMemoryAllocator() const { return memoryAllocator.Get(); }
		
		ID3D12DescriptorHeap* GetRTVHeap() const { return rtvHeap.Get(); }
		ID3D12DescriptorHeap* GetDSVHeap() const { return dsvHeap.Get(); }
		ID3D12DescriptorHeap* GetSamplerHeap() const { return samplerHeap.Get(); }
		ID3D12DescriptorHeap* GetCBVSRVUAVHeap() const { return cbvSrvUavHeap.Get(); }

		DescriptorHeapAllocator& GetRTVHeapAlloc() { return rtvHeapAlloc; }
		DescriptorHeapAllocator& GetDSVHeapAlloc() { return dsvHeapAlloc; }
		DescriptorHeapAllocator& GetSamplerHeapAlloc() { return samplerHeapAlloc; }
		DescriptorHeapAllocator& GetCBVSRVUAVHeapAlloc() { return cbvSrvUavHeapAlloc; }
		DynamicDescriptorHeapAllocator& GetDynamicSamplerHeapAlloc() { return dynamicSamplerHeapAlloc; }
		DynamicDescriptorHeapAllocator& GetDynamicCBVSRVUAVHeapAlloc() { return dynamicCbvSrvUavHeapAlloc; }
		IDXGIFactory7* GetFactory() const { return dxgiFactory.Get(); }


		//void WaitForGPU(IDXGISwapChain3* _swapChain);

	private:

		ComPtr<ID3D12Device> device;
		ComPtr<ID3D12CommandQueue> commandQueue;

		ComPtr<ID3D12RootSignature> rootSignature;
		ComPtr<ID3D12DescriptorHeap> rtvHeap;
		ComPtr<ID3D12DescriptorHeap> dsvHeap;
		ComPtr<ID3D12DescriptorHeap> samplerHeap;
		ComPtr<ID3D12DescriptorHeap> cbvSrvUavHeap;

		ComPtr<ID3D12DescriptorHeap> dynamicSamplerHeap;
		ComPtr<ID3D12DescriptorHeap> dynamicCbvSrvUavHeap;

		ComPtr<ID3D12PipelineState> pipelineState;
		ComPtr<ID3D12GraphicsCommandList> commandList;

		DescriptorHeapAllocator rtvHeapAlloc;
		DescriptorHeapAllocator dsvHeapAlloc;
		DescriptorHeapAllocator samplerHeapAlloc;
		DescriptorHeapAllocator cbvSrvUavHeapAlloc;

		DynamicDescriptorHeapAllocator dynamicSamplerHeapAlloc;
		DynamicDescriptorHeapAllocator dynamicCbvSrvUavHeapAlloc;

		ComPtr<IDXGIFactory7> dxgiFactory;
		ComPtr<IDXGIAdapter4> dxgiAdapter;
		ComPtr<ID3D12Debug> debugLayer;

		ComPtr<D3D12MA::Allocator> memoryAllocator;

		ComPtr<ID3D12CommandAllocator> uploadCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> uploadCommandList;
		ComPtr<ID3D12Fence> uploadFence;
		UInt64 uploadFenceValue;
		HANDLE uploadFenceEvent;


	};
}

