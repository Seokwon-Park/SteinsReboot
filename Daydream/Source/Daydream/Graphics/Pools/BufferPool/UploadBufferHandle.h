#pragma once

#include "Daydream/Graphics/Resources/Buffer.h"

namespace Daydream
{
	class UploadBufferHandle
	{
		friend class UploadBufferPool;
	public:
		UploadBufferHandle() = default;
		~UploadBufferHandle() = default;

		UploadBuffer* GetBuffer() const { return buffer.get(); }
	protected:

	private:
		UInt64 lastUsedLoop = 0;
		Shared<UploadBuffer> buffer;
	};
}
