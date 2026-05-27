#pragma once

#include "Daydream/Graphics/Resources/Buffer.h"
#include "Daydream/Graphics/Pools/IResourcePool.h"
#include "UploadBufferHandle.h"

namespace Daydream
{
	class UploadBufferPool : public IResourcePool
	{
	public:
		UploadBufferPool();
		virtual ~UploadBufferPool();

		UploadBufferHandle RequestUploadBuffer(UInt32 _size);
		void ReturnBuffer(UploadBufferHandle&& _buffer, UInt32 _lastUsedLoop);

		void FlushInFlight(UInt32 _currentLoop, UInt32 _maxFramesInFlight) override;
		void CleanUp(UInt32 _currentLoop, UInt32 _removeLimit) override;
	protected:

	private:
		Queue<UploadBufferHandle> pendingQueue;
		HashMap<UInt32, Queue<UploadBufferHandle>> freeQueue;

		// Inherited via IResourcePool

	};
}
