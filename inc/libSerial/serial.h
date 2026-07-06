/*
 * serial.h
 *
 *  Created on: 4 Jan 2016
 *      Author: Mike
 */



/* Includes ------------------------------------------------------------------*/
#ifdef STM32L152xE
#include "stm32l1xx_hal.h"
#else
#include "stm32f1xx_hal.h"
#endif


#pragma once 

#include <stdint.h>

#if defined __cplusplus
extern "C"
{
#endif 


struct usart_ctl 
{
	/// @brief UART handle
	UART_HandleTypeDef *Handle;
	/// @brief Locally maintained transmit buffer
	
	volatile uint8_t * tx_usart_buff;
	uint16_t  tx_usart_buff_size;
	uint16_t rx_usart_buff_size; 
	volatile uint16_t tx_usart_in_Ptr;
	volatile uint16_t tx_usart_out_Ptr;
	volatile uint16_t tx_usart_overruns; 
	/// @brief Set when the usart is transmitting
	volatile uint8_t tx_usart_running;
	
	/// @brief Locally maintained receive buffer
	volatile uint8_t * rx_usart_buff;
	volatile uint16_t rx_usart_in_Ptr;
	volatile uint16_t rx_usart_out_Ptr;
	/// @brief Set when the receiver buffer is full
	volatile uint8_t rx_usart_buffer_full;
	/// @brief RX callback function called from inside Rx ISR 
	void (*rx_callback)(void * context);
	/// @brief Callback context 
	void * rx_context; 
} ;

 

#if defined SERIAL_UART1
extern struct usart_ctl uc1;
#endif
#if defined SERIAL_UART2
extern struct usart_ctl uc2;
#endif
#if defined SERIAL_UART3
extern struct usart_ctl uc3;
#endif
#if defined SERIAL_UART4
extern struct usart_ctl uc4;
#endif
#if defined SERIAL_UART5
extern struct usart_ctl uc5;
#endif
/// @brief Establish this instance as being in control of a USART
/// @param instance  Pointer to usart_ctl structure
/// @param usart Handle to usart maintained by HAL
/// @param tx_buffer Statically allocated memory for Tx buffer
/// @param rx_buffer Statically allocated memory for Rx buffer
/// @param rx_buffer_size Rx Buffer size
/// @param tx_buffer_size Tx Buffer size
extern void init_usart_ctl(struct usart_ctl *instance,
						   UART_HandleTypeDef *usart,
						   volatile uint8_t * tx_buffer,
						   volatile uint8_t * rx_buffer,
						   uint16_t tx_buffer_size,
						   uint16_t rx_buffer_size);



#if defined __cplusplus
}
#endif

#include "serialCalls.h"

