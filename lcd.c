#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "lcd.h"

void init_lcd(void){
    // First part?
    delay(50);

    /*Enable clocks ports*/
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOC|SYSCTL_RCGC2_GPIOD;

    /*enable LCD as output*/
    GPIO_PORTC_DIR_R = 0xF0; //
    GPIO_PORTD_DIR_R = 0x4C;

    /*enable LCD as digital signal*/
    GPIO_PORTC_DEN_R = 0xF0;
    GPIO_PORTD_DEN_R = 0x4C; //

    // Second part?
    delay(20);
    lcd_nibble(0x03);  // 1st reset
    delay(5);
    lcd_nibble(0x03);  // 2nd reset
    delay(1);
    lcd_nibble(0x03);  // 3rd reset
    delay(1);
    lcd_nibble(0x02); // sets 4 bit operation
    delay(1);

    command_lcd(0x28); // sets 2 line display
    command_lcd(0x0C); // disables cursor and turns on display
    command_lcd(0x06); // cursor moves right after character,
    command_lcd(0x01); // clear screen
}

void pulse_lcd(void)
{
    GPIO_PORTD_DATA_R |= 0x08;
    delay(50);
    GPIO_PORTD_DATA_R &= ~0x08;
    delay(50);
}

void lcd_nibble(uint8_t nibble)
{
    GPIO_PORTC_DATA_R = (nibble << 4) & 0xF0;
    pulse_lcd();
}

void delay(int n){
    volatile int i, j;
    for(i = 0; i < n; i++)
        for(j = 0; j < 3; j++);
}

void print_lcd(char *str){
    while(*str)
    {
        data_lcd(*str++);
    }
}

void data_lcd(unsigned char data){
    GPIO_PORTC_DATA_R = (data & 0xF0);
    GPIO_PORTD_DATA_R |= 0x40;  // RS = 1 for DATA (This is the change!)
    GPIO_PORTD_DATA_R |= 0x80;  // EN = 1
    delay(1);
    GPIO_PORTD_DATA_R &= ~0x80; // EN = 0

    GPIO_PORTC_DATA_R = (data << 4) & 0xF0;
    GPIO_PORTD_DATA_R |= 0x40;  // RS = 1 for DATA
    GPIO_PORTD_DATA_R |= 0x80;  // EN = 1
    delay(1);
    GPIO_PORTD_DATA_R &= ~0x80; // EN = 0
}

void command_lcd(unsigned char command){
    GPIO_PORTC_DATA_R = (command & 0xF0); // Send higher nibble
    GPIO_PORTD_DATA_R &= ~0x40; // RS = 0 for command
    GPIO_PORTD_DATA_R |= 0x80; // EN = 1
    delay(1);
    GPIO_PORTD_DATA_R &= ~0x80; // EN = 0
    delay(1);

    GPIO_PORTC_DATA_R = (command << 4) & 0xF0; // Send lower nibble
    GPIO_PORTD_DATA_R &= ~0x40; // RS = 0 for command
    GPIO_PORTD_DATA_R |= 0x80; // EN = 1
    delay(1);
    GPIO_PORTD_DATA_R &= ~0x80; // EN = 0
    delay(1);
}

void clear_lcd(void){
    command_lcd(0x01); // Clear display
    delay(5); // Delay for clear command
}
