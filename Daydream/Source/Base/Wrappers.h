#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <functional>

namespace Daydream
{
	template <typename T1, typename T2>
	using Pair = std::pair<T1, T2>;

	template <typename T1, typename T2>
	constexpr Pair<T1, T2> MakePair(T1&& _first, T2&& _second)
	{
		return std::make_pair<T1, T2>(std::forward<T1>(_first), std::forward<T2>(_second));
	}

	using Byte = std::byte;
	using String = std::string;
	using StringView = std::string_view;
	using WideString = std::wstring;

	template<typename T>
	using Unique = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Unique<T> MakeUnique(Args&& ... _args)
	{
		return std::make_unique<T>(std::forward<Args>(_args)...);
	}

	template<typename T>
	using Shared = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Shared<T> MakeShared(Args&& ... _args)
	{
		return std::make_shared<T>(std::forward<Args>(_args)...);
	}

	template<typename T>
	using FunctionPtr = std::function<T>;
}