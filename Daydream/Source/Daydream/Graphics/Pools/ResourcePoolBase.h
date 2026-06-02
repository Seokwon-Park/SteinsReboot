#pragma once

#include "Base/Base.h"

namespace Daydream
{
	class IResourcePool
	{
	public:
		virtual ~IResourcePool() = default;

		virtual void FlushInFlight() = 0;
		virtual void CleanUp() = 0;
		virtual void UpdatePoolState(UInt32 _currentLoop) = 0;
	};

	template <typename TKey, typename TPayload>
	class ResourcePoolBase : public IResourcePool
	{
	public:
		virtual ~ResourcePoolBase() = default;

		void Setup(UInt32 _maxFramesInFlight, UInt32 _removeLimit) 
		{
			maxFramesInFlight = _maxFramesInFlight;
			removeLimit = _removeLimit;
		}

		void ReturnResource(TKey _key, TPayload&& _handle) 
		{
			PooledItem returnItem{};
			returnItem.key = _key;
			returnItem.payload = std::move(_handle);
			returnItem.lastUsedLoop = currentLoop;

			pendingQueue.push(returnItem);
		}

		void UpdatePoolState(UInt32 _currentLoop) override
		{
			currentLoop = _currentLoop;
		}

		void FlushInFlight() override
		{
			while (!pendingQueue.empty() && pendingQueue.front().lastUsedLoop + maxFramesInFlight <= currentLoop)
			{
				pendingQueue.front().lastUsedLoop = currentLoop;
				TKey bucketKey = pendingQueue.front().key;
				freeQueue[bucketKey].push(std::move(pendingQueue.front()));
				pendingQueue.pop();
			}
		}

		void CleanUp() override
		{
			for (auto& [bucketKey, handles] : freeQueue)
			{
				while (!handles.empty() && handles.front().lastUsedLoop + removeLimit <= currentLoop)
				{
					handles.pop();
				}
			}
		}

	protected:
		struct PooledItem
		{
			TKey key;
			TPayload payload;
			UInt32 lastUsedLoop;
		};

		UInt32 currentLoop = 0;
		UInt32 removeLimit = 100;
		UInt32 maxFramesInFlight = 3;
		Queue<PooledItem> pendingQueue;
		HashMap<TKey, Queue<PooledItem>> freeQueue;
	};
}
