/* 
 * File:   SoftUART.h
 * Author: Eng. Juan Camilo Gómez Cadavid
 *
 * This is a generic software UART which can be implemented on any MCU with a C compiler.
 * It requires a timer interrupt to be set to 3 times the baud rate (SofUARTTick), and two software
 * controlled pins for the receive and transmit functions.
 *
 *                      1000000
 *  SofUARTTick(uS) = -----------
 *                    (3*baudrate)
 *
 * Configure TX pin as output with a high state initialized
 */

#ifndef SOFTUART_Ht
#define	SOFTUART_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdlib.h>

#define     SOFTUART_DATABITS   8
#define     SUART_HIGH          (1)
#define     SUART_LOW           (0)

typedef union{
    struct{
        unsigned    TXREADY :1;
        unsigned    RXREADY :1;
        unsigned    TXENABLE:1;
        unsigned    RXENABLE:1;
        unsigned    RXERROR:1;
        unsigned    RXWAITING_STOPBIT:1;
    };
    unsigned char AllFlags;
}SoftUARTFlags_T;

typedef struct{
    void (*rx_isr_fcn)(char c);
    unsigned char (*rxget_fcn)(void);
    void (*txhigh_fcn)(void);
    void (*txlow_fcn)(void);
    void (*txctrl_fcn)(unsigned char);
    unsigned char TXBuffer;
    unsigned char RXBuffer;
    unsigned char RXData;
    union{
        unsigned char TXState;       
        unsigned char TXBitIndex;
    };
    unsigned char txtic,rxtic;
    unsigned char rxmask;
    unsigned char bits_left_in_rx;
}SoftUARTDrvData_t;

#define     SUART_BEGIN_TX              (0xFE)
#define     SUART_STOP_TX               (0xFA)
#define     SUART_DATA_TX               (0)

extern volatile SoftUARTFlags_T     SUARTFlags;
extern volatile SoftUARTDrvData_t   SUARTDrvDat;

void SoftUART_Init(void (*txctrl_fcn)(unsigned char), unsigned char (*rx_status)(void), void (*rx_isr_fcn)(char));
void SoftUART_ISRHandler(void);
char SoftUART_GetByte(void);
void SoftUART_PutByte(char ch);
void SoftUART_PutString(const char *s);
unsigned char SoftUART_GetByte(void);

#ifdef	__cplusplus
}
#endif

#endif	/* SOFTUART_H */

