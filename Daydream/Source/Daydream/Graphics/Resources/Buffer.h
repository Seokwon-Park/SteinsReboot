#pragma once

#include "Daydream/Enum/RendererEnums.h"
#include "GPUResource.h"

namespace Daydream
{
	struct BufferDesc
	{
		UInt64 size = 0;
		BufferUsage bufferUsage = BufferUsage::None;
		MemoryUsage memoryUsage = MemoryUsage::Static;
	};


	class GPUBuffer : public GPUResource
	{
	public:
		GPUBuffer(const BufferDesc& _desc);
		virtual ~GPUBuffer() {}

		virtual void UpdateData(const void* _data, UInt32 _size) = 0;

		inline UInt64 GetSize() const { return desc.size; }

		const BufferDesc& GetDesc() const { return desc; }
	protected:
		BufferDesc desc;
		void* mappedData;
	};

	class Buffer
	{
	public:
		Buffer(Shared<GPUBuffer> _buffer) : buffer(_buffer) {}
		virtual ~Buffer() = default;

		inline void UpdateData(const void* _data, UInt32 _size) { buffer->UpdateData(_data, _size); }
		inline GPUBuffer* GetGPUBuffer() const { return buffer.get(); }
		inline UInt64 GetSize() const { return buffer->GetSize(); }
	protected:
		Shared<GPUBuffer> buffer;
	};


	class VertexBuffer : public Buffer
	{
	public:
		VertexBuffer(Shared<GPUBuffer> _buffer, UInt32 _stride);
		~VertexBuffer() {}

		inline UInt32 GetStride() const { return stride; };

		static Shared<VertexBuffer> CreateDynamic(UInt32 _size, UInt32 _stride, const void* _initialData = nullptr, UInt32 _initialDataSize = 0);
		static Shared<VertexBuffer> CreateStatic(UInt32 _size, UInt32 _stride, const void* _initialData);
	private:
		UInt32 stride;
	};

	class IndexBuffer : public Buffer
	{
	public:
		IndexBuffer(Shared<GPUBuffer> _buffer, UInt32 _indexCount);
		~IndexBuffer() {}

		UInt32 GetIndexCount() const { return indexCount; }

		static Shared<IndexBuffer> Create(const UInt32* _indices, UInt32 _count);
	private:
		UInt32 indexCount;

	};

	class ConstantBuffer : public Buffer
	{
	public:
		ConstantBuffer(Shared<GPUBuffer> _buffer);
		~ConstantBuffer() {}

		static Shared<ConstantBuffer> Create(UInt32 _size);
		template <typename T>
		static Shared<ConstantBuffer> Create() { return Create(sizeof(T)); }
	protected:
	};

	class UploadBuffer : public Buffer
	{
	public:
		UploadBuffer(Shared<GPUBuffer> _buffer);
		~UploadBuffer();

		static Shared<UploadBuffer> Create(UInt32 _size);
	protected:
	};

	class ReadbackBuffer : public Buffer
	{
	public:
		ReadbackBuffer(Shared<GPUBuffer> _buffer);
		~ReadbackBuffer();

		static Shared<ReadbackBuffer> Create(UInt32 _size);
	protected:
	};

}

