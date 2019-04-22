#include <cstdio>
#include <wiringPi.h>
#include "lcd1602.h"

int main()
{
	if (wiringPiSetup() == -1)
	{
		printf("Can't setup GPIO!");
		while (true);
		return 0;
	}

	lcd1602::init_lcd1602();
	lcd1602::write_lcd1602(0, 0, "fuck");
	return 0;
}