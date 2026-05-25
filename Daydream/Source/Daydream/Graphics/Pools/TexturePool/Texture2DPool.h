#pragma once

#include "Texture2DPoolKey.h"
#include "Texture2DPoolHandle.h"


namespace Daydream
{

	class Texture2DPool
	{
	public:
		Texture2DPool();
		~Texture2DPool();

		Texture2DPoolHandle RequestTexture2DHandle(UInt32 _width, UInt32 _height, RenderFormat _format);
		void ReturnTexture2DHandle(Texture2DPoolHandle&& _handle, UInt32 _lastUsedLoop);
		void CleanUp(UInt64 _thresholdLoop);
	protected:

	private:
		HashMap<Texture2DPoolKey, Queue<Texture2DPoolHandle>> pool;
	};
}



