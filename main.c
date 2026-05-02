#include <std.int.h>
#include <stdbool.h>

#define SYSCTL_RCGCGPIO_R      (*((volatile uint32_t *)0x400FE608))
#define GPIO_PORTB_DATA_R      (*((volatile uint32_t *)0x400053FC))
#define GPIO_PORTB_DIR_R       (*((volatile uint32_t *)0x40005400))
#define GPIO_PORTB_DEN_R       (*((volatile uint32_t *)0x4000551C))

void Init_LCD(void);
void LCD_WriteString(char *str);

void Delay(uint32_t count){
	volatile uint32_t i;
	for(i = 0; i < count; i++);
}

int main(void)
{
	SYSCTL_RCGCGPIO_R |= 0x02; // Enable clock for Port B

	while((SYSCTL_RCGCGPIO_R & 0x02) == 0); // Wait for Port B to be ready

	GPIO_PORTB_DIR_R |= 0xFF; // Set Port B pins as output
	GPIO_PORTB_DEN_R |= 0xFF; // Enable digital function for Port B pins

	Init_LCD(); // Initialize the LCD

	LCD_WriteString("Hello, World!"); // Write a string to the LCD

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
