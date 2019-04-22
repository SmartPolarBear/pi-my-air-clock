#pragma once
#include <tuple>
#include <string>
#include <algorithm>
#include <type_traits>

namespace lcd1602
{
	template<typename E>
	constexpr typename std::underlying_type<E>::type to_underlying(E e)noexcept
	{
		return static_cast<typename std::underlying_type<E>::type>(e);
	}

	using std::string;

	void init_lcd1602(void);
	void clear_lcd1602(void);
	void write_lcd1602(int x, int y, string data);
}