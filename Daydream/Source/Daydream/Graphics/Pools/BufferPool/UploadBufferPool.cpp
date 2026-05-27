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
	UploadBufferHandle UploadBufferPool::RequestUploadBuffer(UInt32 _size)
	{
		UploadBufferHandle handle;
		if (!freeQueue[_size].empty())
		{
			handle = std::move(freeQueue[_size].front());
			freeQueue[_size].pop();
		}
		else
		{
			handle.buffer = UploadBuffer::Create(_size);
		}

		return handle;
	}

	void UploadBufferPool::ReturnBuffer(UploadBufferHandle&& _handle, UInt32 _lastUsedLoop)
	{
		_handle.lastUsedLoop = _lastUsedLoop;
		pendingQueue.push(std::move(_handle));
	}
	void UploadBufferPool::FlushInFlight(UInt32 _currentLoop, UInt32 _maxFramesInFlight)
	{
		while (!pendingQueue.empty() && pendingQueue.front().lastUsedLoop + _maxFramesInFlight <= _currentLoop)
		{
			pendingQueue.front().lastUsedLoop = _currentLoop;

			freeQueue[pendingQueue.front().buffer->GetSize()].push(std::move(pendingQueue.front()));
			pendingQueue.pop();
		}
	}
	void UploadBufferPool::CleanUp(UInt32 _currentLoop, UInt32 _removeLimit)
	{
		for (auto& [key, handles] : freeQueue)
		{
			while (!handles.empty() && handles.front().lastUsedLoop + _removeLimit <= _currentLoop)
			{
				handles.pop();
			}
		}
	}
}

