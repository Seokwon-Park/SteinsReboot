#pragma once

namespace Daydream
{
	class GPUResource
	{
	public:
		virtual ~GPUResource() = default;

		inline void SetState(ResourceState _newState) { currentState = _newState; }
		inline ResourceState GetState() const { return currentState; }
	protected:

	private:
		ResourceState currentState = ResourceState::Undefined;
	};
}
