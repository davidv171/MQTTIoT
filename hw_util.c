/**************************************************************************

 Predloga projekta za razvojno okolje Keil uVision za izvajanje
 laboratorijskih vaj pri predmetu Mobilni in vseprisotni elektronski sistemi
 
 Datoteka: hw_util.c
 
 Knjiznica za delo z strojno opremo 
 
 Ploscica "ESP extension" se mora povezati na prikljucek EXT1
 Ploscica "IETK - ATM XPL - IO1" se mora povezati na prkljucek EXT3
  
 Avtor: Gregor Donaj
 Datum: 26.2.2018
  
 Razlicica 1.0
 
**************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <math.h>

#include "sam4s.h"
#include "cmsis_os.h"
#include "hw_util.h"

void  IO1_init(void)
{
	PMC->PMC_WPMR = 0x504D4300;
	PMC->PMC_PCER0 = (1UL << ID_PIOA)|(1UL << ID_PIOB)|(1UL << ID_PIOC)|(1UL << ID_ADC);
	
	// izhodi na A so: 10,12,13,24 (vse 4 LED
	PIOA->PIO_PER  =
	PIOA->PIO_OER  =
	PIOA->PIO_PUDR =
	PIOA->PIO_OWER = (PIO_PA10)|(PIO_PA12)|(PIO_PA13)|(PIO_PA14);
	
	// izhodi na B so: UART (druga knjiznica ga inicializira)
	PIOB->PIO_PER  =
	PIOB->PIO_OER  =
	PIOB->PIO_PUDR =
	PIOB->PIO_OWER = (PIO_PB4)|(PIO_PB5);
	
	// izhodi na C so: PC20 filter
	PIOC->PIO_PER  =
	PIOC->PIO_OER  =
	PIOC->PIO_PUDR =
	PIOC->PIO_OWER = (PIO_PC20);
	
	// vhodi na A so: PA0 (SW2)
	PIOA->PIO_PUER =
	PIOA->PIO_IDR  =
	PIOA->PIO_ODR  =
	PIOA->PIO_PER  = (PIO_PA0);
	
	// vhodi na B so: PB2 uart (druga knjiznica ga inicializira)
	
	// vhodi na C so ostale tri tipke
	PIOC->PIO_PUER =
	PIOC->PIO_IDR  =
	PIOC->PIO_ODR  =
	PIOC->PIO_PER  = (PIO_PC21)|(PIO_PC22)|(PIO_PC31);
	
	// interput na PA0 -- SW2
	PIOA->PIO_IER = 
	PIOA->PIO_ESR = 
 	PIOA->PIO_FELLSR = 1UL<<0; // pin0
		
	PMC->PMC_WPMR = 0x504D4301;
	
	// takoj izklopimo vse
	IO1_LED_off(1);
	IO1_LED_off(2);
	IO1_LED_off(3);
	IO1_LED_off(4);
	
	// inicializacija ADC
	ADC->ADC_CR = 1UL << 0;               // reset ADC
	ADC->ADC_MR |= 1UL << 7;              // free run
	ADC->ADC_MR |= 24UL << 8;             // adc clk = 1 MHz
	ADC->ADC_MR |= ADC_MR_STARTUP_SUT960; // startup 960
	ADC->ADC_MR |= ADC_MR_SETTLING_AST17;  // setling 17
	ADC->ADC_MR |= ADC_MR_TRANSFER(2);
	
	ADC->ADC_CHER = ADC_CHER_CH13;        // omogoci adc kanal 13
	ADC->ADC_CHER = ADC_CHER_CH14;        // omogoci adc kanal 14
}

void  IO1_LED_on(int no)
{
	switch (no)
	{
		case 1 : PIOA->PIO_SODR = PIO_PA14;   break;
		case 2 : PIOA->PIO_SODR = PIO_PA12;   break;
		case 3 : PIOA->PIO_SODR = PIO_PA13;   break;
		case 4 : PIOA->PIO_SODR = PIO_PA10;   break;
	}
}

void  IO1_LED_off(int no)
{
	switch (no)
	{
		case 1 : PIOA->PIO_CODR = PIO_PA14;   break;
		case 2 : PIOA->PIO_CODR = PIO_PA12;   break;
		case 3 : PIOA->PIO_CODR = PIO_PA13;   break;
		case 4 : PIOA->PIO_CODR = PIO_PA10;   break;
	}
}
	
float IO1_read_TEMP(void)
{
	int adc_value;
	float value;
	adc_value = ADC->ADC_CDR[14] & 0x00000FFF;
	value = adc_value * 0.0413 - 20.5;
	return value;
}

float IO1_read_ANALOG(void)
{
	int adc_value;
	float value;
	adc_value = ADC->ADC_CDR[13] & 0x00000FFF;
	value = adc_value * 0.00080566;
	return value;
}

void  IO1_LED_set(int no, int value)
{
	if (value==0)
		IO1_LED_off(no);
	else
		IO1_LED_on(no);
}

int   IO1_SW(int no)
{
	int value = 0;
	switch(no)
	{
		case 1 : value = ~PIOC->PIO_PDSR & SW1_PIN; break;
		case 2 : value = ~PIOA->PIO_PDSR & SW2_PIN; break;
		case 3 : value = ~PIOC->PIO_PDSR & SW3_PIN; break;
		case 4 : value = ~PIOC->PIO_PDSR & SW4_PIN; break;
	}		
	if (value)
		return 1;
	else
		return 0;
}

/**************************************************************************
CDC and ESP uart communication
**************************************************************************/

void cdc_uart_init(void)
{
	PMC->PMC_WPMR = 0x504D4300;             // disable write protection

	PMC->PMC_PCER0 = (1UL << ID_UART1);     // enable power to uart1
	PIOB->PIO_OER = PIO_PB3A_UTXD1;         // output enable
	PIOB->PIO_PDR = PIO_PB2A_URXD1;         // disable PIOC
	PIOB->PIO_PDR = PIO_PB3A_UTXD1;         // disable PIOC
	PIOB->PIO_ABCDSR[0] = 0x00000000;       // set pin to uart peripheral
	PIOB->PIO_ABCDSR[0] = 0x00000000;       // set pin to uart peripheral
	
	PMC->PMC_PCER0 = (1UL << ID_USART1);    // enable power to uart0
	PIOA->PIO_ODR = PIO_PA21A_RXD1;         // output enabl
	PIOA->PIO_OER = PIO_PA22A_TXD1;         // output enabl
	PIOA->PIO_PDR = PIO_PA21A_RXD1;         // disable PIOC
	PIOA->PIO_PDR = PIO_PA22A_TXD1;         // disable PIOC
	PIOA->PIO_ABCDSR[0] = 0x00000000;       // set pin to uart peripheral
	PIOA->PIO_ABCDSR[1] = 0x00000000;       // set pin to uart peripheral
	
	PMC->PMC_WPMR = 0x504D4301;             // enable write protection
	
	
	UART1->UART_CR = UART_CR_TXEN;          // enable transmitter
	UART1->UART_CR = UART_CR_RXEN;          // enable reciever
	UART1->UART_BRGR = 0x00000271;          // br=9600; br = 96 MHz / (16 * UART_BRGR)
	UART1->UART_MR = 0x00000800;            // normal mode, no parity
	
	USART1->US_WPMR = 0x55534100;
	// USART1->US_CR = US_CR_RSTTX;
	// USART1->US_CR = US_CR_RSRTX;
	USART1->US_CR = US_CR_TXEN;             // enable transmitter
	USART1->US_CR = US_CR_RXEN;             // enable reciever
	USART1->US_MR = 0x001008C0; 
	USART1->US_BRGR = 0x00000271;           // br=9600; br = 96 MHz / (16 * UART_BRGR)
	USART1->US_WPMR = 0x55534101;
}

void cdc_uart_put_char(unsigned char c)
{
	while (!(UART1->UART_SR & UART_SR_TXEMPTY)){}
		UART1->UART_THR=c;
}

void cdc_uart_put_string(unsigned char *s)
{
	while (*s != 0) {
		cdc_uart_put_char(*s++);
	}
}

unsigned char cdc_uart_read_char(void)
{
	while(!(UART1->UART_SR & UART_SR_RXRDY)){}
		return UART1->UART_RHR;
}

unsigned char cdc_uart_read_char_nb(void)
{
	if (UART1->UART_SR & UART_SR_RXRDY)
		return UART1->UART_RHR;
	else
		return 0;
}

void esp_uart_put_char(unsigned char c)
{
	while (!(USART1->US_CSR & US_CSR_TXEMPTY)){}
		USART1->US_THR=c;
}

void esp_uart_put_string(unsigned char *s)
{
	while (*s != 0) {
		esp_uart_put_char(*s++);
	}
}

unsigned char esp_uart_read_char(void)
{
	while(!(USART1->US_CSR & US_CSR_RXRDY)){}
		return USART1->US_RHR;
}

unsigned char esp_uart_read_char_nb(void)
{
	if (USART1->US_CSR & US_CSR_RXRDY)
		return USART1->US_RHR;
	else
		return 0;
}

void cdc_uart_read_string(unsigned char *s)
{
	unsigned char buffer[50];
	int index;
	char a;
	int i;
	
	index = 0;
	do
	{
		a = cdc_uart_read_char();
		buffer[index++] = a;
		// if (a == 10) break;
	}	while (a != 10);
	
	for (i = 0; i < index; i++)
	{
		s[i] = buffer[i];
	}
}

void esp_uart_read_string(unsigned char *s)
{
	unsigned char buffer[50];
	int index;
	char a;
	int i;
	
	index = 0;
	do
	{
		a = esp_uart_read_char();
		buffer[index++] = a;
		// if (a == 10) break;
	}	while (a != 10);
	
	for (i = 0; i < index; i++)
	{
		s[i] = buffer[i];
	}
}

/**************************************************************************
Float to ASCII
**************************************************************************/

void reverse(unsigned char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}

int intToStr(int x, unsigned char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

void ftoa(float n, unsigned char *res, int afterpoint)
{
    int ipart = (int)n;
    float fpart = n - (float)ipart;
    int i = intToStr(ipart, res, 0);
    if (afterpoint != 0)
    {
        res[i] = '.';
        fpart = fpart * pow(10, afterpoint);
        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}
