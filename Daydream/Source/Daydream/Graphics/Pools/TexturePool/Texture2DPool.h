#pragma once

#include "Texture2DPoolKey.h"
#include "Texture2DAllocation.h"

#include "Daydream/Graphics/Pools/ResourcePoolBase.h"

namespace Daydream
{
	class Texture2DPool : public ResourcePoolBase<Texture2DPoolKey, Texture2DAllocation>
	{
	public:
		Texture2DPool();
		virtual ~Texture2DPool();

		void ReturnAllocation(Texture2DAllocation& _allocation);

		Texture2DAllocation AllocateTexture2DHandle(UInt32 _width, UInt32 _height, RenderFormat _format);
		Texture2DAllocation AllocateTexture2DHandle(const Texture2DPoolKey& _key);
	protected:

	private:

	};
}



