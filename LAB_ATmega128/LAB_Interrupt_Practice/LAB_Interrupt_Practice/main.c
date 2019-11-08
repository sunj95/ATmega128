/*
 * LAB_Interrupt.c
 *
 * Created: 2019-11-01 오전 11:04:10
 * Author : HSJ
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int msec = 0, sec = 0;
char rx_data = 0;
// USART Initialize
void USART_Init(unsigned int ubrr)
{
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)(ubrr);
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0); // Receive Interrupt Enable
	UCSR0C = (3 << UCSZ0);
}
// USART Character Transmit
void USART_Transmit(char data){
	while(!((UCSR0A)&(1<<UDRE0)));
	UDR0 = data;
}
// USART String Transmit
void USART_Transmit_String(char *str){
	while(*str != '\0') USART_Transmit(*str++);
}
// USART Character Receive
char USART_Receive(){
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

// USART0 Receive Interrupt ISR
ISR(USART0_RX_vect){
	rx_data = UDR0;
	UCSR0B |= (1 << UDRIE0); // Enable data buffer empty flag	
}

int main(void)
{
	DDRA = 0xFF; // Initialize LED
	USART_Init(103);  // Initialize USART(9600)
	SREG = 0x80; // Global Interrupt Enable
	
    while (1) 
    {
		if(rx_data == '1') PORTA = 0xFF;		// if USART Interrupt is '1' LED On
		else if (rx_data == '2') PORTA = 0x00;	// if USART Interupt is '2' LED Off
    }
}
