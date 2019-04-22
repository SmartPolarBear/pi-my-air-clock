#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <cassert>
#include <cstdint>
#include <string>
#include <type_traits>
#include <algorithm>
#include "lcd1602.h"

using std::min;
using std::max;
using std::string;

enum class LCD1602_CONFIGS
{
	LCD1602_8LINE = 0x33,
	LCD1602_4LINE = 0x32,
	LCD1602_DISPLAY_PROPERTIES = 0x28, //2lines, 5x7dots
	LCD1602_CURSOR = 0x0C
};

enum class LCD1602_COMMANDS
{
	LCD1602_CLEAR = 0x01,
};

static constexpr auto lcd_addr = 0x27;

static constexpr auto high4_mask = 0b11110000;
static constexpr auto low4_mask = 0b00001111;
static constexpr auto en_mask = 0b11111011;

static int fd = 0;

template<size_t delaytime = 0>
static inline void write_word(int32_t data)
{
	assert(fd != 0);

	int ret = wiringPiI2CWrite(fd, data | 0x08);
	assert(ret >= 0);

	delay(delaytime);
}

template<int32_t RS, int32_t RW, int32_t EN>
static inline void send(int32_t data)
{
	constexpr int flags = (EN << 2) + (RW << 1) + (RS << 0);

	int high = (data & high4_mask) | flags;
	write_word<2>(high);
	high &= en_mask;
	write_word(high);

	int low = ((data & low4_mask) << 4) | flags;
	write_word<2>(low);
	low &= en_mask;
	write_word(low);
}

template<size_t delaytime = 0>
static inline void send_command(int32_t comm)
{
	send<false, false, true>(comm);
	delay(delaytime);
}

template<size_t delaytime = 0>
static inline void send_data(int32_t data)
{
	send<true, false, true>(data);
	delay(delaytime);
}


void lcd1602::init_lcd1602(void)
{
	fd = wiringPiI2CSetup(lcd_addr);
	assert(fd >= 0);

	send_command<5>(to_underlying(LCD1602_CONFIGS::LCD1602_8LINE));			// Must initialize to 8-line mode at first
	send_command<5>(to_underlying(LCD1602_CONFIGS::LCD1602_4LINE));			// Then initialize to 4-line mode
	send_command<5>(to_underlying(LCD1602_CONFIGS::LCD1602_DISPLAY_PROPERTIES));	// 2 Lines & 5*7 dots
	send_command<5>(to_underlying(LCD1602_CONFIGS::LCD1602_CURSOR));			// Enable display without cursor

	clear_lcd1602();

	int ret = wiringPiI2CWrite(fd, 0x08);
	assert(ret >= 0);
}

void lcd1602::clear_lcd1602(void)
{
	send_command(to_underlying(LCD1602_COMMANDS::LCD1602_CLEAR));	// Clear Screen
}

void lcd1602::write_lcd1602(int x, int y, string data)
{
	assert(x >= 0 && x <= 15);
	assert(y >= 0 && y <= 15);

	auto cur_addr = 0x80 + 0x40 * y + x;
	send_command(cur_addr);

	std::for_each(data.begin(), data.end(), [](char c) {send_data(c); });
}
