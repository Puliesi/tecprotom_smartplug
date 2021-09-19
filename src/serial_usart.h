/* 
 * File:   serial_usart.h
 * Author: Marcos
 *
 * Created on 19 de Abril de 2016, 01:01
 */

#ifndef SERIAL_USART_H
#define	SERIAL_USART_H

#define TAMANHO_BUFFER_RX = 100;


void USART_init(unsigned long baudrate, unsigned char numBits);
void EnviaDadoUSART(char dado);
void HabilitaUSART(void);
void DesabilitaUSART(void);
unsigned char RecebeDadoUSART(void);
int OverflowUSART(void);
int DadoRecebido(void);
int TxOcupado(void);
void LimpaBufferRxUSART(void);
void enviaStringPelaUart(unsigned char str[100], int tamanho);




#endif	/* SERIAL_USART_H */

