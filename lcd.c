#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "lcd.h"

void init_lcd(void){
    int dumdum;
    /*Enable clocks ports*/
    SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOC|SYSCTL_RCGC2_GPIOD;

    dumdum = SYSCTL_RCGC2_R;

    /*enable LCD as output*/
    GPIO_PORTC_DIR_R = 0xF0; //
    GPIO_PORTD_DIR_R = 0x4C;

    /*enable LCD as digital signal*/
    GPIO_PORTC_DEN_R = 0xF0;
    GPIO_PORTD_DEN_R = 0x4C; //
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

void clearLCD(void){
    command_lcd(0x01); // Clear display
    delay(2); // Delay for clear command
}