/*
 * serial.c
 *
 *  Created on: 4 Jan 2016
 *      Author: Mike
 *
 *      This (ab)uses the STMCubeMX HAL declarations to implement a generic STM32F1xx
 *      USART driver layer
 *
 * Note - this requires the UART interrupt to be globally enabled in STM32CubeMX,
 * but not to generate IRQ handler or HAL driver for the UART.
 *
 *
 */

#include "libSerial/serial.h"

/* workspaces for the USARTS being used */

uint16_t
SerialCharsReceived(struct usart_ctl *instance)
{
  uint16_t result = 0; // assume no characters received yet

  __HAL_UART_DISABLE_IT(instance->Handle, UART_IT_RXNE);

  if (instance->rx_usart_buffer_full)
  { // buffer is full...
    result = instance->rx_usart_buff_size;
  }
  else if (instance->rx_usart_in_Ptr >= instance->rx_usart_out_Ptr)
  { // buffer has not wrapped...
    result = instance->rx_usart_in_Ptr - instance->rx_usart_out_Ptr;
  }
  else
  { // buffer has possibly wrapped...
    result = instance->rx_usart_buff_size - instance->rx_usart_out_Ptr + instance->rx_usart_in_Ptr;
  }
  __HAL_UART_ENABLE_IT(instance->Handle, UART_IT_RXNE);

  return result;
}

uint16_t SerialTransmitSpace(struct usart_ctl *instance)
{
  __HAL_UART_DISABLE_IT(instance->Handle, UART_IT_TXE);
  uint16_t result;
  if (instance->tx_usart_in_Ptr == instance->tx_usart_out_Ptr)
  {
    result = instance->tx_usart_running ? 0 : instance->tx_usart_buff_size;
  }
  else if (instance->tx_usart_in_Ptr > instance->tx_usart_out_Ptr)
  { // buffer has not wrapped...
    result = instance->tx_usart_buff_size - instance->tx_usart_in_Ptr - instance->tx_usart_out_Ptr;
  }
  else
  { // buffer has possibly wrapped...
    result = instance->tx_usart_buff_size - instance->tx_usart_out_Ptr + instance->tx_usart_in_Ptr;
  }

  if (instance->tx_usart_running)
    __HAL_UART_ENABLE_IT(instance->Handle, UART_IT_TXE);
  return result;
}

inline uint8_t
PollSerial(struct usart_ctl *instance)
{
  uint8_t rc;
  __HAL_UART_DISABLE_IT(instance->Handle, UART_IT_RXNE);
  rc = (instance->rx_usart_buffer_full || (instance->rx_usart_in_Ptr != instance->rx_usart_out_Ptr));
  __HAL_UART_ENABLE_IT(instance->Handle, UART_IT_RXNE);

  return rc;
}

inline uint8_t
GetCharSerial(struct usart_ctl *instance)
{
  uint8_t c;
  __HAL_UART_DISABLE_IT(instance->Handle, UART_IT_RXNE);
  while (!instance->rx_usart_buffer_full && (instance->rx_usart_in_Ptr == instance->rx_usart_out_Ptr))
  {
    __HAL_UART_ENABLE_IT(instance->Handle, UART_IT_RXNE);
    __WFI(); /* wait for something */
    __HAL_UART_DISABLE_IT(instance->Handle, UART_IT_RXNE);
  }

  c = instance->rx_usart_buff[instance->rx_usart_out_Ptr];
  instance->rx_usart_buffer_full = 0; /* removed character */
  instance->rx_usart_out_Ptr++;
  if (instance->rx_usart_out_Ptr >= instance->rx_usart_buff_size)
  {
    instance->rx_usart_out_Ptr = 0;
  }
  __HAL_UART_ENABLE_IT(instance->Handle, UART_IT_RXNE);
  return c;
}

/*
 * \brief
 * void EnableSerialRxInterrupt(void) - this function is used from the interrupt handler and the main scheduler loop
 * to enable the serial rx interrupt after resetting the serial rx buffer...
 */
inline void
EnableSerialRxInterrupt(struct usart_ctl *instance)
{
  /* cheat here - this is a macro and I have the same Instance member as the HAL handle, with the same meaning */
  __HAL_UART_ENABLE_IT(instance->Handle, UART_IT_RXNE);
}

/****!
 * @brief send a character to the serial USART via placing it in the serial buffer
 *
 */

static void
PutCharSerialFIFO(struct usart_ctl *instance, uint8_t c)
{
  __HAL_UART_DISABLE_IT(instance->Handle, UART_IT_TXE);

  instance->tx_usart_buff[instance->tx_usart_in_Ptr++] = c;
  if (instance->tx_usart_in_Ptr >= instance->tx_usart_buff_size)
  {
    instance->tx_usart_in_Ptr = 0;
  }
  /* Handle overrun by losing oldest characters */
  if (instance->tx_usart_in_Ptr == instance->tx_usart_out_Ptr)
  {
    instance->tx_usart_overruns++;
    instance->tx_usart_out_Ptr++;
    if (instance->tx_usart_out_Ptr >= instance->tx_usart_buff_size)
    {
      instance->tx_usart_out_Ptr = 0;
    }
  }

  instance->tx_usart_running = 1;
  __HAL_UART_ENABLE_IT(instance->Handle, UART_IT_TXE);
}

void UART_IRQHandler(struct usart_ctl *instance)
{

  __disable_irq();
  // status from USART receiver
  uint32_t rxStatus = instance->Handle->Instance->SR; // read the status bits - this resets all the hardware signalling flags

  if ((rxStatus & USART_SR_LBD))
    __HAL_UART_CLEAR_FLAG(instance->Handle, USART_SR_LBD);

  if ((rxStatus & USART_SR_RXNE) != RESET)
  {
    // no error has occurred...
    uint8_t rxChar = (uint8_t)(instance->Handle->Instance->DR & 0xff); // read the bottom 8-bits only

    if (!instance->rx_usart_buffer_full)
    {
      instance->rx_usart_buff[instance->rx_usart_in_Ptr++] = rxChar;

      if (instance->rx_usart_in_Ptr >= instance->rx_usart_buff_size)
      {
        instance->rx_usart_in_Ptr = 0;
      }
      if (instance->rx_usart_in_Ptr == instance->rx_usart_out_Ptr)
      {
        instance->rx_usart_buffer_full = 1; /* buffer overrun */
      }
    }
    // perform callback if required : note in interrupt handler here
    if (instance->rx_callback)
      instance->rx_callback(instance->rx_context);
  }
  /* check for transmitter interrupt : this code is used */
  if ((rxStatus & USART_SR_TXE) != RESET)
  {

    /* Only enable the transmitter when baud detect has completed or check expired.
     * and the software is ready for it to be enabled as programming mode is wanting
     * to receive a response and that can get blocked if we're streaming a lot of debug messages*/
    if (instance->tx_usart_in_Ptr != instance->tx_usart_out_Ptr)
    {
      instance->Handle->Instance->DR =
          instance->tx_usart_buff[instance->tx_usart_out_Ptr++];

      if (instance->tx_usart_out_Ptr >= instance->tx_usart_buff_size)
      {
        instance->tx_usart_out_Ptr = 0;
      }
    }
    if (instance->tx_usart_in_Ptr == instance->tx_usart_out_Ptr)
    {
      __HAL_UART_DISABLE_IT(instance->Handle, UART_IT_TXE);
      instance->tx_usart_running = 0;
    }
  }
  __enable_irq();
}

void PutCharSerial(struct usart_ctl *instance, uint8_t c)
{
  PutCharSerialFIFO(instance, c);
}

void ResetTxBuffer(struct usart_ctl *instance)
{

  instance->tx_usart_out_Ptr = 0;
  instance->tx_usart_running = 0;
  instance->tx_usart_in_Ptr = 0; /* setup in pointer last to drop any chars come in */
  instance->tx_usart_overruns = 0;
}

/*
 * \brief
 * void ResetRxBuffer(void) - resets the serial receiver buffer
 */
void ResetRxBuffer(struct usart_ctl *instance)
{

  instance->rx_usart_out_Ptr = 0;
  instance->rx_usart_buffer_full = 0;
  instance->rx_usart_in_Ptr = 0; /* setup in pointer last to drop any chars come in */
}

/***!
 * @brief Flush Serial input and output buffers
 */
void FlushSerial(struct usart_ctl *instance)
{
  ResetRxBuffer(instance);
  ResetTxBuffer(instance);
}

/***!
 * @brief check if tx buffer is empty...
 */
uint8_t
TxBufferEmpty(struct usart_ctl *instance)
{
  if (instance->tx_usart_running)
    return 0;
  return (instance->Handle->Instance->SR & USART_SR_TC) == 0;
}

/***!
 * @brief wait for transmission to finish
 */

void TxWaitEmpty(struct usart_ctl *instance)
{
  // no wait if not running
  if (!instance->tx_usart_running)
    return;
  // wait for it to finish
  while (instance->tx_usart_running ||
         (instance->Handle->Instance->SR & USART_SR_TC) != RESET)
  {
  };
}

/****
 * @brief Initialise control structure
 */
void init_usart_ctl(struct usart_ctl *instance, UART_HandleTypeDef *handle,
                    volatile uint8_t *tx_buffer,
                    volatile uint8_t *rx_buffer,
                    uint16_t tx_buffer_size,
                    uint16_t rx_buffer_size)
{

  instance->Handle = handle;
  instance->tx_usart_buff = tx_buffer;
  instance->rx_usart_buff = rx_buffer;
  instance->tx_usart_buff_size = tx_buffer_size;
  instance->rx_usart_buff_size = rx_buffer_size;
  instance->rx_callback = NULL;
  instance->rx_context = NULL;

  /* cheat here - this is a macro and I have the same Instance member as the HAL handle, with the same meaning */
  __HAL_UART_DISABLE_IT(instance->Handle, UART_IT_TXE);
  __HAL_UART_DISABLE_IT(instance->Handle, UART_IT_RXNE);

  FlushSerial(instance);
}

void setBaud(struct usart_ctl *ctl, uint32_t baud)
{
  ctl->Handle->Init.BaudRate = baud;
  __disable_irq();
  HAL_UART_Init(ctl->Handle);
  __enable_irq();
}

void setRxCallback(struct usart_ctl *ctl, void (*fn)(void *), void *arg)
{
  __disable_irq();
  ctl->rx_context = arg;
  ctl->rx_callback = fn;
  __enable_irq();
}
