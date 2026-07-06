# libSerial
A small serial library that uses the STM32Cube HAL with USARTS/UARTS but implements circular buffers for read and write operations. It has survived running at 1Mbit/sec on an STM32F103 with CAN bus also running alongside it. 

## Static library build 
If you use the Makefile, it builds a static library.
## Makefile fragment 

The libSerial.mk integrates with a larger build system. Loading this fragment sets up various build target elements for a Makefile. 
Configuration options defined on the compiler command line define which USARTS will be controlled. 

## STMCubeMx options
It is necessary to tell STM32CubeMX to enable interrupts but not generate the handlers for USARTS that are being controlled by this library. 
