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

void USART_init(unsigned int ubrr){ // Set USART0 Registers
	UBRR0H = (unsigned char)(ubrr >> 8); // UBRR's High bits
	UBRR0L = (unsigned char)(ubrr);		 // UBRR;s Low bits -> UBRR : 103
	UCSR0B = (1<<RXEN0) | (1<<TXEN0); // Receive Enable, Transmit Enable
	UCSR0C = (3<<UCSZ0);			// Set Data bits to 8 bits
}

void USART_Transmit(char data){
	while(!((UCSR0A) & (1<<UDRE0))); // Wait for Data Register empty
	UDR0 = data;
}

char USART_Receive(){
	while(!((UCSR0A) & (1<<RXC0))); // Wait for Receive Buffer is empty
	return UDR0;
}

void LED_init (void){ // function for Led initialization
	DDRF = 0xFF; // use PORTF as output
	PORTF = 0x00; // initialize PORT as 0
}

void switch_init (void){ // function for switch initialization
	DDRC = 0x00; // use PORTC as input
}

int main (void){
	USART_init(MYUBRR); // USART initialize with Baud rate 9600
	LED_init(); // LED initialize
	switch_init(); // switch initialize
	char array[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}; // array to use for loop
	while(1){
		for(int i=0;i<8;i++){ // check switch for PC0~PC7
			if( (PINC & array[i]) == array[i]){ // check switch
				USART_Transmit((char)(i+48)); // transmit switch input to ascii code
				PORTF = array[i]; // Turn on LED on the corresponding switch
			}
		}
	}
}

/*
int main(void)
{
	unsigned char r_data = 0; // use for save Received data
	USART_init(MYUBRR); // USART initialize with Baud rate 9600
	LED_init(); // LED initialize
    while (1) 
    {
		r_data = USART_Receive(); // put Received Data at r_data
		PORTF = r_data - 48; // convert ascii number to number
		_delay_ms(100);
    }
}
*/

