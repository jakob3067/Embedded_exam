#include <std.int.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"
#include "lcd.h"

extern volatile INT16S ticks;

INT16S alive_timer = MILLISEC(100);

int main(void)
{
	// Application loop
	while(1)
	{
		
	}

	return 0;
}

void Init_LCD(void)
{

}

void LCD_WriteString(char *str)
{

}
