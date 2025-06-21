#include "DMA_config.h"
#include "USART_config.h"

volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];

void DMA2_Stream0_init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    DMA2_Stream0->CR &= ~DMA_SxCR_EN;
    while (DMA2_Stream0->CR & DMA_SxCR_EN);

    DMA2_Stream0->PAR  = (uint32_t)&ADC1->DR;
    DMA2_Stream0->M0AR = (uint32_t)&adc_buffer[0];
    DMA2_Stream0->NDTR = ADC_BUFFER_SIZE;

    DMA2_Stream0->CR = 0;
    DMA2_Stream0->CR |= (0 << DMA_SxCR_CHSEL_Pos);  // Canal 0 para ADC1
    DMA2_Stream0->CR |= DMA_SxCR_MINC | DMA_SxCR_CIRC;
    DMA2_Stream0->CR |= DMA_SxCR_PSIZE_0 | DMA_SxCR_MSIZE_0; // 16 bits
    DMA2_Stream0->CR |= DMA_SxCR_TCIE | DMA_SxCR_HTIE;
    DMA2_Stream0->CR |= (2 << DMA_SxCR_PL_Pos);

    DMA2_Stream0->CR |= DMA_SxCR_EN;

    NVIC_SetPriority(DMA2_Stream0_IRQn, 1);
    NVIC_EnableIRQ(DMA2_Stream0_IRQn);

    NVIC_SetPriority(DMA2_Stream7_IRQn, 1);
    NVIC_EnableIRQ(DMA2_Stream7_IRQn);
}

void send_raw_block(uint16_t* start_ptr) {
    // Espera o Stream7 estar livre
    while (DMA2_Stream7->CR & DMA_SxCR_EN);

    // Envia header de sincronização (2 bytes fixos)
    USART1_send(0xAA);
    USART1_send(0x55);

    // Envia bloco via DMA
    USART1_DMA_send((uint8_t*)start_ptr, (ADC_BUFFER_SIZE / 2) * 2);
}

void DMA2_Stream0_IRQHandler(void) {
    if (DMA2->LISR & DMA_LISR_HTIF0) {
        DMA2->LIFCR |= DMA_LIFCR_CHTIF0;
        send_raw_block(&adc_buffer[0]);
    }

    if (DMA2->LISR & DMA_LISR_TCIF0) {
        DMA2->LIFCR |= DMA_LIFCR_CTCIF0;
        send_raw_block(&adc_buffer[ADC_BUFFER_SIZE / 2]);
    }
}

void DMA2_Stream7_IRQHandler(void) {
    if (DMA2->HISR & DMA_HISR_TCIF7) {
        DMA2->HIFCR |= DMA_HIFCR_CTCIF7; // FIM da transmissão
    }
}
