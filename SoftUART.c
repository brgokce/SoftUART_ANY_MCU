/*
 * File:   SoftUART.h
 * Author: Eng. Juan Camilo Gómez Cadavid
 */

#include "SoftUART.h"

volatile SoftUARTFlags_T     SUARTFlags;
volatile SoftUARTDrvData_t   SUARTDrvDat;
/*============================================================================*/
void SoftUART_Init(void (*txctrl_fcn)(unsigned char), unsigned char (*rx_status)(void), void (*rx_isr_fcn)(char)){
    SUARTDrvDat.rx_isr_fcn = rx_isr_fcn;
    SUARTDrvDat.rxget_fcn = rx_status;
    SUARTDrvDat.txctrl_fcn = txctrl_fcn;
    SUARTFlags.AllFlags = 0;
    SUARTFlags.TXREADY = 0;
    SUARTFlags.TXENABLE = 1;
    SUARTFlags.RXENABLE = 1;
    SUARTDrvDat.TXBuffer = 0x00;
    SUARTDrvDat.TXState = SUART_BEGIN_TX;
    SUARTDrvDat.txtic  = 0;
    SUARTFlags.RXWAITING_STOPBIT = 0;
}
/*============================================================================*/
void SoftUART_ISRHandler(void){
    if(SUARTFlags.TXREADY){ //transmision
        if (++SUARTDrvDat.txtic<3) goto END_TX_STATE;
        if (SUARTDrvDat.TXState >= SUART_BEGIN_TX){
            SUARTDrvDat.txctrl_fcn(SUART_LOW);
            SUARTDrvDat.TXState = SUART_DATA_TX;
            goto END_TX_STATE;
        }
        if (SUARTDrvDat.TXState < SOFTUART_DATABITS){ // data transmision
            if( ((SUARTDrvDat.TXBuffer>>SUARTDrvDat.TXBitIndex)&0x01) ) SUARTDrvDat.txctrl_fcn(SUART_HIGH);//SUARTDrvDat.txhigh_fcn();
            else    SUARTDrvDat.txctrl_fcn(SUART_LOW);
            SUARTDrvDat.TXBitIndex++;
            if(SUARTDrvDat.TXBitIndex>=SOFTUART_DATABITS)   SUARTDrvDat.TXState = SUART_STOP_TX;
            goto END_TX_STATE;
        }
        if (SUARTDrvDat.TXState == SUART_STOP_TX){
            SUARTDrvDat.txctrl_fcn(SUART_HIGH);
            SUARTFlags.TXREADY = 0;
            goto END_TX_STATE;
        }
        END_TX_STATE: if(SUARTDrvDat.txtic>=3) SUARTDrvDat.txtic=0;
    }
    /* receiver section ----------------------------------------------------------------------*/
    if (SUARTDrvDat.rx_isr_fcn==NULL) return;
    if (SUARTFlags.RXWAITING_STOPBIT){
        if (--SUARTDrvDat.rxtic<=0 ){
            SUARTFlags.RXWAITING_STOPBIT = 0;
            SUARTDrvDat.RXBuffer &= 0xFF;
            SUARTDrvDat.RXData = SUARTDrvDat.RXBuffer;
            SUARTDrvDat.rx_isr_fcn(SUARTDrvDat.RXData);
            SUARTFlags.RXREADY = 0;
        }
    }
    else{
        if ( SUARTFlags.RXREADY==0 ){
            if (SUARTDrvDat.rxget_fcn()==SUART_LOW){ //check for start bit
                 SUARTFlags.RXREADY = 1;
                 SUARTDrvDat.RXBuffer = 0;
                 SUARTDrvDat.rxtic = 4;
                 SUARTDrvDat.bits_left_in_rx = SOFTUART_DATABITS;
                 SUARTDrvDat.rxmask = 0x01;
            }
        }
        else{
            if ( --SUARTDrvDat.rxtic<=0 ){
                SUARTDrvDat.rxtic = 3;
                if(SUARTDrvDat.rxget_fcn()==SUART_HIGH) SUARTDrvDat.RXBuffer |= SUARTDrvDat.rxmask;
                SUARTDrvDat.rxmask<<=1;
                if ( --SUARTDrvDat.bits_left_in_rx<=0 ) SUARTFlags.RXWAITING_STOPBIT = 1;
            }
        }
    }
}
/*============================================================================*/
void SoftUART_PutByte(unsigned char data){
    while ( SUARTFlags.TXREADY){} // wait until previous completes
    SUARTDrvDat.TXBuffer = data;
    SUARTDrvDat.TXState = SUART_BEGIN_TX;
    SUARTFlags.TXREADY = 1;
}
/*============================================================================*/
void SoftUART_PutString(const char *s){
    unsigned char i=0;
    while(s[i])
    SoftUART_PutByte(s[i++]);
}
/*============================================================================*/
unsigned char SoftUART_GetByte(void){
    while(SUARTFlags.RXREADY){}
    return (SUARTDrvDat.RXData);
}
/*============================================================================*/