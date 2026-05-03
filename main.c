#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "lcd.h"

extern volatile int16_t ticks;

int16_t alive_timer;

int main(void)
{
	init_lcd(); // Initialize LCD (4-bit mode, 2-line)

	clear_lcd();
    print_lcd("Welcome to the");// Line 1

    command_lcd(0xC0); // goes a line down

    print_lcd("Coffee Machine!");// Line 2


	// Application loop
	while(1)
	{
		
	}
}
