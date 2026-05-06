/*****************************************************************************
* University of Southern Denmark
* Embedded C Programming (ECP)
*
* MODULENAME.: tmodel.h
*
* PROJECT....: ECP
*
* DESCRIPTION: Defines the elemtn of the task model..
*
* Change Log:
******************************************************************************
* Date    Id    Change
* YYMMDD
* --------------------
* 101004  MoH   Module created.
*
*****************************************************************************/

#ifndef _TMODEL_H_
#define _TMODEL_H_

// Tasks.
// ------
#define TASK_RTC         6
#define TASK_RTC_DISPLAY 7
#define TASK_RTC_ADJUST  8
#define TASK_LCD         9
#define TASK_SW_TIMERS   11
#define TASK_MAIN        12
#define TASK_BUTTON      13
#define TASK_UI          14
#define TASK_CASH        15
#define TASK_CARD        16
#define TASK_BREW        17
#define TASK_ENCODER     18
#define TASK_UART_LOG    19


// Interrupt Service Routines.
// ---------------------------
#define ISR_TIMER 21

// Shared State Memory.
// --------------------
#define SSM_RTC_SEC            31
#define SSM_RTC_MIN            32
#define SSM_RTC_HOUR           33

#define SSM_SELECTED_PRODUCT   50   // 0=espresso, 1=latte, 2=filter
#define SSM_PAYMENT_METHOD     51   // 0=cash, 1=card
#define SSM_PRICE_ESPRESSO     52
#define SSM_PRICE_LATTE        53
#define SSM_PRICE_FILTER       54   // price per cl

#define SSM_CASH_BALANCE       55   // balance inserted in øre
#define SSM_CASH_TARGET        56

// Shared Event Buffers.
// ---------------------
#define SEB_BUTTON_EVENT 41
#define SEB_TO_BUTTON 42 // Time Out
#define SEB_TO_TRAFFIC_LIGHT 43 // Time Out
#define SEB_TO_RTC 44 // Time Out

#define SEB_ENCODER_EVENT      60   // CW=+20, CCW=+5
#define SEB_UI_EVENT           61   // product/payment chosen
#define SEB_PAYMENT_DONE       62   // triggers brew task
#define SEB_BREW_DONE          63   // triggers logging

// Semaphores
// ----------
#define SEM_OUTPINS       0
#define SEM_RTC_UPDATED   1
#define SEM_RTC_UPDATE    2
#define SEM_LCD_ACCESS    3
#define MUTEX_LCD_DISPLAY 4
#define MUTEX_SYSTEM_RTC  5
#define SEM_LCD_DISPLAY   6

// QUEUEs.
// -------
#define Q_OUTPUT 1

// Traffic Light Modes.
// --------------------
#define MM_TLIGHT   1
#define MM_REDLIGHT 2
#define MM_FLASH    3

// Software Timers.
// ----------------
#define ST_BUTTON   1
#define ST_RTC      2
#define ST_TLIGHT   3

#define ST_BREW_GRIND    10
#define ST_BREW_BREW     11
#define ST_BREW_FROTH    12
#define ST_FILTER_FLOW   13
#define ST_FILTER_IDLE   14

#endif /* _TMODEL_H_ */
