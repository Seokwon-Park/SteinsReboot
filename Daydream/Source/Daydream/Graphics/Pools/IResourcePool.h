#pragma once

#include "Base/Base.h"

namespace Daydream
{
	class IResourcePool
	{
	public:
		virtual ~IResourcePool() = default;

		virtual void FlushInFlight(UInt32 _currentLoop, UInt32 _maxInFlight) = 0;
		virtual void CleanUp(UInt32 _currentLoop, UInt32 _removeLimit) = 0;
	};
}
