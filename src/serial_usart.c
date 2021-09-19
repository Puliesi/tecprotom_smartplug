
#include <pic16f877A.h>
#include <pic16f877.h>
//#include <pic18f4520.h>
//#include <pic18.h>
#include "hardware.h"


/////////////////////////////////funçoes usadas pela uart //////////////////////////////////////////////////////
void USART_init(unsigned long baudrate, unsigned char numBits){
    ////por padrão é usado o modo 8 bits e sem paridade, mas se necessario ajuste aqui a configuração desejada.
    //verifique datasheet para ver a porcentagem de erro e se a velocidade é possivel para o cristal utilizado.
    
    RCSTAbits.SPEN = 1; //habilita porta serial
    switch (numBits){
        case 8: RCSTAbits.RX9 = 0; break;
        case 9: RCSTAbits.RX9 = 1; break;
    }
    RCSTAbits.SREN = 0; //recepção unitária desabilitada
    RCSTAbits.CREN = 1; //recepção contínua habilitada
    RCSTAbits.ADDEN;  //desabilita sistema de endereçamento
    
    switch (numBits){
        case 8: TXSTAbits.TX9 = 0; break;
        case 9: TXSTAbits.TX9 = 1; break;
    }
    TXSTAbits.TXEN = 1; //transmissão abilitada
    TXSTAbits.SYNC = 0; //modo assincrono
    TXSTAbits.BRGH = 1; //baudrate alto
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    TXSTAbits.BRGH = 1;
    unsigned int taxa;
    //SPBRG = 25; 
    //SPBRG = 103; 
    taxa = (int)((_XTAL_FREQ/(16 * baudrate))-1);
    SPBRG = taxa;
    RCIE = 1;//habilita interrupção de recepção
    TXIE = 0;//deixa interrupção de transmissão desligado(pois corre se o risco de ter uma interrupção escrita e leitura ao mesmo tempo)
}


int DadoRecebido(void){
    return PIR1bits.RCIF;
}


void EnviaDadoUSART(char dado){
    TXIF = 0; //limpa flag que sinaliza envio completo.
    TXREG = dado;
    while(TRMT == 0); //espera enviar caracter
}


void HabilitaUSART(void){
    RCSTAbits.SPEN = 1; //habilita porta serial
}


void DesabilitaUSART(void){
    RCSTAbits.SPEN = 0; //habilita porta serial
}


unsigned char RecebeDadoUSART(void)
{
    unsigned char dado;
    dado = RCREG;
    RCIF = 0; //limpa flag que sinaliza envio completo.
    return dado;
}

int OverflowUSART(void){
    if (RCSTAbits.OERR == 0){
        return 0;
    }   
    return 1;
} 


void LimpaBufferRxUSART(void){
    
    unsigned char lixo;

    lixo = RCREG;
    lixo = RCREG;
    lixo = RCREG;
    lixo = RCREG;
    PIR1bits.RCIF = 0;
}

int TxOcupado(void){
    return !TXSTAbits.TRMT;
}

void enviaStringPelaUart(unsigned char str[100], int tamanho){
    int vetor;
        
    for(vetor = 0; vetor < tamanho; vetor = vetor + 1){
        while(TXSTAbits.TRMT == 0);
        TXSTAbits.TRMT = 0;
        TXREG = str[vetor];
    }
    
}



