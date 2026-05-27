#include "DaydreamPCH.h"
#include "D3D12Buffer.h"

#include "D3D12Utility.h"

namespace Daydream
{
	D3D12GPUBuffer::D3D12GPUBuffer(D3D12RenderDevice* _device, const BufferDesc& _desc)
		:GPUBuffer(_desc)
	{
		device = _device;

		//D3D12_HEAP_PROPERTIES heapProperties = GraphicsUtility::DirectX12::ConvertToD3D12HeapProperties(_desc);
		D3D12MA::ALLOCATION_DESC allocationDesc = GraphicsUtility::DirectX12::ConvertToD3D12MemoryAllocationDesc(_desc);
		D3D12_RESOURCE_DESC resourceDesc= GraphicsUtility::DirectX12::ConvertToD3D12ResourceDesc(_desc);

		HRESULT hr = device->GetMemoryAllocator()->CreateResource(
			&allocationDesc,
			&resourceDesc,
			GraphicsUtility::DirectX12::ConvertToD3D12InitialState(_desc.memoryUsage),
			nullptr,
			allocation.GetAddressOf(),
			IID_PPV_ARGS(buffer.GetAddressOf()));

		if (_desc.memoryUsage == MemoryUsage::Dynamic || _desc.memoryUsage == MemoryUsage::Upload)
		{
			buffer->Map(0, nullptr, &mappedData);
		}
	}

	D3D12GPUBuffer::~D3D12GPUBuffer()
	{
		if (desc.memoryUsage == MemoryUsage::Dynamic || desc.memoryUsage == MemoryUsage::Upload)
		{
			buffer->Unmap(0, nullptr);
		}
	}

	void D3D12GPUBuffer::UpdateData(const void* _data, UInt32 _size)
	{
		memcpy(mappedData, _data, _size);
	}

	//D3D12VertexBuffer::D3D12VertexBuffer(D3D12RenderDevice* _device, MemoryUsage _usage, UInt32 _size, UInt32 _stride)
	//{
	//	device = _device;
	//	usage = _usage;
	//	bufferSize = _size;
	//	stride = _stride;

	//	if (usage == MemoryUsage::Static)
	//	{
	//		vertexBuffer = _device->CreateBuffer(_size, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST);
	//	}
	//	else if (usage == MemoryUsage::Dynamic)
	//	{
	//		vertexBuffer = _device->CreateBuffer(_size, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);

	//		HRESULT hr = vertexBuffer->Map(0, nullptr, &mappedData);
	//		DAYDREAM_CORE_ASSERT(SUCCEEDED(hr), "Failed to map uploadBuffer");

	//	}

	//	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	//	vertexBufferView.SizeInBytes = (UINT)_size;
	//	vertexBufferView.StrideInBytes = stride;
	//}

	//D3D12VertexBuffer::~D3D12VertexBuffer()
	//{
	//	
	//}


	//void D3D12VertexBuffer::Bind() const
	//{
	//	device->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	//}
	//void D3D12VertexBuffer::Unbind() const
	//{
	//}

	//void D3D12VertexBuffer::SetData(const void* _data, UInt32 _dataSize)
	//{
	//	memcpy(mappedData, _data, _dataSize);
	//}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//D3D12IndexBuffer::D3D12IndexBuffer(D3D12RenderDevice* _device, UInt32 _indexCount)
	//{
	//	device = _device;
	//	indexCount = _indexCount;

	//	UInt32 bufferSize = sizeof(UInt32) * _indexCount;
	//	indexBuffer = _device->CreateBuffer(bufferSize, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST);

	//	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	//	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	//	indexBufferView.SizeInBytes = sizeof(UInt32) * _indexCount;
	//}
	//void D3D12IndexBuffer::Bind() const
	//{
	//	device->GetCommandList()->IASetIndexBuffer(&indexBufferView);

	//}
	//void D3D12IndexBuffer::Unbind() const
	//{
	//}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//D3D12ConstantBuffer::D3D12ConstantBuffer(D3D12RenderDevice* _device, UInt32 _size)
	//{
	//	device = _device;
	//	
	//	constantBuffer = _device->CreateBuffer((_size + 255) & ~255, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);

	//	constantBufferView.BufferLocation = constantBuffer->GetGPUVirtualAddress();
	//	constantBufferView.SizeInBytes = (_size + 255) & ~255;//static_cast<UINT>(_size);

	//	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
	//	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	//	device->GetCBVSRVUAVHeapAlloc().Alloc(&cpuHandle, &gpuHandle);
	//	device->GetDevice()->CreateConstantBufferView(&constantBufferView, cpuHandle);

	//	constantBuffer->Map(0, nullptr, &mappedData);
	//}

	//D3D12ConstantBuffer::~D3D12ConstantBuffer()
	//{
	//
	//}

	//void D3D12ConstantBuffer::Update(const void* _data, UInt32 _size)
	//{
	//	memcpy(mappedData, _data, _size);
	//}

}