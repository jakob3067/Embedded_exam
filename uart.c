/***************************** Include files *******************************/
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "FreeRTOS.h"
#include "tmodel.h"
#include "queue.h"
#include "lcd.h"
#include "rtc.h"
#include "string.h"
/*****************************    Defines    *******************************/

extern QueueHandle_t xUARTQueue;
extern QueueHandle_t xLCDQueue;

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/


/*****************************   Functions   *******************************/

BOOLEAN uart0_put_q( INT8U ch )
{
  put_queue( Q_UART_TX, ch, 1 );
  return( 1 );
}

BOOLEAN uart0_get_q( INT8U *pch )
{
  return( get_queue( Q_UART_RX, pch, 1 ));
}

BOOLEAN uart0_rx_rdy()
{
  return( !(UART0_FR_R & UART_FR_RXFE) );
}

INT8U uart0_getc()
{
  return ( UART0_DR_R );
}

BOOLEAN uart0_tx_rdy()
{
  return( UART0_FR_R & UART_FR_TXFE );
}

void uart0_putc( INT8U ch )
{
  UART0_DR_R = ch;
}

extern void uart_rx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
  if( uart0_rx_rdy() )
  	put_queue( Q_UART_RX, uart0_getc(), WAIT_FOREVER );
  else
	wait( 1 );
}

extern void uart_tx_task(INT8U my_id, INT8U my_state, INT8U event, INT8U data)
{
  INT8U ch;

  if( get_queue( Q_UART_TX, &ch, WAIT_FOREVER ))
  	UART0_DR_R = ch;
}

INT32U lcrh_databits( INT8U antal_databits )
{
  if(( antal_databits < 5 ) || ( antal_databits > 8 ))
	antal_databits = 8;
  return(( (INT32U)antal_databits - 5 ) << 5 );  // Control bit 5-6, WLEN
}

INT32U lcrh_stopbits( INT8U antal_stopbits )
{
  if( antal_stopbits == 2 )
    return( 0x00000008 );  		// return bit 3 = 1
  else
	return( 0x00000000 );		// return all zeros
}

INT32U lcrh_parity( INT8U parity )
{
  INT32U result;

  switch( parity )
  {
    case 'e':
      result = 0x00000006;
      break;
    case 'o':
      result = 0x00000002;
      break;
    case '0':
      result = 0x00000086;
      break;
    case '1':
      result = 0x00000082;
      break;
    case 'n':
    default:
      result = 0x00000000;
  }
  return( result );
}

void uart0_fifos_enable()
{
  UART0_LCRH_R  |= 0x00000010;
}

void uart0_fifos_disable()
{
  UART0_LCRH_R  &= 0xFFFFFFEF;
}

void uart0_init( INT32U baud_rate, INT8U databits, INT8U stopbits, INT8U parity )
{
  INT32U BRD;

  #ifndef E_PORTA
  #define E_PORTA
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOA;					// Enable clock for Port A
  #endif

  #ifndef E_UART0
  #define E_UART0
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART0;					// Enable clock for UART 0
  #endif

  GPIO_PORTA_AFSEL_R |= 0x00000003;
  GPIO_PORTA_PCTL_R  = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) | 0x00000011;
  GPIO_PORTA_DIR_R   |= 0x00000002;
  GPIO_PORTA_DEN_R   |= 0x00000003;
  GPIO_PORTA_PUR_R   |= 0x00000002;

  UART0_IBRD_R = 16000000  / (16 * baud_rate);
  UART0_FBRD_R = ((16000000  * 4) / baud_rate) & 0x0000003F;

  UART0_LCRH_R  = lcrh_databits( databits );
  UART0_LCRH_R += lcrh_stopbits( stopbits );
  UART0_LCRH_R += lcrh_parity( parity );

  uart0_fifos_disable();

  UART0_CTL_R |= (UART_CTL_UARTEN | UART_CTL_TXE | UART_CTL_RXE); // Enable UART TX and RX
}

static void build_timestamp(char *timestamp, char *msg)
{
    static char hour_str[3], min_str[3], sec_str[3];

    hour_str[0] = '0' + get_hour() / 10;  hour_str[1] = '0' + get_hour() % 10;  hour_str[2] = '\0';
    min_str[0]  = '0' + get_min()  / 10;  min_str[1]  = '0' + get_min()  % 10;  min_str[2]  = '\0';
    sec_str[0]  = '0' + get_sec()  / 10;  sec_str[1]  = '0' + get_sec()  % 10;  sec_str[2]  = '\0';

    strcpy(timestamp, "[");
    strcat(timestamp, hour_str);
    strcat(timestamp, ":");
    strcat(timestamp, min_str);
    strcat(timestamp, ":");
    strcat(timestamp, sec_str);
    strcat(timestamp, "] ");
    strcat(timestamp, msg);
}

void uart_log_task(void *pvParameters)
{
  INT8U coffee_type;
  INT8U *pStr;
  INT8U *discard;
  char *p;

  // Timestamped log message
  static char timestamp[96];

  while (1)
  {
    // Read from UART Queue
    if (xQueueReceive(xUARTQueue, &coffee_type, portMAX_DELAY) == pdTRUE)
    {
      char *msg;
      switch (coffee_type)
      {
          case 0: msg = "Product: Espresso | Price: 15 DKK | Amount: 1\r\n"; break;
          case 1: msg = "Product: Latte | Price: 27 DKK | Amount: 1\r\n";    break;
          case 2: msg = "Product: Filter Coffee\r\n";                        break;
      }

      // Send log message to LCD
      pStr = (INT8U *)"Logging... ";
      xQueueSend(xLCDQueue, (void *) &pStr, portMAX_DELAY);
      // Make sure logging is readable in LCD
      vTaskDelay(pdMS_TO_TICKS(2000));

      while (xQueueReceive(xLCDQueue, &discard, 0) == pdTRUE);
      vTaskDelay(pdMS_TO_TICKS(2000));

      // Get current time from RTC
      build_timestamp(timestamp, msg);

      // Send log message to UART
      p = timestamp;
      while (*p != '\0')
      {
        while (!uart0_tx_rdy())
        {
          vTaskDelay(pdMS_TO_TICKS(1));
        }
        uart0_putc((INT8U)*p++);
      }

      // Write completed log message to LCD
      pStr = (INT8U *)"Logging complete ";
      xQueueSend(xLCDQueue, (void *) &pStr, portMAX_DELAY);
      vTaskDelay(pdMS_TO_TICKS(2000));
    }
  }
}

/****************************** End Of Module *******************************/












