
#include <pic16f877A.h>
#include <pic16f877.h>
//#include <pic18f4520.h>
//#include <pic18.h>
#include <xc.h>
#include "hardware.h"
#include <time.h>


void iniHardware(void){
    
    TRISAbits.TRISA0 = 1; // ENTRADA
    TRISAbits.TRISA1 = 1; // ENTRADA
    TRISAbits.TRISA2 = 1; // ENTRADA
    TRISAbits.TRISA3 = 1; // ENTRADA
    TRISAbits.TRISA5 = 1; // ENTRADA
    //TRISAbits.TRISA6 = 1; // OSC2
    //TRISAbits.TRISA7 = 1; // OSC1
    
    TRISBbits.TRISB0 = 1; // ENTRADA IN SIGNAL FIO LARANJA
    TRISBbits.TRISB1 = 0; // SAIDA DSP_A
    TRISBbits.TRISB2 = 0; // SAIDA DSP_B
    TRISBbits.TRISB3 = 0; // SAIDA DSP_C
    TRISBbits.TRISB4 = 0; // SAIDA DSP_D
    TRISBbits.TRISB5 = 0; // SAIDA DSP_E
    TRISBbits.TRISB6 = 0; // SAIDA DSP_F
    TRISBbits.TRISB7 = 0; // SAIDA DSP_G 
    
    TRISCbits.TRISC0 = 1; // ENTRADA BOTÃO START SCAN
    TRISCbits.TRISC1 = 0; // SAÍDA PWM V- (TENSÃO NEGATIVA GROUND BOLT)
    TRISCbits.TRISC2 = 0; // SAIDA DSP_P
    TRISCbits.TRISC3 = 0; // SAÍDA LED GB OK 
    TRISCbits.TRISC4 = 0; // SAÍDA LED GB NOK 
    TRISCbits.TRISC5 = 0; // SAÍDA LED GB CURTO 
    TRISCbits.TRISC6 = 0; // SAIDA TX1
    TRISCbits.TRISC7 = 1; // ENTRADA RX1
    
    TRISDbits.TRISD0 = 0; // SAIDA VBOOST
    TRISDbits.TRISD1 = 0; // SAÍDA PIC OUT SIGNAL FIO AMARELO
    TRISDbits.TRISD2 = 0; // SAIDA LIGA BOOST
    TRISDbits.TRISD3 = 0; // SAÍDA LED NÍVEL BATERIA
    TRISDbits.TRISD4 = 0; // SAIDA LED TESTE OK
    TRISDbits.TRISD5 = 0; // SAIDA LED TESTE NOK
    TRISDbits.TRISD6 = 0; // SAIDA LED FALHA 20V
    TRISDbits.TRISD7 = 0; // SAIDA LED FALHA 9V
    
    
    TRISEbits.TRISE0 = 1; // 
    TRISEbits.TRISE1 = 1; // 
    TRISEbits.TRISE2 = 1; // 
    

/*   
    LATA0 = 0;
    LATA1 = 0;
    LATA2 = 0;
    LATA3 = 0;
    LATA5 = 0;
    LATA6 = 0;
    LATA7 = 0;
    
    LATB0 = 0;
    LATB1 = 0;
    LATB2 = 0;
    LATB3 = 0;
    LATB4 = 0;
    LATB5 = 0;
    LATB6 = 0;
    LATB7 = 0;
    
    LATC0 = 0;
    LATC1 = 0;
    LATC2 = 0;
    LATC3 = 0;
    LATC4 = 0;
    LATC5 = 1;
    LATC6 = 1;
    LATC7 = 0;


    //ADCON0bits.CHS0 = 0;
    //ADCON0bits.CHS1 = 0;
    //ADCON0bits.CHS2 = 1;
    //ADCON0bits.CHS3 = 1;
    //ADCON0bits.CHS4 = 1;

    //ODCONbits.U1OD = 1;
    //ODCONbits.SSPOD = 1;
*/
    
}

void iniConversorAD(void){
    
    ADCON1bits.ADFM = 1; // JUSTIFICADO A DIREITA
    ADCON1bits.PCFG = 2; // Vref+ = VDD; Vref- = VSS
    //ADCON1bits.ADCS2 = 1; // TEMPO DE CONVERSÃO
    ADCON0bits.ADCS = 2; // TEMPO DE CONVERSÃO

}

void iniTimer1(void){
    T1CONbits.TMR1CS = 0; // incremento por ciclo de máquina
    T1CONbits.T1CKPS = 0; // 1:8
    //TMR1L = 155; // AJUSTE PARA OCORRER INTERRUPÇÃO A CADA 1ms
    //TMR1H = 245; // AJUSTE PARA OCORRER INTERRUPÇÃO A CADA 1ms
    T1CONbits.TMR1ON = 1; // LIGA TIMER
    PIE1bits.TMR1IE = 1; // habilita interrupção
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1; // INTERRUPÇÃO GLOBAL ATIVADA    
}

void iniUart(void){
    TXSTAbits.BRGH = 1;
    TXSTAbits.SYNC = 0;
    //SPBRG = 25; // SPBRG = (Fosc/(16 x BR)) - 1  -> Baudrate = 9600 (Fosc = 4MHz)
    //SPBRG = 103; // SPBRG = (Fosc/(16 x BR)) - 1  -> Baudrate = 9600 (Fosc = 16MHz)
    SPBRG = 129; // SPBRG = (Fosc/(16 x BR)) - 1  -> Baudrate = 9600 (Fosc = 20MHz)
    //SPBRG = (_XTAL_FREQ / (16 * 9600)) - 1; 
    RCSTAbits.SPEN = 1;
    TXSTAbits.TXEN = 1;
    TXSTAbits.TX9 = 0;
    RCSTAbits.RX9 = 0;
    RCSTAbits.ADDEN = 0;
    PIE1bits.RCIE = 1;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;    
}

void INT0_init(void){
    INTCONbits.INTE = 1; //  Enables the RB0/INT external interrupt
    OPTION_REGbits.INTEDG = 1; //Interrupt on rising edge of RB0/INT pin
    INTCONbits.PEIE = 1; 
    INTCONbits.GIE = 1;  
}


void liga_PWM2(unsigned char freq_pwm){
    unsigned char dutyCycle = 90;
    unsigned char freqPWM;
        
    freqPWM = freq_pwm;
    //TRISCbits.TRISC1 = 0; // Pino CCP2 como saída
    CCP2CON = 0x0F; // Seleciona Modo PWM
    PR2 = 180;
    CCPR2L = dutyCycle;
    T2CONbits.T2CKPS = 0; // PRESCALER 1:16
    T2CONbits.TOUTPS = 0; //POSTCALE 1:1
    TMR2ON = 1; // Inicia Timer2 para geração do PWM     
}

void desligaPWM2(void){
    CCP2CONbits.CCP2M = 0x00; // PWM LIGADO
}




unsigned int obtemConvercaoAD(char canal){
    unsigned int valor;
        
    
    INTCONbits.GIE = 0; // INTERRUPÇÃO GLOBAL ATIVADA 
    valor = 0;
    ADCON1bits.ADFM = 1; // JUSTIFICADO A DIREITA
    ADCON0bits.CHS = canal; // selecionar canal
    ADCON0bits.ADON = 1; // LIGA CONVERSOR ANALOGICO DIGITAL
    delay_us(10); // tempo para adequação do capacitor de conversão    
    ADCON0bits.GO_DONE = 1; // inicia converssao
    while(ADCON0bits.GO_DONE); // aguarda termninar conversão

    valor = ADRESH;
    valor = (valor << 8) | ADRESL ;
        
    ADCON0bits.ADON = 0; // desliga conversor para economizar bateria
    INTCONbits.GIE = 1; // INTERRUPÇÃO GLOBAL ATIVADA 
    return valor; // retorna com valor convertido    
}



void delay_ms(unsigned int valor){//32
    unsigned int contador;
    for(contador = 0; contador < valor; contador = contador + 1){
        //_delay(1);
        __delay_ms(1);
    }
}

void delay_us(unsigned int valor){//32
    unsigned int contador;
    for(contador = 0; contador < valor; contador = contador + 1){
        //_delay(1);
        __delay_us(1);
    }
}



