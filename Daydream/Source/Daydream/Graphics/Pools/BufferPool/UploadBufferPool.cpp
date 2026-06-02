#include "DaydreamPCH.h"
#include "UploadBufferPool.h"

namespace Daydream
{
	UploadBufferPool::UploadBufferPool()
	{
	}

	UploadBufferPool::~UploadBufferPool()
	{
	}
	Shared<UploadBuffer> UploadBufferPool::RequestBuffer(UInt32 _size)
	{
		Shared<UploadBuffer> buffer;
		if (!freeQueue[_size].empty())
		{
			buffer = std::move(freeQueue[_size].front().payload);
			freeQueue[_size].pop();
		}
		else
		{
			buffer = UploadBuffer::Create(_size);
		}

		return buffer;
	}
}

