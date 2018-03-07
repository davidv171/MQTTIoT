/**************************************************************************

 Predloga projekta za razvojno okolje Keil uVision za izvajanje
 laboratorijskih vaj pri predmetu Mobilni in vseprisotni elektronski sistemi
 
 Datoteka: hw_util.h
 
 Knjiznica za delo z strojno opremo 
 
 Ploscica "ESP extension" se mora povezati na prikljucek EXT1
 Ploscica "IETK - ATM XPL - IO1" se mora povezati na prkljucek EXT3
  
 Avtor: Gregor Donaj
 Datum: 26.2.2018
  
 Razlicica 1.0
 
**************************************************************************/

// definicije portov in pinov za LED in tipke za razsiritveni
// modul IO1, kadar ga prikljucimo na EXT3

#define LED1_PIN   PIO_PA14
#define LED2_PIN   PIO_PA12
#define LED3_PIN   PIO_PA13
#define LED4_PIN   PIO_PC10
#define SW1_PIN    PIO_PC31
#define SW2_PIN    PIO_PA0
#define SW3_PIN    PIO_PC22
#define SW4_PIN    PIO_PC21

// funkcije za delo z opremo na razsiritveni ploscici IO1
// inicializacija strojne opreme, delo z LED, branje tipke
// branje temperature, branje vrednosti analognega vhoda

extern void  IO1_init(void);
extern void  IO1_LED_on(int no);
extern void  IO1_LED_off(int no);
extern void  IO1_LED_set(int no, int value);
extern int   IO1_SW(int no);
extern float IO1_read_TEMP(void);
extern float IO1_read_ANALOG(void);

// funkcije za delo s serijskimi vmesniki za povezavo na 
// PC in povezavo na ESP modul na prikljucku EXT1

extern void cdc_uart_init(void);

extern void cdc_uart_put_char(unsigned char c);
extern void cdc_uart_put_string(unsigned char *s);
extern unsigned char cdc_uart_read_char(void);
extern unsigned char cdc_uart_read_char_nb(void);

extern void esp_uart_put_char(unsigned char c);
extern void esp_uart_put_string(unsigned char *s);
extern unsigned char esp_uart_read_char(void);
extern unsigned char esp_uart_read_char_nb(void);

extern void cdc_uart_read_string(unsigned char *s);
extern void esp_uart_read_string(unsigned char *s);

// pretvorba float stevila v niz ASCII znakov

extern void ftoa(float n, unsigned char *res, int afterpoint);
