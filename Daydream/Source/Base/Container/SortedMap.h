#pragma once

#include <map>

namespace Daydream
{
	template <typename Key, typename Value, class Pr = std::less<Key>, class Alloc = std::allocator<Pair<const Key, Value>>>
	using SortedMap = std::map<Key, Value, Pr, Alloc>;
}