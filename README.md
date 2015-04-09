# EC-SoftUART
 *
 * This is a generic software UART which can be implemented on any MCU with a C compiler.
 * It requires a timer interrupt to be set to 3 times the baud rate (SofUARTTick), and two software
 * controlled pins for the receive and transmit functions.
 *
 *                      
 *  SofUARTTick(uS) = 1000000/(3*baudrate)
 *                    
 *
 * Configure TX pin as output with a high state initialized
