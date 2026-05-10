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
#include "emp_type.h"
#include "glob_def.h"
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

extern QueueHandle_t xMenuQueue;
extern QueueHandle_t xLCDQueue;

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/

/*****************************   Functions   *******************************/

void menu_task(void *pvParameters)
{
    INT8U *pStr;
    pStr = (INT8U *)"Select Item";
    xQueueSend(xLCDQueue, &pStr, portMAX_DELAY);



    while(1)
    {

    }
}

/****************************** End Of Module *******************************/




