#ifndef LCD_H_
#define LCD_H_

void init_portF(void);

void init_lcd(void);

void pulse_lcd(void);

void lcd_nibble(uint8_t);

void delay(int n);

void print_lcd(char *str);

void clear_lcd(void);

void data_lcd(unsigned char data);

void command_lcd(unsigned char command);

void lcd_task(void *pvParameters);

#endif
