#pragma once

#include "unordered_map"

namespace Daydream
{
	template <typename Key, typename Value, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
	using HashMap = std::unordered_map<Key, Value, Hash, KeyEqual>;
}