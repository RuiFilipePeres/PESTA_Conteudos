/*
 * TIM_config.h
 *
 *  Created on: 05/04/2025
 *      Author: 35191
 */

#ifndef TIM_CONFIG_H_
#define TIM_CONFIG_H_

#include "stm32f4xx.h"

// LED a 1Hz
void TIM11_init(void);
void TIM1_CH1_trigger_init(uint32_t sample_rate_hz);

#endif /* TIM_CONFIG_H_ */
