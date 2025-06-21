/*
 * GPIO_init.c
 *
 *  Created on: 05/04/2025
 *      Author: 35191
 */

#include "GPIO_init.h"

// Inicialização do GPIO para LED
void GPIO_init(void) {
    // Habilita o clock para o GPIOA
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // Configura PA5 como saída (LED)
    GPIOA->MODER &= ~(3 << (5 * 2));  // Limpa os bits de configuração de PA5
    GPIOA->MODER |=  (1 << (5 * 2));  // Configura PA5 como saída

    // PA7 como saída (toggle para medir duração da interrupção)
    GPIOA->MODER &= ~(3 << (7 * 2));
    GPIOA->MODER |=  (1 << (7 * 2));

    // Configura PA9 (TX) e PA10 (RX) para funções alternativas (AF)
    // PA9 como AF7 (USART1_TX)
    GPIOA->MODER &= ~(3 << (9 * 2));  // Limpa os bits de MODER para PA9
    GPIOA->MODER |=  (2 << (9 * 2));  // Configura PA9 como AF

    // PA10 como AF7 (USART1_RX)
    GPIOA->MODER &= ~(3 << (10 * 2)); // Limpa os bits de MODER para PA10
    GPIOA->MODER |=  (2 << (10 * 2)); // Configura PA10 como AF

    // AF7 (USART1) PA9 e PA10
    GPIOA->AFR[1] &= ~(0xF << (1 * 4));  // Limpa os bits de AF para PA9
    GPIOA->AFR[1] |=  (7 << (1 * 4));    // Define AF7 (USART1_TX) para PA9

    GPIOA->AFR[1] &= ~(0xF << (2 * 4));  // Limpa os bits de AF para PA10
    GPIOA->AFR[1] |=  (7 << (2 * 4));    // Define AF7 (USART1_RX) para PA10

    // Configura os pinos PA9 e PA10 com alta velocidade (Speed = 0b11 -> 50 MHz)
    GPIOA->OSPEEDR &= ~(3 << (9 * 2));   // Limpa os bits de speed para PA9
    GPIOA->OSPEEDR |=  (3 << (9 * 2));   // Configura PA9 como alta velocidade
    GPIOA->OSPEEDR &= ~(3 << (10 * 2));  // Limpa os bits de speed para PA10
    GPIOA->OSPEEDR |=  (3 << (10 * 2));  // Configura PA10 como alta velocidade

    // Configura PA0 e PA1 como entrada analógica
    GPIOA->MODER |= (0b11 << (0 * 2)) | (0b11 << (1 * 2));  // PA0 e PA1 como entrada analógica

}

