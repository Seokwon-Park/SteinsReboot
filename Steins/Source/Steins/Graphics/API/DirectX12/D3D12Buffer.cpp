#include "SteinsPCH.h"
#include "D3D12Buffer.h"

namespace Steins
{
	// TODO: commandlist 생성전에 배리어 명령 내리는 문제 수정할 것
	D3D12VertexBuffer::D3D12VertexBuffer(D3D12RenderDevice* _device, UInt32 _bufferSize, UInt32 _stride)
	{
		device = _device;
		stride = _stride;

		D3D12_HEAP_PROPERTIES props{};
		props.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = _bufferSize;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		HRESULT hr = device->GetDevice()->CreateCommittedResource(&props,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(vertexBuffer.GetAddressOf())
		);

		// 3. Vertex Buffer View 생성
		vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = _bufferSize;
		vertexBufferView.StrideInBytes = stride;
	}
	D3D12VertexBuffer::D3D12VertexBuffer(D3D12RenderDevice* _device, void* _vertices, UInt32 _size, UInt32 _stride)
	{
		device = _device;
		stride = _stride;
		D3D12_HEAP_PROPERTIES props{};
		props.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = _size;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		device->GetDevice()->CreateCommittedResource(&props,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.GetAddressOf())
		);

		props.Type = D3D12_HEAP_TYPE_DEFAULT;

		device->GetDevice()->CreateCommittedResource(&props,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(vertexBuffer.GetAddressOf())
		);

		void* data;
		HRESULT hr = uploadBuffer->Map(0, nullptr, &data);
		STEINS_CORE_ASSERT(SUCCEEDED(hr), "Failed to map uploadBuffer");
		memcpy(data, _vertices, _size);
		uploadBuffer->Unmap(0, nullptr);

		device->GetCommandList()->CopyBufferRegion(vertexBuffer.Get(), 0, uploadBuffer.Get(), 0, _size);

		D3D12_RESOURCE_BARRIER barrier;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = vertexBuffer.Get(); // 전이시킬 리소스
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		device->GetCommandList()->ResourceBarrier(1, &barrier);

		// (4) VB 뷰 생성
		vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = (UINT)_size;
		vertexBufferView.StrideInBytes = stride;
	}
	void D3D12VertexBuffer::Bind() const
	{
		device->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	}
	void D3D12VertexBuffer::Unbind() const
	{
	}

	void D3D12VertexBuffer::SetData(void* _data, UInt32 _dataSize)
	{
		void* data;
		HRESULT hr = vertexBuffer->Map(0, nullptr, &data);
		STEINS_CORE_ASSERT(SUCCEEDED(hr), "Failed to map uploadBuffer");
		memcpy(data, _data, _dataSize);
		vertexBuffer->Unmap(0, nullptr);

	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	D3D12IndexBuffer::D3D12IndexBuffer(D3D12RenderDevice* _device, UInt32* _indices, UInt32 _indexCount)
	{
		indexCount = _indexCount;

		device = _device;
		D3D12_HEAP_PROPERTIES props{};
		props.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = sizeof(UInt32) * _indexCount;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		device->GetDevice()->CreateCommittedResource(&props,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.GetAddressOf())
		);

		props.Type = D3D12_HEAP_TYPE_DEFAULT;

		device->GetDevice()->CreateCommittedResource(&props,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(indexBuffer.GetAddressOf())
		);

		void* data;
		uploadBuffer.Get()->Map(0, nullptr, &data);
		memcpy(data, _indices, sizeof(UInt32) * _indexCount);
		uploadBuffer.Get()->Unmap(0, nullptr);

		device->GetCommandList()->CopyBufferRegion(indexBuffer.Get(), 0, uploadBuffer.Get(), 0, sizeof(UInt32) * _indexCount);

		D3D12_RESOURCE_BARRIER barrier;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = indexBuffer.Get(); // 전이시킬 리소스
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_INDEX_BUFFER;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		device->GetCommandList()->ResourceBarrier(1, &barrier);

		// (4) VB 뷰 생성
		indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		indexBufferView.SizeInBytes = sizeof(UInt32) * _indexCount;
	}
	void D3D12IndexBuffer::Bind() const
	{
		device->GetCommandList()->IASetIndexBuffer(&indexBufferView);

	}
	void D3D12IndexBuffer::Unbind() const
	{
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	D3D12ConstantBuffer::D3D12ConstantBuffer(D3D12RenderDevice* _device, UInt32 _size)
	{
		device = _device;
		D3D12_HEAP_PROPERTIES props{};
		props.Type = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC desc{};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Alignment = 0;
		desc.Width = (_size + 255) & ~255;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		device->GetDevice()->CreateCommittedResource(
			&props,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(constantBuffer.GetAddressOf())
		);

		constantBufferView.BufferLocation = constantBuffer->GetGPUVirtualAddress();
		constantBufferView.SizeInBytes = (_size + 255) & ~255;//static_cast<UINT>(_size);

		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
		device->GetCBVSRVUAVHeapAlloc().Alloc(&cpuHandle, &gpuHandle);
		device->GetDevice()->CreateConstantBufferView(&constantBufferView, cpuHandle);
	}
	void D3D12ConstantBuffer::Update(const void* _data, UInt32 _size)
	{
		void* data;
		constantBuffer->Map(0, nullptr, &data);
		memcpy(data, _data, _size);
		constantBuffer->Unmap(0, nullptr);

	}
}