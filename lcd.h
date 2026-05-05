#ifndef _LCD_H
#define _LCD_H

#define LF      0x0A
#define FF      0x0C
#define CR      0x0D
#define ESC     0x1B

void move_LCD( INT8U, INT8U );
INT8U wr_ch_LCD( INT8U );
void wr_str_LCD( INT8U* );

void lcd_task( INT8U task_no );

#endif
