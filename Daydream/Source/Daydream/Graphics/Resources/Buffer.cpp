#include "DaydreamPCH.h"
#include "Daydream/Graphics/Resources/Buffer.h"
#include "Daydream/Graphics/Core/RenderDevice.h"

#include "Daydream/Graphics/Core/Renderer.h"
#include "Daydream/Graphics/Core/RenderContext.h"

namespace Daydream
{
	GPUBuffer::GPUBuffer(const BufferDesc& _desc)
	{
		desc = _desc;
	}

	VertexBuffer::VertexBuffer(Shared<GPUBuffer> _buffer, UInt32 _stride)
		:Buffer(_buffer), stride(_stride)
	{

	}

	Shared<VertexBuffer> VertexBuffer::CreateDynamic(UInt32 _size, UInt32 _stride, const void* _initialData, UInt32 _initialDataSize)
	{
		BufferDesc desc{};
		desc.bufferUsage = BufferUsage::Vertex;
		desc.memoryUsage = MemoryUsage::Dynamic;
		desc.size = _size;

		Shared<GPUBuffer> gpuBuffer = Renderer::GetRenderDevice()->CreateGPUBuffer(desc);
		Shared<VertexBuffer> vertexBuffer = MakeShared<VertexBuffer>(gpuBuffer, _stride);

		if (_initialData != nullptr && _initialDataSize > 0)
			vertexBuffer->UpdateData(_initialData, _initialDataSize);

		return vertexBuffer;
	}
	Shared<VertexBuffer> VertexBuffer::CreateStatic(UInt32 _size, UInt32 _stride, const void* _initialData)
	{
		DAYDREAM_CORE_ASSERT(_initialData, "Static Vertex Buffer must have initial data!");

		BufferDesc desc{};
		desc.bufferUsage = BufferUsage::Vertex;
		desc.memoryUsage = MemoryUsage::Static;
		desc.size = _size;

		Shared<GPUBuffer> gpuBuffer = Renderer::GetRenderDevice()->CreateGPUBuffer(desc);
		Shared<VertexBuffer> vertexBuffer = MakeShared<VertexBuffer>(gpuBuffer, _stride);
		Shared<UploadBuffer> uploadBuffer = Renderer::GetUploadBufferPool()->RequestBuffer(_size);
		uploadBuffer->UpdateData(_initialData, _size);

		//참조의 경우 uploadBuffer가 이 함수를 벗어나면서 파괴되기 때문에 mutable
		Renderer::EnqueuePreFrameCommand([=]()
			{
				Renderer::CopyBuffer(uploadBuffer.get(), vertexBuffer.get(), _size, 0, 0);
				Renderer::TransitionBufferState(vertexBuffer, ResourceState::CopyDest, ResourceState::VertexBuffer);

			});

		Renderer::GetUploadBufferPool()->ReturnResource(desc.size, std::move(uploadBuffer));

		return vertexBuffer;
	}

	IndexBuffer::IndexBuffer(Shared<GPUBuffer> _buffer, UInt32 _indexCount)
		:Buffer(_buffer), indexCount(_indexCount)
	{
	}

	Shared<IndexBuffer> IndexBuffer::Create(const UInt32* _indices, UInt32 _count)
	{
		BufferDesc desc{};
		desc.bufferUsage = BufferUsage::Index;
		desc.memoryUsage = MemoryUsage::Static;
		desc.size = sizeof(UInt32) * _count;

		Shared<GPUBuffer> gpuBuffer = Renderer::GetRenderDevice()->CreateGPUBuffer(desc);
		Shared<IndexBuffer> indexBuffer = MakeShared<IndexBuffer>(gpuBuffer, _count);
		Shared<UploadBuffer> uploadBuffer = Renderer::GetUploadBufferPool()->RequestBuffer(desc.size);
		uploadBuffer->UpdateData(_indices, desc.size);

		Renderer::EnqueuePreFrameCommand([=]()
			{
				Renderer::CopyBuffer(uploadBuffer.get(), indexBuffer.get(), desc.size, 0, 0);
				Renderer::TransitionBufferState(indexBuffer, ResourceState::CopyDest, ResourceState::IndexBuffer);
			}
		);

		Renderer::GetUploadBufferPool()->ReturnResource(desc.size, std::move(uploadBuffer));

		return indexBuffer;
	}

	ConstantBuffer::ConstantBuffer(Shared<GPUBuffer> _buffer)
		:Buffer(_buffer)
	{
	}

	Shared<ConstantBuffer> ConstantBuffer::Create(UInt32 _size)
	{
		BufferDesc desc{};
		desc.bufferUsage = BufferUsage::Constant;
		desc.memoryUsage = MemoryUsage::Dynamic;
		desc.size = _size;

		Shared<GPUBuffer> gpuBuffer = Renderer::GetRenderDevice()->CreateGPUBuffer(desc);
		Shared<ConstantBuffer> constantBuffer = MakeShared<ConstantBuffer>(gpuBuffer);
		return constantBuffer;
	}

	UploadBuffer::UploadBuffer(Shared<GPUBuffer> _buffer)
		:Buffer(_buffer)
	{

	}

	UploadBuffer::~UploadBuffer()
	{
	}

	Shared<UploadBuffer> UploadBuffer::Create(UInt32 _size)
	{
		BufferDesc desc{};
		desc.bufferUsage = BufferUsage::None;
		desc.memoryUsage = MemoryUsage::Upload;
		desc.size = _size;

		Shared<GPUBuffer> gpuBuffer = Renderer::GetRenderDevice()->CreateGPUBuffer(desc);
		Shared<UploadBuffer> uploadBuffer = MakeShared<UploadBuffer>(gpuBuffer);
		return uploadBuffer;
	}
}

