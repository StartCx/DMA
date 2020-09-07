#ifndef __UART_DMA_H
#define	__UART_DMA_H

#include "uart_dma.h"
#include "usart.h"
#include <stdio.h>




void DMA_config(uint8_t *rx_buf, uint16_t dma_buf_num);
void UART_DMA(void);

#endif 

