/**************************************************************************

 Predloga projekta za razvojno okolje Keil uVision za izvajanje
 laboratorijskih vaj pri predmetu Mobilni in vseprisotni elektronski sistemi
 
 Datoteka: main.c
 
 Osrednja programska datoteka
 
 Ploscica "ESP extension" se mora povezati na prikljucek EXT1
 Ploscica "IETK - ATM XPL - IO1" se mora povezati na prkljucek EXT3
  
 Avtor: Gregor Donaj
 Datum: 26.2.2018
  
 Razlicica 1.0
 
**************************************************************************/

#include "cmsis_os.h"
#include "sam4S.h"
#include "hw_util.h"
#include "string.h"
#include "stdio.h"
#include "MQTTPacket.h"
#include "MQTTPublish.h"
#include "MQTTConnect.h"
#include "MQTTFormat.h"
#include "MQTTSubscribe.h"
#include "MQTTUnsubscribe.h"


void passtrough()
{
	IO1_LED_on(4);
	
	PIOA->PIO_PER  =
	PIOA->PIO_OER  =
	PIOA->PIO_PUDR =
	PIOA->PIO_OWER = (PIO_PA22);
	
	PIOB->PIO_PER  =
	PIOB->PIO_OER  =
	PIOB->PIO_PUDR =
	PIOB->PIO_OWER = (PIO_PB3);
	
	PIOA->PIO_PUER =
	PIOA->PIO_IDR  =
	PIOA->PIO_ODR  =
	PIOA->PIO_PER  = (PIO_PA23);
	
	PIOB->PIO_PUER =
	PIOB->PIO_IDR  =
	PIOB->PIO_ODR  =
	PIOB->PIO_PER  = (PIO_PB2);
	
	while(1)
	{
		if (~PIOB->PIO_PDSR & PIO_PB2)
			PIOA->PIO_CODR = PIO_PA22;
		else
			PIOA->PIO_SODR = PIO_PA22;
		if (~PIOA->PIO_PDSR & PIO_PA21)
			PIOB->PIO_CODR = PIO_PB3;
		else
			PIOB->PIO_SODR = PIO_PB3;
	}
}

		

int publish(unsigned char *buffer, float temperatura){
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	char buf[200];
	char temp[200];
	int templen = sizeof(temp);
	int buflen = sizeof(buf); 
	MQTTString topicString = MQTTString_initializer;
	int len = 0;
	
	cdc_uart_put_string("AT+CIPSTART=\"TCP\",\"broker.hivemq.com\",1883\r\n");
  	esp_uart_put_string("AT+CIPSTART=\"TCP\",\"broker.hivemq.com\",1883\r\n");

	
	while(1) {
		esp_uart_read_string(buffer);
		if (!(strcmp(buffer,"OK\r\n"))){
			break;
		} 
		if (!(strcmp(buffer,"ERROR\r\n"))){
			break;
		} 
	} 
	ftoa(temperatura, temp, 0);
	data.clientID.cstring = temp;
  	data.keepAliveInterval = 20;
  	data.cleansession = 1;
  	data.MQTTVersion = 3;
	
	len = MQTTSerialize_connect(buf, buflen, &data);
    topicString.cstring = "Temp";
	len += MQTTSerialize_publish(buf + len, buflen - len, 0, 0, 0, 0, topicString, temp,templen);
	len += MQTTSerialize_disconnect(buf + len, buflen - len);

	
	cdc_uart_put_string("AT+CIPSEND=\r\n");
	esp_uart_put_string("AT+CIPSEND=\r\n");
	ftoa(buflen,buffer,0);
	esp_uart_put_string(buffer);
	esp_uart_put_string("\r\n");
	esp_uart_put_string(buf);
	osDelay(2000);
	cdc_uart_put_string("...done\r\n");
	return 0;
}


int main (void) {

	float temperatura;
	unsigned char buffer[350];

	
	
	// inicializacija itd.
	
	SystemCoreClockUpdate();
	IO1_init();
	
	// ce je ob zagonu/resetu mikrokrmilnika pritisnjena
	// tipka 1, bo mikrokrmilnik �e v nacin delovanje, kjer
	// bo samo posredoval komunikacijo med obema serijskima
	// vmesnikoma; ce nebo tipka pritisnjega, bo �el v
	// v obicajni nacin delovanja
	
	if (IO1_SW(1)) passtrough();
	cdc_uart_init();
		
	// delaj 5000 milisekund - damo cas wifi modulu, da se inicializira
	osDelay(5000);
	
	// posljemo mu ukaz "AT" (izpisi verzijo)
	esp_uart_put_string("AT+RST\r\n");
	
	// beremo vrstico po vrstico izhoda, dokler ne
	// vidimo ali OK ali pa ERROR
	
	do {
		esp_uart_read_string(buffer);
		// v polju buffer imamo zdaj odgovor
		// potem si lahko nekaj z njim zacnemo
		if (!(strcmp(buffer,"OK\r\n"))) break;	
		if (!(strcmp(buffer,"ERROR\r\n"))) break;
	} while (1);
	
	//esp na modul
	//cdc na ra
	//AT komande----------------
	
	cdc_uart_put_string("AT+CWMODE=3\r\n");
	esp_uart_put_string("AT+CWMODE=3\r\n");
	osDelay(1000);
	cdc_uart_put_string(buffer);
	osDelay(1000);
	
	cdc_uart_put_string("AT+CWJAP=\"WiGuestFERI\",\"burek1234\"\r\n");
	esp_uart_put_string("AT+CWJAP=\"WiGuestFERI\",\"burek1234\"\r\n");
	esp_uart_read_string(buffer);
	esp_uart_read_string(buffer);
	esp_uart_read_string(buffer);
	osDelay(1000);
	cdc_uart_put_string(buffer);
  	osDelay(1000);
	
	
	
	while(1)
	{
		
		// beremo temperaturo
		// shrami v zapis z eno decimalko
		// posljemo na cdc uart
		
		temperatura = IO1_read_TEMP();
		ftoa(temperatura, buffer, 1);
		cdc_uart_put_string("Temperatura je: ");
		cdc_uart_put_string(buffer);
		cdc_uart_put_char('\n');
		
		publish(buffer,temperatura);
		osDelay(1000);
		
		
	}
	
	//cdc_uart_put_string("AT+CIPLOSE=len\r\n");
}
