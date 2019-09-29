/*
 * USART.c
 *
 * Created: 2019-09-27 오전 11:14:04
 * Author : HSJ
 */ 
#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include <avr/io.h>
#include <util/delay.h>

void USART_init(unsigned int ubrr){
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)(ubrr);
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	UCSR0C = (3<<UCSZ0);
}

void USART_Transmit(char data){
	while(!((UCSR0A) & (1<<UDRE0)));
	UDR0 = data;
}

char USART_Receive(){
	while(!((UCSR0A) & (1<<RXC0)));
	return UDR0;
}

void LED_init (void){
	DDRF = 0xFF;
	PORTF = 0x00;
}

void switch_init (void){
	DDRC = 0x00;
}
/* for assignment 1 */
int main(void)
{
	unsigned char r_data = 0;
	USART_init(MYUBRR);
	LED_init();
    while (1) 
    {
		r_data = USART_Receive();
		PORTF = r_data - 48;
		_delay_ms(100);
    }
}

/*
int main (void){
	USART_init(MYUBRR);
	LED_init();
	switch_init();
	char array[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
	while(1){
		for(int i=0;i<8;i++){
			if( (PINC & array[i]) == array[i]){
				USART_Transmit(i+48);
				PORTF = array[i];
			}
		}
	}
}
*/

