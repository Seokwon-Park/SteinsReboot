#pragma once

#include "RenderTargetPoolKey.h"
#include "RenderTargetPoolHandle.h"


namespace Daydream
{

	class RenderTargetPool
	{
	public:
		RenderTargetPool();
		~RenderTargetPool();

		RenderTargetPoolHandle RequestRenderTargetView(UInt32 _width, UInt32 _height, RenderFormat _format);
		void ReleaseRenderTarget(RenderTargetPoolHandle&& _renderTargetHandle, UInt32 _lastUsedLoop);
		void UpdatePool(UInt64 _thresholdLoop);
	protected:

	private:
		HashMap<RenderTargetPoolKey, Queue<RenderTargetPoolHandle>> pool;
	};
}



