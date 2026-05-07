/*
 * Menu.c
 *
 *  Created on: May 7, 2026
 *      Author: jakob
 */
/***************************** Include files *******************************/
#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "emp_type.h"
//#include "glob_def.h"
//#include "binary.h"
#include "status_led.h"
#include "menu.h"
#include "lcd.h"

/*****************************    Defines    *******************************/
#define PF0     0       // Bit 0

#define USERTASK_STACK_SIZE configMINIMAL_STACK_SIZE
#define IDLE_PRIO 0
#define LOW_PRIO  1
#define MED_PRIO  2
#define HIGH_PRIO 3
/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

void menu_text(void)
{
    INT8U *pStr = "what would you";
    clr_LCD();
    vTaskDelay(pdMS_TO_TICKS(5));
    home_LCD();
    vTaskDelay(pdMS_TO_TICKS(5));

    /* If a string pointer was passed as pvParameters, print it char-by-char */
    if (pStr != NULL)
    {
      while (*pStr)
      {
        out_LCD(*pStr);
        vTaskDelay(pdMS_TO_TICKS(20)); /* allow char to be processed (~20ms) */
        pStr++;
      }
    }


}


void menu_task(void *pvParameters)
{

    while(1)
    {

        xTaskCreate( lcd_task, "LCD", USERTASK_STACK_SIZE, (void *)"What would you", HIGH_PRIO, NULL);
        vTaskDelay(pdMS_TO_TICKS(2500));
        xTaskCreate( lcd_task, "LCD1", USERTASK_STACK_SIZE, (void *)"like to drink?", HIGH_PRIO, NULL);
        vTaskDelay(pdMS_TO_TICKS(2500));

    }


}


/****************************** End Of Module *******************************/




