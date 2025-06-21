/*
 * USART_config.c
 *
 *  Created on: 05/04/2025
 *      Author: 35191
 */

#include "USART_config.h"

// Inicializa a USART1 para comunica��o serial com DMA
void USART1_init(void) {
    // Habilita o clock do perif�rico USART1
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // Configura o baud rate: BRR = Fclk / BaudRate
    USART1->BRR = SystemCoreClock / USART1_BAUDRATE;

    // Palavra de 8 bits (M = 0)
    USART1->CR1 &= ~USART_CR1_M;

    // Um stop bit (STOP = 00)
    USART1->CR2 &= ~USART_CR2_STOP;

    // Ativa suporte DMA para transmiss�o (bit DMAT)
    USART1->CR3 |= USART_CR3_DMAT;

    // Habilita transmissor
    USART1->CR1 |= USART_CR1_TE;

    // Habilita USART1
    USART1->CR1 |= USART_CR1_UE;
}

// Envia um �nico byte pela USART1 (bloqueante)
void USART1_send(uint8_t dado) {
    while (!(USART1->SR & USART_SR_TXE));  // Espera buffer vazio
    USART1->DR = dado;                     // Escreve dado
    while (!(USART1->SR & USART_SR_TC));   // Espera envio terminar
}

// Envia uma string completa pela USART1 (bloqueante)
void USART1_send_string(char *str) {
    while (*str) {
        USART1_send(*str++);
    }
}

// Envia bloco de dados via USART1 DMA2 Stream7
void USART1_DMA_send(uint8_t* data, uint16_t size) {
    // Habilita o clock do DMA2
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    // Desabilita o Stream7
    DMA2_Stream7->CR &= ~DMA_SxCR_EN;
    while (DMA2_Stream7->CR & DMA_SxCR_EN);

    // Configura perif�rico e mem�ria
    DMA2_Stream7->PAR = (uint32_t)&USART1->DR;  	// Registo USART
    DMA2_Stream7->M0AR = (uint32_t)data;        	// Endere�o do bloco de dados
    DMA2_Stream7->NDTR = size;                  	// N�mero de bytes

    // Configura o canal e dire��o
    DMA2_Stream7->CR = 0;  							// Limpa configura��es anteriores
    DMA2_Stream7->CR |= (4 << DMA_SxCR_CHSEL_Pos);  // Canal 4 (USART1_TX)
    DMA2_Stream7->CR |= DMA_SxCR_MINC;              // Incrementa mem�ria automaticamente
    DMA2_Stream7->CR |= DMA_SxCR_DIR_0;             // Mem�ria -> Perif�rico
    DMA2_Stream7->CR |= DMA_SxCR_PL_0;              // Prioridade m�dia
    DMA2_Stream7->CR |= DMA_SxCR_TCIE;              // Ativa interrup��o ao terminar (debug)

    // Ativa o Stream
    DMA2_Stream7->CR |= DMA_SxCR_EN;
}
