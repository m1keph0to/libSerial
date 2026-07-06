#pragma once 
#include <stdint.h>
#if defined __cplusplus
extern "C"
{
#endif 


///@brief  returns the number of characters in the recieve buffer
///@param instance Pointer to usart_ctl structure
///@return Number of characters
extern uint16_t SerialCharsReceived(struct usart_ctl *instance);

///@brief Get the amount of free space in the transmit buffer.
///@param instance Pointer to usart_ctl structure
///@return Free space at time of checking 
extern uint16_t SerialTransmitSpace(struct usart_ctl *instance);

/// @brief Return 1 if there are any characters in the receive buffer
/// @param instance Pointer to usart_ctl structure
/// @return 1 if any characters 
extern uint8_t PollSerial(struct usart_ctl *instance);

/// @brief return the next character in the Serial input buffer
/// This function will wait until a character arrives.
/// Use PollSerial() or SerialCharsReceived() if you want
/// to ensure you do not block here.
/// @param instance Pointer to usart_ctl structure 
/// @return next character 
extern uint8_t GetCharSerial(struct usart_ctl *instance);

/// @brief Enable the receive interrupt 
/// @param instance 
extern void EnableSerialRxInterrupt(struct usart_ctl *instance);

/// @brief Send a character
/// @param instance Pointer to usart_ctl structure
/// @param c character to send
extern void PutCharSerial(struct usart_ctl *instance, uint8_t c);

/// @brief Reset transmit buffer
/// @param instance Pointer to usart_ctl structure
extern void ResetTxBuffer(struct usart_ctl *instance);

/// @brief Reset receive buffer
/// @param instance Pointer to usart_ctl structure
extern void ResetRxBuffer(struct usart_ctl *instance);

/// @brief Reset both transmit and receive buffers
/// @param instance Pointer to usart_ctl structure
extern void FlushSerial(struct usart_ctl *instance);

/// @brief Check if the transmitter buffer is empty
/// @param instance Pointer to usart_ctl structure
/// @return 1 if the buffer is empty
extern uint8_t TxBufferEmpty(struct usart_ctl *instance);

///@brief  wait until the USART buffer is empty and all characters are sent
/// @param instance Pointer to usart_ctl structure
extern void TxWaitEmpty(struct usart_ctl *instance);


/// @brief Set the baud rate on this instance
/// @param instance  Pointer to usart_ctl structure
/// @param baud Baud rate in bits per second
extern void setBaud(struct usart_ctl *instance, uint32_t baud);


/// @brief Set the receiver interrupt callback
/// @param ctl USART control block 
/// @param fn Function pointer
/// @param arg Callback value 
extern void setRxCallback(struct usart_ctl * ctl, void (*fn)(void *), void * arg);

#if defined __cplusplus
}
#endif
