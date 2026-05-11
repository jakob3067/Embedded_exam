/*
 * payment.h
 *
 *  Created on: May 8, 2026
 *      Author: jakob
 */

#ifndef PAYMENT_H_
#define PAYMENT_H_

void payment_task(void *pvParameters);

void card(void);

INT8U validate_pay(INT8U *card_details, INT8U *card_pin);

#endif /* PAYMENT_H_ */
