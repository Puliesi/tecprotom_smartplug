/* 
 * File:   hardware.h
 * Author: Marcos
 *
 * Created on 20 de Fevereiro de 2020, 22:52
 */
//******************************************************************************


#ifndef HARDWARE_H
#define	HARDWARE_H

#include <pic16f877A.h>
#include <pic16f877.h>
//#include <pic18f4520.h>
//#include <pic18.h>
//#include <p18cxxx.h>
#include <xc.h>

#define _XTAL_FREQ 20000000


#define __delay_us(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000000.0)))
#define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))


// *****************************************************************************
//  SETA DADOS NA EEPROM
// *****************************************************************************

__EEPROM_DATA(1,0,0,0,0,0,0,0); //0x00 ... 0x07
//__EEPROM_DATA(0,0,0,0,0,0,0,0); //0x08 ... 0x0F
//...
//...
//...

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT enabled)
#pragma config CP = OFF         // FLASH Program Memory Code Protection bits (Code protection off)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low Voltage In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EE Memory Code Protection (Code Protection off)
//#pragma config WRT = ON         // FLASH Program Memory Write Enable (Unprotected program memory may be written to by EECON control)


//******************************************************************************
// DEFINIÇÃO DAS CONSTANTES                      
//******************************************************************************

#define     VREF_SENSORES       PORTAbits.RA0       
#define     VREF_BATERIA        PORTAbits.RA1       
#define     VREF_GROUND_BOLT    PORTAbits.RA2       
#define     IMPUT_RA3           PORTAbits.RA3       
#define     IMPUT_RA5           PORTAbits.RA4       

#define     ENTRADA_PULSADA     PORTBbits.RB0                 
#define     DISPLAY_A           RB1                 
#define     DISPLAY_B           RB2                
#define     DISPLAY_C           RB3                 
#define     DISPLAY_D           RB4                 
#define     DISPLAY_E           RB5                 
#define     DISPLAY_F           RB6                 
#define     DISPLAY_G           RB7                 

#define     BT_START_SCAN       PORTCbits.RC0 
#define     SAIDA_PWM           RC1
#define     DISPLAY_P           RC2
#define     LED_GB_OK           RC3
#define     LED_GB_NOK          RC4                          
#define     LED_GB_CURTO        RC5
#define     USART_TX1           RC6                 
#define     USART_RX1           PORTCbits.RC7       

#define     SEL_9V              RD0
#define     SAIDA_PULSADA       RD1
#define     SEL_20V             RD2
#define     LED_NIVEL_BATERIA   RD3
#define     LED_TESTE_OK        RD4
#define     LED_TESTE_NOK       RD5
#define     LED_FALHA_20V       RD6
#define     LED_FALHA_9V        RD7

//#define                       RE0
//#define                       RE1
//#define                       RE1






//******************************************************************************
// DEFINIÇÃO GERAIS
//******************************************************************************
#define     OFF     0;
#define     ON      1;

#define     CANAL_AD_SENSORES       0
#define     CANAL_AD_BATERIA        1
#define     CANAL_AD_GROUND_BOLT    2
#define     CANAL_AD_AMPLITUDE      3

//******************************************************************************
// FUNÇÕES
//******************************************************************************

void iniHardware(void);
void iniTimer1(void);
void iniUart(void);
void iniConversorAD(void);
void liga_PWM2(unsigned char freq_pwm);
void desligaPWM2(void);
void INT0_init(void);



unsigned int obtemConvercaoAD(char canal);
void delay_ms(unsigned int valor);
void delay_us(unsigned int valor);


#endif	/* HARDWARE_H */

