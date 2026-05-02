#include <std.int.h>
#include <stdbool.h>

#define SYSCTL_RCGCGPIO_R      (*((volatile uint32_t *)0x400FE608))
#define GPIO_PORTB_DATA_R      (*((volatile uint32_t *)0x400053FC))
#define GPIO_PORTB_DIR_R       (*((volatile uint32_t *)0x40005400))
#define GPIO_PORTB_DEN_R       (*((volatile uint32_t *)0x4000551C))

void Init_LCD(void);
void LCD_WriteString(char *str);

int main(void)
{


	return 0;
}

void Init_LCD(void)
{

}

void LCD_WriteString(char *str)
{

}
