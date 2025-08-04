#ifndef DMA_CONFIG_H_
#define DMA_CONFIG_H_

#include "stm32f4xx.h"

#define NSAMPLES 256
#define ADC_BUFFER_SIZE (NSAMPLES * 2)

extern volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];

void DMA2_Stream0_init(void);

#endif /* DMA_CONFIG_H_ */
