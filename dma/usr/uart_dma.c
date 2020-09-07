#include "uart_dma.h"

#include "usart.h"
#include <stdio.h>


#define RX_BUF_NUM 36u


extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;


uint8_t cache[RX_BUF_NUM]={0};
uint8_t RecvData[RX_BUF_NUM]={0};
int fputc(int ch, FILE *f)
{
	
	HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);	
	
	return (ch);
}


int fgetc(FILE *f)
{
		
	int ch;
	HAL_UART_Receive(&huart1, (uint8_t *)&ch, 1, 1000);	
	return (ch);
}


void DMA_config(uint8_t *rx_buf, uint16_t dma_buf_num)
{
   
    SET_BIT(huart1.Instance->CR3, USART_CR3_DMAR);
	
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    __HAL_DMA_DISABLE(&hdma_usart1_rx);
	
    while(hdma_usart1_rx.Instance->CR & DMA_SxCR_EN)
    {
        __HAL_DMA_DISABLE(&hdma_usart1_rx);
    }

    hdma_usart1_rx.Instance->PAR = (uint32_t) & (USART1->DR);
    
    hdma_usart1_rx.Instance->M0AR = (uint32_t)(rx_buf);
	hdma_usart1_rx.Instance->M1AR = (uint32_t)(rx_buf);
    
    hdma_usart1_rx.Instance->NDTR = dma_buf_num;
   
    SET_BIT(hdma_usart1_rx.Instance->CR, DMA_SxCR_DBM);

    __HAL_DMA_ENABLE(&hdma_usart1_rx);
}


void UART_DMA(void)
{
	DMA_config(&cache[0], RX_BUF_NUM);
}




static void convert_uart(volatile const uint8_t *cache_buf, uint8_t *VisionRecvData)
{
    
	
}



void USART1_IRQHandler(void)
{
    if(huart1.Instance->SR & UART_FLAG_RXNE)//接收到数据
    {
        __HAL_UART_CLEAR_PEFLAG(&huart1);
    }
    else if(USART1->SR & UART_FLAG_IDLE)
    {
        static uint16_t this_time_rx_len = 0;

        __HAL_UART_CLEAR_PEFLAG(&huart1);

       
		__HAL_DMA_DISABLE(&hdma_usart1_rx);

		this_time_rx_len = RX_BUF_NUM - hdma_usart1_rx.Instance->NDTR;

		hdma_usart1_rx.Instance->NDTR = RX_BUF_NUM;

		DMA1_Stream1->CR &= ~(DMA_SxCR_CT);

		__HAL_DMA_ENABLE(&hdma_usart1_rx);

		if(this_time_rx_len == 15)
		{
			convert_uart(&cache[0], &RecvData[0]);
		}
    }
}



