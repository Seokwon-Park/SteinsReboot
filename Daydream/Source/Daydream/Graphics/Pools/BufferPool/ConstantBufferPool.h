#pragma once

#include "Daydream/Graphics/Resources/Buffer.h"
#include "Daydream/Graphics/Pools/ResourcePoolBase.h"
#include "BufferPoolHandle.h"

namespace Daydream
{
	class ConstantBufferPool : public ResourcePoolBase<UInt32, Shared<ConstantBuffer>>
	{
	public:
		ConstantBufferPool();
		virtual ~ConstantBufferPool();

		Shared<ConstantBuffer> RequestBuffer(UInt32 _size);
	protected:

	private:
	};
}
