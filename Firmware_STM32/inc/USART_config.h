#ifndef USART_CONFIG_H_
#define USART_CONFIG_H_

#include "stm32f4xx.h"

#define USART1_BAUDRATE 3000000

void USART1_init(void);
void USART1_send(uint8_t dado);
void USART1_send_string(char *str);
void USART1_DMA_send(uint8_t* data, uint16_t size);

#endif /* USART_CONFIG_H_ */
