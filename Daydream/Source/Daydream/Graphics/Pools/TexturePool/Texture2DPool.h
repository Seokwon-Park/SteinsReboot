#pragma once

#include "Texture2DPoolKey.h"
#include "Texture2DPoolHandle.h"
#include "Daydream/Graphics/Pools/IResourcePool.h"


namespace Daydream
{

	class Texture2DPool : public IResourcePool
	{
	public:
		Texture2DPool();
		virtual ~Texture2DPool();

		Texture2DPoolHandle RequestTexture2DHandle(UInt32 _width, UInt32 _height, RenderFormat _format);
		void ReturnTexture2DHandle(Texture2DPoolHandle&& _handle, UInt32 _lastUsedLoop);

		virtual void FlushInFlight(UInt32 _currentLoop, UInt32 _maxFramesInFlight) override;
		virtual void CleanUp(UInt32 _currentLoop, UInt32 _removeLimit) override;
	protected:

	private:
		Queue<Texture2DPoolHandle> pendingQueue;
		HashMap<Texture2DPoolKey, Queue<Texture2DPoolHandle>> freeQueue;

		
	};
}



