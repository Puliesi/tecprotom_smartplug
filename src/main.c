
/* 
 * File:   main.c
 * Author: Marcos
 *
 * Created on 20 de Fevereiro de 2020, 22:48
 */
//******************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <pic16f877A.h>
#include <pic16f877.h>
//#include <pic18f4520.h>
#include <time.h>
#include <xc.h>
//#include <pic18.h>
#include "hardware.h"
#include "serial_usart.h"
#include <string.h>


/*
// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
//#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = ON         // Data EEPROM Memory Code Protection bit (Data EEPROM code-protected)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
//#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)
*/

#define DEBUG               1


#define STANDBY             0
#define TESTANDO            1
#define TESTANDO_EM_9V      2
#define TESTANDO_EM_20V     3

#define OUT9VOLTS              9
#define OUT20VOLTS             20

#define SAIDA_9_VOLTS       1
#define SAIDA_20_VOLTS      0
#define DESLIGA_BOOST       0
#define LIGA_BOOST          1
#define BAIXO               0
#define ALTO                1
#define FLAGBAT_VERMELHO    1
#define FLAGBAT_VERDE       0
#define END_MEM_FLAGBAT     0
#define LIMITE_DE_PULSO     10
#define PERIODO_PULSO_ALTO  1000
#define PERIODO_PULSO_BAIXO 70000


char statusScan;
char numeroDeSensores;
char flagErroDeLeitura;
unsigned int pulsosSemResposta;
unsigned int numeroDePulsos;
char respostaOk;
char falhaPulsoDeEntrada;
unsigned char flagNivelDeBateria;
unsigned int periodoDoPulso;
char tensaoDeSaida;
char flagErrroDeAmplitude;

char quantidadeDeSensoresEm9V;
char quantidadeDeSensoresEm20V;

char estadoDoSinal;
char contadorDeErros;

unsigned int tempoDeReacaoDoSensor;
char flagTempoDeReacaoDoSensor;



void testaSaidaPulsadaEm9Volts(void);
void testaSaidaPulsadaEm20Volts(void);
void operacoesEmTick(void);
char lerBotao1(void);
void giraDisplay(void);
void obtemQuantidadeDeSensores(void);
void obtemNivelDeBateria(void);
void obtemTesteDoGoundBolt(void);

void ligaLEDVermelhoBateria(void);
void ligaLEDVerdeBateria(void);


unsigned int ObtemAmplitureDoSinal(void);

void exibeQuantidadeDeSensores(char quantidade);
void exibeNumero0(void);
void exibeNumero1(void);
void exibeNumero2(void);
void exibeNumero3(void);
void exibeNumero4(void);
void exibeNumero5(void);
void exibeNumero6(void);
void exibeNumero7(void);
void exibeNumero8(void);
void exibeNumero9(void);
void exibeNumeroA(void);
void exibeNumeroB(void);
void exibeNumeroC(void);
void exibeNumeroD(void);
void exibeNumeroE(void);
void exibeNumeroF(void);
void exibeNumeroNenhum(void);
void exibeNumeroP(void);

void inicializaDisplay(void);
void ligaPainelLed(void);
void desligaPainelLed(void);

void ligaModuloBoost(void);
void desligaModuloBoost(void);
void setaSaidaEm9Volts(void);
void setaSaidaEm20Volts(void);
void trataEntradaPulsada(void);
void executaTesteDosSensores(void);
void atualizaPainelDeLed(void);

void entraEmModoAjuste(void);
char checaBotaoAjustes(void);
void modoTeste9(void);
void modoTeste20(void);


void interrupt trataInterrupcao(void){
    if(PIR1bits.TMR1IF == 1){
        PIR1bits.TMR1IF = 0;
        //TMR1L = 155; // AJUSTE PARA OCORRER INTERRUP플O A CADA 1ms
        //TMR1H = 245; // AJUSTE PARA OCORRER INTERRUP플O A CADA 1ms
        operacoesEmTick();
    }
    
    
    if(INTCONbits.INTF == 1){
        INTCONbits.INTF = 0;
        INTCONbits.GIE = 0;
        trataEntradaPulsada();
        INTCONbits.GIE = 1;
    }
}

//void interrupt high_priority intAltaPrioridade(void){
//    
//    
//}


/*
void interrupt low_priority intBaixaPrioridade(void){
    if(PIR1bits.TMR1IF == 1){
        PIR1bits.TMR1IF = 0;
        TMR1L = 155; // AJUSTE PARA OCORRER INTERRUP플O A CADA 1ms
        TMR1H = 245; // AJUSTE PARA OCORRER INTERRUP플O A CADA 1ms
        operacoesEmTick();
    }
}
*/  



//******************************************************************************
//*********************** programa principal ***********************************
//******************************************************************************
void main(void){ // INICIO DO PROGRAMA PRINCIPAL
    
    iniHardware();
    iniConversorAD;
    iniTimer1();
    //USART_init(9600, 8);
    iniUart();
    liga_PWM2(100);
    //MODULO_BOOST = LIGA_BOOST;
    setaSaidaEm9Volts();
    SAIDA_PULSADA = BAIXO;
    inicializaDisplay();
    INT0_init();
    exibeNumeroNenhum();
    delay_ms(500);
    ligaModuloBoost();
    
    EEPROM_WRITE(END_MEM_FLAGBAT, FLAGBAT_VERDE);
    flagNivelDeBateria = EEPROM_READ(END_MEM_FLAGBAT);
    if(flagNivelDeBateria == FLAGBAT_VERDE){
        ligaLEDVerdeBateria();
    }else{
        ligaLEDVermelhoBateria();
    }
    
    if(checaBotaoAjustes() == 1){
        exibeNumeroP();
        delay_ms(1000);
        entraEmModoAjuste();
        giraDisplay();
        exibeNumeroNenhum();
        delay_ms(1000);
    }
    
    //obtemTesteDoGoundBolt();
    //obtemNivelDeBateria();
    //obtemQuantidadeDeSensores();
    //exibeQuantidadeDeSensores(numeroDeSensores);
    //ligaModuloBoost();

//******************************************************************************    
//************************** loop principal ************************************
//******************************************************************************    
    while(1){ // INICIO DO LOOP PRINCIPAL
          
        executaTesteDosSensores();
        obtemTesteDoGoundBolt();
        obtemNivelDeBateria();
        #ifdef DEBUG
            unsigned char mensagem[50];
            sprintf(mensagem, "\r\n");
            enviaStringPelaUart(mensagem, strlen(mensagem));
        #endif  




    }// FIM DO LOOP PRINCIPAL

}// FIM DO PROGRAMA PRINCIPAL



void executaTesteDosSensores(void){
    numeroDeSensores = 0;    
    flagErroDeLeitura = 0;
    
    testaSaidaPulsadaEm9Volts();
    testaSaidaPulsadaEm20Volts();
    obtemQuantidadeDeSensores();
    exibeQuantidadeDeSensores(numeroDeSensores);
        
    atualizaPainelDeLed();
}

void atualizaPainelDeLed(void){
    if(flagErroDeLeitura == 0){
        LED_TESTE_OK = 1;
        LED_TESTE_NOK = 0;
    }else{
        LED_TESTE_OK = 0;
        LED_TESTE_NOK = 1;
    }
}


void ligaModuloBoost(void){
    //MODULO_BOOST = 0;
}

void desligaModuloBoost(void){
    //MODULO_BOOST = 1;
}

void setaSaidaEm9Volts(void){
    SEL_20V = 0;
    SEL_9V = 1;
}

void setaSaidaEm20Volts(void){
    SEL_9V = 0;
    SEL_20V = 1;
}

char lerBotao1(void){
    unsigned int deboucing;

    deboucing = 0;
    while(BT_START_SCAN == 0){
        deboucing = deboucing + 1;
        if(deboucing > 1000) return 1;
    }
    return 0;
}


void testaSaidaPulsadaEm9Volts(void){
    
    SAIDA_PULSADA = BAIXO;
    setaSaidaEm9Volts();
    tensaoDeSaida = OUT9VOLTS;
    delay_ms(10);
    pulsosSemResposta = 0;
    falhaPulsoDeEntrada = 0;
    respostaOk = 0;
    tempoDeReacaoDoSensor = 0;
    flagTempoDeReacaoDoSensor = 0;
    numeroDePulsos = 0;
    for(numeroDePulsos = 0; numeroDePulsos < (LIMITE_DE_PULSO); numeroDePulsos++){
        pulsosSemResposta = 1;
        OPTION_REGbits.INTEDG = 1; //Interrupt on rising edge of RB0/INT pin
        estadoDoSinal = ALTO;
        SAIDA_PULSADA = ALTO;
        for(tempoDeReacaoDoSensor = 0; tempoDeReacaoDoSensor < 110; tempoDeReacaoDoSensor++){ //90
            __delay_us(1);
        }
        pulsosSemResposta = 1;
        OPTION_REGbits.INTEDG = 0; //Interrupt on falling edge of RB0/INT pin
        estadoDoSinal = BAIXO;
        SAIDA_PULSADA = BAIXO;
        for(tempoDeReacaoDoSensor = 0; tempoDeReacaoDoSensor < 26500; tempoDeReacaoDoSensor++){
            __delay_us(1);
        }
        //obtemQuantidadeDeSensores();
        //exibeQuantidadeDeSensores(numeroDeSensores);
        //LED_FALHA_9V = !LED_FALHA_9V;
    }
    estadoDoSinal = BAIXO;
    SAIDA_PULSADA = BAIXO;
    if(pulsosSemResposta > 0 || flagTempoDeReacaoDoSensor == 1 || flagErrroDeAmplitude == 1){
        LED_FALHA_9V = 1;
        LED_TESTE_NOK = 1;
        LED_TESTE_OK = 0;
        flagErroDeLeitura = 1;
    }else{
        LED_FALHA_9V = 0;
        //LED_TESTE_NOK = 0;
        //LED_TESTE_OK = 1;
        //flagErroDeLeitura = 0;
    }
}

void testaSaidaPulsadaEm20Volts(void){
    
    SAIDA_PULSADA = BAIXO;
    setaSaidaEm20Volts();
    tensaoDeSaida = OUT20VOLTS;
    delay_ms(10);
    pulsosSemResposta = 0;
    falhaPulsoDeEntrada = 0;
    respostaOk = 0;
    tempoDeReacaoDoSensor = 0;
    flagTempoDeReacaoDoSensor = 0;
    numeroDePulsos = 0;
    for(numeroDePulsos = 0; numeroDePulsos < LIMITE_DE_PULSO + 50; numeroDePulsos++){
        pulsosSemResposta = 1;
        OPTION_REGbits.INTEDG = 1; //Interrupt on rising edge of RB0/INT pin
        estadoDoSinal = ALTO;
        SAIDA_PULSADA = ALTO;
        for(tempoDeReacaoDoSensor = 0; tempoDeReacaoDoSensor < 260; tempoDeReacaoDoSensor++){ //250
            __delay_us(1);
        }
        pulsosSemResposta = 1;
        OPTION_REGbits.INTEDG = 0; //Interrupt on falling edge of RB0/INT pin
        estadoDoSinal = BAIXO;
        SAIDA_PULSADA = BAIXO;
        for(tempoDeReacaoDoSensor = 0; tempoDeReacaoDoSensor < 3300; tempoDeReacaoDoSensor++){
            __delay_us(1);
        }
        //obtemQuantidadeDeSensores();
        //exibeQuantidadeDeSensores(numeroDeSensores);
        //LED_FALHA_20V = !LED_FALHA_20V;
    }
    estadoDoSinal = BAIXO;
    SAIDA_PULSADA = BAIXO;
    if(pulsosSemResposta > 0 || flagTempoDeReacaoDoSensor == 1 || flagErrroDeAmplitude == 1){
        LED_FALHA_20V = 1;
        LED_TESTE_NOK = 1;
        LED_TESTE_OK = 0;
        flagErroDeLeitura = 1;
    }else{
        LED_FALHA_20V = 0;
        //LED_TESTE_NOK = 0;
        //LED_TESTE_OK = 1;
        //flagErroDeLeitura = 0;
    }
    
}


void obtemQuantidadeDeSensores(void){
    unsigned int resultadoObtidoDoSensores;
    unsigned char mensagem[50];
    unsigned int numeroDePulsos;
    
//    setaSaidaEm9Volts();
//    for(numeroDePulsos = 0; numeroDePulsos < 10; numeroDePulsos = numeroDePulsos + 1){
//        SAIDA_PULSADA = ALTO;
//        delay_ms(1);
//        SAIDA_PULSADA = BAIXO;
//        delay_ms(26);
//    }
    
    resultadoObtidoDoSensores = 0;
    resultadoObtidoDoSensores = obtemConvercaoAD(CANAL_AD_SENSORES);

#ifdef DEBUG
    sprintf(mensagem, "Quantidade de Sensores = %05d\r\n", resultadoObtidoDoSensores);
    enviaStringPelaUart(mensagem, strlen(mensagem));
#endif    
    if(resultadoObtidoDoSensores >= 1023 - 13){
        numeroDeSensores = 0;
        //exibeNumero0();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 960 - 13) && (resultadoObtidoDoSensores <= 960 + 13)){
        numeroDeSensores = 1;
        //exibeNumero1();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 896 - 13) && (resultadoObtidoDoSensores <= 896 + 13)){
        numeroDeSensores = 2;
        //exibeNumero2();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 783 - 13) && (resultadoObtidoDoSensores <= 783 + 13)){
        numeroDeSensores = 3;
        //exibeNumero3();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 768 - 13) && (resultadoObtidoDoSensores <= 768 + 13)){
        numeroDeSensores = 4;
        //exibeNumero4();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 704 - 13) && (resultadoObtidoDoSensores <= 704 + 13)){
        numeroDeSensores = 5;
        //exibeNumero5();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 639 - 13) && (resultadoObtidoDoSensores <= 639 + 13)){ //if((resultadoObtidoDoSensores >= 655 - 13) && (resultadoObtidoDoSensores <= 655 + 13)){
        numeroDeSensores = 6;
        //exibeNumero6();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 608 - 13) && (resultadoObtidoDoSensores <= 608 + 13)){ //if((resultadoObtidoDoSensores >= 632 - 13) && (resultadoObtidoDoSensores <= 632 + 13)){
        numeroDeSensores = 7;
        //exibeNumero7();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 575 - 13) && (resultadoObtidoDoSensores <= 575 + 13)){
        numeroDeSensores = 8;
        //exibeNumero8();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 543 - 13) && (resultadoObtidoDoSensores <= 543 + 13)){
        numeroDeSensores = 9;
        //exibeNumero9();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 512 - 13) && (resultadoObtidoDoSensores <= 512 + 13)){
        numeroDeSensores = 10;
        //exibeNumeroA();
        return;
    }

    numeroDeSensores = -1;
    //exibeNumeroF();
}

/*
void obtemQuantidadeDeSensoresEm9Volts(void){
    unsigned int resultadoObtidoDoSensores;
    unsigned char mensagem[50];   

    resultadoObtidoDoSensores = obtemConvercaoAD(CANAL_AD_SENSORES);
#ifdef DEBUG
    sprintf(mensagem, "Quantidade de Sensores = %05d\r\n", resultadoObtidoDoSensores);
    enviaStringPelaUart(mensagem, strlen(mensagem));
#endif    
    if(resultadoObtidoDoSensores >= 1023 - 13){
        numeroDeSensores = 0;
        //exibeNumero0();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 960 - 13) && (resultadoObtidoDoSensores <= 960 + 13)){
        numeroDeSensores = 1;
        //exibeNumero1();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 896 - 13) && (resultadoObtidoDoSensores <= 896 + 13)){
        numeroDeSensores = 2;
        //exibeNumero2();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 783 - 13) && (resultadoObtidoDoSensores <= 783 + 13)){
        numeroDeSensores = 3;
        //exibeNumero3();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 768 - 13) && (resultadoObtidoDoSensores <= 768 + 13)){
        numeroDeSensores = 4;
        //exibeNumero4();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 704 - 13) && (resultadoObtidoDoSensores <= 704 + 13)){
        numeroDeSensores = 5;
        //exibeNumero5();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 639 - 13) && (resultadoObtidoDoSensores <= 639 + 13)){
        numeroDeSensores = 6;
        //exibeNumero6();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 608 - 13) && (resultadoObtidoDoSensores <= 608 + 13)){
        numeroDeSensores = 7;
        //exibeNumero7();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 574 - 13) && (resultadoObtidoDoSensores <= 574 + 13)){
        numeroDeSensores = 8;
        //exibeNumero8();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 540 - 13) && (resultadoObtidoDoSensores <= 540 + 13)){
        numeroDeSensores = 9;
        //exibeNumero9();
        return;
    }
    
    if((resultadoObtidoDoSensores >= 512 - 13) && (resultadoObtidoDoSensores <= 512 + 13)){
        numeroDeSensores = 10;
        //exibeNumeroA();
        return;
    }

    numeroDeSensores = -1;
    //exibeNumeroF();
}
*/


void obtemNivelDeBateria(void){
    static unsigned int tempoDeAmostragem = 0;
    unsigned int nivelDaBateria;
    unsigned char mensagem[50];
    
    nivelDaBateria = obtemConvercaoAD(CANAL_AD_BATERIA);
#ifdef DEBUG    
    sprintf(mensagem, "Nivel de Bateria =       %u\r\n", nivelDaBateria);
    enviaStringPelaUart(mensagem, strlen(mensagem));
#endif 
    
    
    if(flagNivelDeBateria == FLAGBAT_VERDE){
        if(nivelDaBateria < 700){
            ligaLEDVermelhoBateria();
            EEPROM_WRITE(END_MEM_FLAGBAT, FLAGBAT_VERMELHO);
            flagNivelDeBateria = FLAGBAT_VERMELHO;
        }
    }
    
    if(flagNivelDeBateria == FLAGBAT_VERMELHO){
        if(nivelDaBateria > 827){ //830
            ligaLEDVerdeBateria();
            EEPROM_WRITE(END_MEM_FLAGBAT, FLAGBAT_VERDE);
            flagNivelDeBateria = FLAGBAT_VERDE;
        }
    }
    
}

void ligaLEDVermelhoBateria(void){
    LED_NIVEL_BATERIA = 0;
}

void ligaLEDVerdeBateria(void){
    LED_NIVEL_BATERIA = 1;
}


void obtemTesteDoGoundBolt(void){
    unsigned int resultadoDoTesteDoGB;
    unsigned char mensagem[50];
    
    resultadoDoTesteDoGB = obtemConvercaoAD(CANAL_AD_GROUND_BOLT);
#ifdef DEBUG
    sprintf(mensagem, "Teste do Ground Bolt =   %u\r\n", resultadoDoTesteDoGB);
    enviaStringPelaUart(mensagem, strlen(mensagem));
#endif    
    if((resultadoDoTesteDoGB < 327 + 20) && (resultadoDoTesteDoGB > 327 - 20)){
        LED_GB_OK = 1;
        LED_GB_NOK = 0;
        LED_GB_CURTO = 0;
        return;
    }
    
    if((resultadoDoTesteDoGB < 224 + 20) && (resultadoDoTesteDoGB > 224 - 20)){
        LED_GB_NOK = 1;
        LED_GB_OK = 0;
        LED_GB_CURTO = 0;
        return;
    }
    
    if((resultadoDoTesteDoGB < 384 + 20) && (resultadoDoTesteDoGB > 384 - 20)){
        LED_GB_CURTO = 1;
        LED_GB_NOK = 0;
        LED_GB_OK = 0;
        return;
    }
    
    LED_GB_CURTO = 0;
    LED_GB_NOK = 1;
    LED_GB_OK = 0;
    
}


void exibeQuantidadeDeSensores(char quantidade){
    char mensagem[50];
   
    //sprintf(mensagem, "SENSORES = %d\r\n", quantidade);
    //enviaStringPelaUart(mensagem, strlen(mensagem));
    switch(quantidade){
        case 0:
            exibeNumero0();
            break;
        case 1:
            exibeNumero1();
            break;
        case 2:
            exibeNumero2();
            break;
        case 3:
            exibeNumero3();
            break;
        case 4:
            exibeNumero4();
            break; 
        case 5:
            exibeNumero5();
            break;
        case 6:
            exibeNumero6();
            break;
        case 7:
            exibeNumero7();
            break;
        case 8:
            exibeNumero8();
            break;
        case 9:
            exibeNumero9();
            break;
        case 10:
            exibeNumeroA();
            break;    
        default:
            //exibeNumeroF();
            break;
    }
}


unsigned int ObtemAmplitureDoSinal(void){
    //static unsigned int tempoDeAmostragem = 0;
    unsigned int amplitudeDoSinal;
    unsigned char mensagem[50];
    
    amplitudeDoSinal = 0;
    amplitudeDoSinal = obtemConvercaoAD(CANAL_AD_AMPLITUDE);
#ifdef DEBUG    
    sprintf(mensagem, "Ampliture do Sinal =       %u\r\n", amplitudeDoSinal);
    enviaStringPelaUart(mensagem, strlen(mensagem));
#endif 
    return amplitudeDoSinal;
}


void exibeNumero0(void){
    DISPLAY_A = 0;
    DISPLAY_B = 0;
    DISPLAY_C = 0;
    DISPLAY_D = 0;
    DISPLAY_E = 0;
    DISPLAY_F = 0;
    DISPLAY_G = 1;
}

void exibeNumero1(void){
    DISPLAY_A = 1;
    DISPLAY_B = 0;
    DISPLAY_C = 0;
    DISPLAY_D = 1;
    DISPLAY_E = 1;
    DISPLAY_F = 1;
    DISPLAY_G = 1;
}

void exibeNumero2(void){
    DISPLAY_A = 0;
    DISPLAY_B = 0;
    DISPLAY_C = 1;
    DISPLAY_D = 0;
    DISPLAY_E = 0;
    DISPLAY_F = 1;
    DISPLAY_G = 0;
}

void exibeNumero3(void){
    DISPLAY_A = 0;
    DISPLAY_B = 0;
    DISPLAY_C = 0;
    DISPLAY_D = 0;
    DISPLAY_E = 1;
    DISPLAY_F = 1;
    DISPLAY_G = 0;
}

void exibeNumero4(void){
    DISPLAY_A = 1;
    DISPLAY_B = 0;
    DISPLAY_C = 0;
    DISPLAY_D = 1;
    DISPLAY_E = 1;
    DISPLAY_F = 0;
    DISPLAY_G = 0;
}

void exibeNumero5(void){
    DISPLAY_A = 0;
    DISPLAY_B = 1;
    DISPLAY_C = 0;
    DISPLAY_D = 0;
    DISPLAY_E = 1;
    DISPLAY_F = 0;
    DISPLAY_G = 0;
}

void exibeNumero6(void){
    DISPLAY_A = 0;
    DISPLAY_B = 1;
    DISPLAY_C = 0;
    DISPLAY_D = 0;
    DISPLAY_E = 0;
    DISPLAY_F = 0;
    DISPLAY_G = 0;
}

void exibeNumero7(void){
    DISPLAY_A = 0;
    DISPLAY_B = 0;
    DISPLAY_C = 0;
    DISPLAY_D = 1;
    DISPLAY_E = 1;
    DISPLAY_F = 1;
    DISPLAY_G = 1;
}

void exibeNumero8(void){
    DISPLAY_A = 0;
    DISPLAY_B = 0;
    DISPLAY_C = 0;
    DISPLAY_D = 0;
    DISPLAY_E = 0;
    DISPLAY_F = 0;
    DISPLAY_G = 0;
}

void exibeNumero9(void){
    DISPLAY_A = 0;
    DISPLAY_B = 0;
    DISPLAY_C = 0;
    DISPLAY_D = 0;
    DISPLAY_E = 1;
    DISPLAY_F = 0;
    DISPLAY_G = 0;
}

void exibeNumeroA(void){
    DISPLAY_A = 0;
    DISPLAY_B = 0;
    DISPLAY_C = 0;
    DISPLAY_D = 1;
    DISPLAY_E = 0;
    DISPLAY_F = 0;
    DISPLAY_G = 0;
}

void exibeNumeroB(void){
    DISPLAY_A = 1;
    DISPLAY_B = 1;
    DISPLAY_C = 0;
    DISPLAY_D = 0;
    DISPLAY_E = 0;
    DISPLAY_F = 0;
    DISPLAY_G = 0;
}

void exibeNumeroC(void){
    DISPLAY_A = 0;
    DISPLAY_B = 1;
    DISPLAY_C = 1;
    DISPLAY_D = 0;
    DISPLAY_E = 0;
    DISPLAY_F = 0;
    DISPLAY_G = 1;
}

void exibeNumeroD(void){
    DISPLAY_A = 1;
    DISPLAY_B = 0;
    DISPLAY_C = 0;
    DISPLAY_D = 0;
    DISPLAY_E = 0;
    DISPLAY_F = 1;
    DISPLAY_G = 0;
}

void exibeNumeroE(void){
    DISPLAY_A = 0;
    DISPLAY_B = 1;
    DISPLAY_C = 1;
    DISPLAY_D = 0;
    DISPLAY_E = 0;
    DISPLAY_F = 0;
    DISPLAY_G = 0;
}

void exibeNumeroF(void){
    DISPLAY_A = 0;
    DISPLAY_B = 1;
    DISPLAY_C = 1;
    DISPLAY_D = 1;
    DISPLAY_E = 0;
    DISPLAY_F = 0;
    DISPLAY_G = 0;
}

void exibeNumeroNenhum(void){
    DISPLAY_A = 1;
    DISPLAY_B = 1;
    DISPLAY_C = 1;
    DISPLAY_D = 1;
    DISPLAY_E = 1;
    DISPLAY_F = 1;
    DISPLAY_G = 1;
    DISPLAY_P = 1;
}

void exibeNumeroP(void){
    DISPLAY_A = 0;
    DISPLAY_B = 0;
    DISPLAY_C = 1;
    DISPLAY_D = 1;
    DISPLAY_E = 0;
    DISPLAY_F = 0;
    DISPLAY_G = 0;
    DISPLAY_P = 0;
}

void giraDisplay(void){
    DISPLAY_A = 0;
    delay_ms(100);
    DISPLAY_A = 1;
    delay_ms(50);
    DISPLAY_B = 0;
    delay_ms(100);
    DISPLAY_B = 1;
    delay_ms(50);
    DISPLAY_C = 0;
    delay_ms(100);
    DISPLAY_C = 1;
    delay_ms(50);
    DISPLAY_D = 0;
    delay_ms(100);
    DISPLAY_D = 1;
    delay_ms(50);
    DISPLAY_E = 0;
    delay_ms(100);
    DISPLAY_E = 1;
    delay_ms(50);
    DISPLAY_F = 0;
    delay_ms(100);
    DISPLAY_F = 1;
    delay_ms(50);
    DISPLAY_P = 0;
    delay_ms(100);
    DISPLAY_P = 1;
    delay_ms(100);
}


void inicializaDisplay(void){
    
    exibeNumero8();
    ligaPainelLed();
    delay_ms(1000);
    exibeNumeroNenhum();
    desligaPainelLed();
    
}

void ligaPainelLed(void){
    LED_TESTE_OK = 1;
    LED_TESTE_NOK = 1;
    LED_FALHA_20V = 1;
    LED_FALHA_9V = 1;
    LED_GB_OK = 1;
    LED_GB_NOK = 1;
    LED_GB_CURTO = 1;
}

void desligaPainelLed(void){
    LED_TESTE_OK = 0;
    LED_TESTE_NOK = 0;
    LED_FALHA_20V = 0;
    LED_FALHA_9V = 0;
    LED_GB_OK = 0;
    LED_GB_NOK = 0;
    LED_GB_CURTO = 0;
}

void trataEntradaPulsada(void){
    unsigned int amplitude;
    char mensagem[50];
    
    pulsosSemResposta = 0;
        
    if(estadoDoSinal == ALTO){
        if(tempoDeReacaoDoSensor < 1 || tempoDeReacaoDoSensor > 300){ //if(tempoDeReacaoDoSensor < 5 || tempoDeReacaoDoSensor > 250){
            flagTempoDeReacaoDoSensor = 1;
        }
        amplitude = 400;
        //amplitude = ObtemAmplitureDoSinal();
        //sprintf(mensagem, "Amplitude =   %u\r\n", amplitude);
        //enviaStringPelaUart(mensagem, strlen(mensagem));
        if(tensaoDeSaida == OUT9VOLTS){
            if(amplitude < 150){ // 170
                flagErrroDeAmplitude = 1;
            }else{
                flagErrroDeAmplitude = 0;
            }
        }
        
        if(tensaoDeSaida == OUT20VOLTS){
            if(amplitude < 350){
                flagErrroDeAmplitude = 1;
            }else{
                flagErrroDeAmplitude = 0;
            }
        }
        
    }

    if(estadoDoSinal == BAIXO){
        if(tempoDeReacaoDoSensor > 600){
            flagTempoDeReacaoDoSensor = 1;
        }
    }
}



//OPERA합ES EM TICK EM 1ms
void operacoesEmTick(void){
    static int contadorDe100ms = 0;
    static int contadorDe1s = 0;
    
    //OPERA합ES EM TICK EM 100ms
    contadorDe100ms = contadorDe100ms + 1;
    if(contadorDe100ms > 40){
        contadorDe100ms = 0;  
        DISPLAY_P = !DISPLAY_P;
    }
    
    //OPERA합ES EM TICK EM 1s
    contadorDe1s = contadorDe1s + 1;
    if(contadorDe1s > 500){
        contadorDe1s = 0;
    }     
}


char checaBotaoAjustes(void){
    unsigned int contador;
    
    contador = 0;
    while(BT_START_SCAN == 0){
        contador = contador + 1;
        if(contador > 5000){
            return 1;
        }
    }
    return 0;
}

void entraEmModoAjuste(void){
  
    ligaModuloBoost();
    setaSaidaEm9Volts();
    LED_FALHA_9V = 1;
    while(checaBotaoAjustes() == 0){
        modoTeste9();
        //obtemTesteDoGoundBolt();
        //obtemNivelDeBateria();
    }
    
    setaSaidaEm20Volts();
    LED_FALHA_9V = 0;
    LED_FALHA_20V = 1;
    delay_ms(1000);
    while(checaBotaoAjustes() == 0){
        modoTeste20();
        //obtemTesteDoGoundBolt();
        //obtemNivelDeBateria();
    }
    
    LED_FALHA_9V = 0;
    LED_FALHA_20V = 0;
    desligaModuloBoost();
}

void modoTeste9(void){
    
    SAIDA_PULSADA = BAIXO;
    setaSaidaEm9Volts();
    delay_ms(100);
    pulsosSemResposta = 0;
    falhaPulsoDeEntrada = 0;
    respostaOk = 0;
    tempoDeReacaoDoSensor = 0;
    flagTempoDeReacaoDoSensor = 0;
    numeroDePulsos = 0;
    for(numeroDePulsos = 0; numeroDePulsos < (LIMITE_DE_PULSO); numeroDePulsos++){
        pulsosSemResposta = 1;
        OPTION_REGbits.INTEDG = 1; //Interrupt on rising edge of RB0/INT pin
        estadoDoSinal = ALTO;
        SAIDA_PULSADA = ALTO;
        for(tempoDeReacaoDoSensor = 0; tempoDeReacaoDoSensor < 250; tempoDeReacaoDoSensor++){
            __delay_us(1);
        }
        pulsosSemResposta = 1;
        OPTION_REGbits.INTEDG = 0; //Interrupt on falling edge of RB0/INT pin
        estadoDoSinal = BAIXO;
        SAIDA_PULSADA = BAIXO;
        for(tempoDeReacaoDoSensor = 0; tempoDeReacaoDoSensor < 7000; tempoDeReacaoDoSensor++){
            __delay_us(1);
        }
        obtemQuantidadeDeSensores();
        exibeQuantidadeDeSensores(numeroDeSensores);
        LED_FALHA_9V = !LED_FALHA_9V;
    }
    if(pulsosSemResposta > 0 || flagTempoDeReacaoDoSensor == 1){
        LED_FALHA_9V = 1;
        LED_TESTE_NOK = 1;
        LED_TESTE_OK = 0;
        flagErroDeLeitura = 1;
    }else{
        LED_FALHA_9V = 0;
        LED_TESTE_NOK = 0;
        LED_TESTE_OK = 1;
    }

}

void modoTeste20(void){
    
    SAIDA_PULSADA = BAIXO;
    setaSaidaEm20Volts();
    delay_ms(100);
    pulsosSemResposta = 0;
    falhaPulsoDeEntrada = 0;
    respostaOk = 0;
    tempoDeReacaoDoSensor = 0;
    flagTempoDeReacaoDoSensor = 0;
    numeroDePulsos = 0;
    for(numeroDePulsos = 0; numeroDePulsos < LIMITE_DE_PULSO + 100; numeroDePulsos++){
        pulsosSemResposta = 1;
        OPTION_REGbits.INTEDG = 1; //Interrupt on rising edge of RB0/INT pin
        estadoDoSinal = ALTO;
        SAIDA_PULSADA = ALTO;
        for(tempoDeReacaoDoSensor = 0; tempoDeReacaoDoSensor < 250; tempoDeReacaoDoSensor++){
            __delay_us(1);
        }
        pulsosSemResposta = 1;
        OPTION_REGbits.INTEDG = 0; //Interrupt on falling edge of RB0/INT pin
        estadoDoSinal = BAIXO;
        SAIDA_PULSADA = BAIXO;
        for(tempoDeReacaoDoSensor = 0; tempoDeReacaoDoSensor < 3000; tempoDeReacaoDoSensor++){
            __delay_us(1);
        }
        obtemQuantidadeDeSensores();
        exibeQuantidadeDeSensores(numeroDeSensores);
        LED_FALHA_20V = !LED_FALHA_20V;
    }
    if(pulsosSemResposta > 0 || flagTempoDeReacaoDoSensor == 1){
        LED_FALHA_20V = 1;
        LED_TESTE_NOK = 1;
        LED_TESTE_OK = 0;
        flagErroDeLeitura = 1;
    }else{
        LED_FALHA_20V = 0;
        LED_TESTE_NOK = 0;
        LED_TESTE_OK = 1;
    }
}