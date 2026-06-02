#include "DaydreamPCH.h"
#include "ConstantBufferPool.h"

namespace Daydream
{
	ConstantBufferPool::ConstantBufferPool()
	{
	}

	ConstantBufferPool::~ConstantBufferPool()
	{
	}

	Shared<ConstantBuffer> ConstantBufferPool::RequestBuffer(UInt32 _size)
	{
		Shared<ConstantBuffer> buffer;
		if (!freeQueue[_size].empty())
		{
			buffer = std::move(freeQueue[_size].front().payload);
			freeQueue[_size].pop();
		}
		else
		{
			buffer = ConstantBuffer::Create(_size);
		}

		return buffer;
	}
}

