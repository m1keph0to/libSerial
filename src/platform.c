

#include "libSerial/serial.h"

/// @brief Interrupt handler generic wrapper
/// @param instance Pointer to usart_ctl structure
extern void UART_IRQHandler(struct usart_ctl *instance);

/////////////////////////////////////////////////////////
/// Moved from generated code  to avoid crappy HAL handler
#if defined SERIAL_UART1

struct usart_ctl uc1;
void USART1_IRQHandler(void)
{
  UART_IRQHandler(&uc1);
}
#endif
#if defined SERIAL_UART2
struct usart_ctl uc2;
void USART2_IRQHandler(void)
{
  UART_IRQHandler(&uc2);
}
#endif
#if defined SERIAL_UART3
struct usart_ctl uc3;
void USART3_IRQHandler(void)
{
  UART_IRQHandler(&uc3);
}
#endif
#if defined SERIAL_UART4
struct usart_ctl uc4;
void UART4_IRQHandler(void)
{
  UART_IRQHandler(&uc4);
}
#endif
#if defined SERIAL_UART5
struct usart_ctl uc5;
void UART5_IRQHandler(void)
{
  UART_IRQHandler(&uc5);
}
#endif
