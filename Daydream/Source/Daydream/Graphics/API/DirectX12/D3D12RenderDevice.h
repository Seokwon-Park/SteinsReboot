#pragma once

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
		//virtual Shared<VertexBuffer> CreateDynamicVertexBuffer(UInt32 _size, UInt32 _stride, UInt32 _initialDataSize = 0, const void* _initialData = nullptr) override;
		//virtual Shared<VertexBuffer> CreateStaticVertexBuffer(UInt32 _size, UInt32 _stride, const void* _initialData) override;
		//virtual Shared<IndexBuffer> CreateIndexBuffer(const UInt32 * _indices, UInt32 _count) override;
		//virtual Shared<ConstantBuffer> CreateConstantBuffer(UInt32 _size) override;
		//virtual Shared<RenderPass> CreateRenderPass(const RenderPassDesc& _desc) override;
		//virtual Shared<Framebuffer> CreateFramebuffer(Shared<RenderPass> _renderPass, const FramebufferDesc& _desc) override;
		virtual Shared<GraphicsPipelineState> CreatePipelineState(const GraphicsPipelineStateDesc& _desc)override;
		virtual Shared<Shader> CreateShader(const std::string& _src, const ShaderType& _type, ShaderLoadMode _mode) override;
		virtual Shared<Swapchain> CreateSwapchain(const DaydreamWindow& _window, const SwapchainDesc& _desc)override;
		//virtual Shared<Texture2D> CreateTexture2D(const void* _imageData, const TextureDesc& _desc)override;
		//virtual Shared<Texture2D> CreateEmptyTexture2D(const TextureDesc& _desc)override;
		//virtual Shared<TextureCube> CreateTextureCube(Array<const void*>& _imagePixels, const TextureDesc& _desc)override;
		//virtual Shared<TextureCube> CreateEmptyTextureCube(const TextureDesc& _desc) override;
		virtual Shared<Sampler> CreateSampler(const SamplerDesc& _desc) override;
		virtual Unique<ImGuiRenderer> CreateImGuiRenderer() override;


		ID3D12Device* GetDevice() const { return device.Get(); }
		ID3D12CommandQueue* GetCommandQueue() const { return commandQueue.Get(); }
		void SetCommandList(ComPtr<ID3D12GraphicsCommandList> _commandList) { commandList = _commandList; }
		ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }
		ID3D12DescriptorHeap* GetRTVHeap() const { return rtvHeap.Get(); }
		ID3D12DescriptorHeap* GetDSVHeap() const { return dsvHeap.Get(); }
		ID3D12DescriptorHeap* GetSamplerHeap() const { return samplerHeap.Get(); }
		ID3D12DescriptorHeap* GetCBVSRVUAVHeap() const { return cbvSrvUavHeap.Get(); }
		ID3D12DescriptorHeap* GetDynamicSamplerHeap() const { return dynamicSamplerHeap.Get(); }
		ID3D12DescriptorHeap* GetDynamicCBVSRVUAVHeap() const { return dynamicCbvSrvUavHeap.Get(); }

		DescriptorHeapAllocator& GetRTVHeapAlloc() { return rtvHeapAlloc; }
		DescriptorHeapAllocator& GetDSVHeapAlloc() { return dsvHeapAlloc; }
		DescriptorHeapAllocator& GetSamplerHeapAlloc() { return samplerHeapAlloc; }
		DescriptorHeapAllocator& GetCBVSRVUAVHeapAlloc() { return cbvSrvUavHeapAlloc; }
		DynamicDescriptorHeapAllocator& GetDynamicSamplerHeapAlloc() { return dynamicSamplerHeapAlloc; }
		DynamicDescriptorHeapAllocator& GetDynamicCBVSRVUAVHeapAlloc() { return dynamicCbvSrvUavHeapAlloc; }
		IDXGIFactory7* GetFactory() const { return dxgiFactory.Get(); }

		void ExecuteSingleTimeCommands(std::function<void(ID3D12GraphicsCommandList*)> commands);
		ComPtr<ID3D12Resource> CreateBuffer(UINT64 _size,
			D3D12_HEAP_TYPE _heapType,
			D3D12_RESOURCE_STATES _initialState,
			D3D12_RESOURCE_FLAGS _flags = D3D12_RESOURCE_FLAG_NONE);
		void CopyBuffer(ID3D12Resource* _src, ID3D12Resource* _dst, UInt32 _dataSize);

		ComPtr<ID3D12Resource> CreateTexture(
			const D3D12_RESOURCE_DESC& _desc,
			D3D12_RESOURCE_STATES _initialState
		);

		void CopyBufferToImage(ID3D12Resource* _src, ID3D12Resource* _dst, Array<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> _subresourceFootprint);

		void TransitionResourceState(
			ID3D12GraphicsCommandList* _commandList,
			ID3D12Resource* _resource,
			D3D12_RESOURCE_STATES _stateBefore,
			D3D12_RESOURCE_STATES _stateAfter);
		void TransitionResourceStateImmediate(D3D12_RESOURCE_BARRIER _barrier);


		//void WaitForGPU(IDXGISwapChain3* _swapChain);

	private:
		ComPtr<ID3D12Device> device;
		ComPtr<ID3D12CommandQueue> commandQueue;
		ComPtr<ID3D12CommandAllocator> allocator;

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

		ComPtr<ID3D12CommandAllocator> uploadCommandAllocator;
		ComPtr<ID3D12GraphicsCommandList> uploadCommandList;
		ComPtr<ID3D12Fence> uploadFence;
		UInt64 uploadFenceValue;
		HANDLE uploadFenceEvent;


	};
}

