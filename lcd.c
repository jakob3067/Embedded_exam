#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "lcd.h"
#include "FreeRTOS.h"
#include "task.h"

void init_portF(void){
    SYSCTL_RCGCGPIO_R |= 0x20; 
    while((SYSCTL_PRGPIO_R & 0x20) == 0) {};
    GPIO_PORTF_LOCK_R = 0x4C4F434B; 
    GPIO_PORTF_CR_R = 0x1F; 
    GPIO_PORTF_DIR_R = 0x0E; 
    GPIO_PORTF_PUR_R = 0x11; 
    GPIO_PORTF_DEN_R = 0x1F;
}

void init_lcd(void){
    delay(50);
    SYSCTL_RCGCGPIO_R |= 0x0C; // Enable Port C and D
    while((SYSCTL_PRGPIO_R & 0x0C) != 0x0C); // Wait until BOTH are ready
    delay(2);

    GPIO_PORTC_DIR_R |= 0xF0; 
    GPIO_PORTC_DEN_R |= 0xF0; 
    GPIO_PORTD_DIR_R |= 0xC0; // PD2, PD3, PD6, PD7 (adjust based on your actual pins)
    GPIO_PORTD_DEN_R |= 0xC0;

    delay(50);

    lcd_nibble(0x30);  // 1st reset
    delay(5);
    lcd_nibble(0x30);  // 2nd reset
    delay(1);
    lcd_nibble(0x30);  // 3rd reset
    delay(1);
    lcd_nibble(0x20); // sets 4 bit operation
    delay(1);

    command_lcd(0x28); // sets 2 line display
    command_lcd(0x06); // cursor moves right after character,
    command_lcd(0x01); // clear screen
    command_lcd(0x0F); // turns on display and cursor
    delay(5);
}

void pulse_lcd(void)
{
    GPIO_PORTD_DATA_R |= 0x80;
    delay(5);
    GPIO_PORTD_DATA_R &= ~0x80;
    delay(5);
}

void lcd_nibble(uint8_t nibble)
{
    GPIO_PORTD_DATA_R &= ~0x40; // Ensure RS = 0 for command
    GPIO_PORTC_DATA_R = (nibble & 0xF0);
    pulse_lcd();
}

void delay(int n){
    volatile int i, j;
    for(i = 0; i < n; i++)
        for(j = 0; j < 3180; j++);
}

void print_lcd(char *str){
    while(*str)
    {
        data_lcd(*str++);
    }
}

void data_lcd(unsigned char data){
    GPIO_PORTC_DATA_R = (GPIO_PORTC_DATA_R & 0xF0) | (data & 0xF0);
    GPIO_PORTD_DATA_R |= 0x40;  // RS = 1 for DATA (This is the change!)
    GPIO_PORTD_DATA_R |= 0x80;  // EN = 1
    delay(1);
    GPIO_PORTD_DATA_R &= ~0x80; // EN = 0

    GPIO_PORTC_DATA_R = (GPIO_PORTC_DATA_R & 0xF0) | ((data << 4) & 0xF0);
    GPIO_PORTD_DATA_R |= 0x40;  // RS = 1 for DATA
    GPIO_PORTD_DATA_R |= 0x80;  // EN = 1
    delay(1);
    GPIO_PORTD_DATA_R &= ~0x80; // EN = 0
}

void command_lcd(unsigned char command){
    GPIO_PORTC_DATA_R = (GPIO_PORTC_DATA_R & 0xF0) | (command & 0xF0); // Send higher nibble
    GPIO_PORTD_DATA_R &= ~0x40; // RS = 0 for command
    GPIO_PORTD_DATA_R |= 0x80; // EN = 1
    delay(1);
    GPIO_PORTD_DATA_R &= ~0x80; // EN = 0
    delay(1);

    GPIO_PORTC_DATA_R = (GPIO_PORTC_DATA_R & 0xF0) | ((command << 4) & 0xF0); // Send lower nibble
    GPIO_PORTD_DATA_R &= ~0x40; // RS = 0 for command
    GPIO_PORTD_DATA_R |= 0x80; // EN = 1
    delay(1);
    GPIO_PORTD_DATA_R &= ~0x80; // EN = 0
    delay(1);
}

void lcd_task(void *pvParameters)
{
    print_lcd("Welcome");
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void clear_lcd(void){
    command_lcd(0x01); // Clear display
    delay(5);; // Delay for clear command
}
