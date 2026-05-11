#ifndef COFFEE_H_
#define COFFEE_H_

void brew(int coffee);

void brew_task(void *pvParameters);

void payment_option(void);

INT8U validate_pay(INT8U *card_details, INT8U *card_pin);

#endif /* COFFEE_H_ */
