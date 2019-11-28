/*
 * ATmega128_SPI_Master.c
 *
 * Created: 2019-11-25 오후 10:16:33
 * Author : HSJ
 */ 

/*
ATmega128         ATmega128
Master            Slave
////////////////////////////
PB4---------------PB0(/SS)
PB1(SCK)----------PB1(SCK)
PB2(MOSI)---------PB2(MOSI)
PB3(MISO)---------PB3(MISO)
////////////////////////////
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define SS_1     PORTB |= 0x10
#define SS_0     PORTB &=~0x10

volatile unsigned char rx_buf, count=0;

void USART0_Init(unsigned int ubrr){ // Set USART0 Registers
	UBRR0H = (unsigned char)(ubrr >> 8); // UBRR's High bits
	UBRR0L = (unsigned char)(ubrr);		 // UBRR;s Low bits -> UBRR : 103
	UCSR0B = (1<<RXEN0) | (1<<TXEN0); // Receive Enable, Transmit Enable
	UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);			// Set Data bits to 8 bits
}

void USART0_Transmit(char data){
	while(!((UCSR0A)  & (1<<UDRE0))); // Wait for Data Register empty
	UDR0 = data;
}

void USART0_Transmit_String(char *str){
	while(*str != '\0') USART0_Transmit(*str++);
}

char USART0_Receive(void){
	while(!(UCSR0A & (1<<RXC0))); // Wait for Receive Buffer is empty
	return UDR0;
}


void SPI_Masterinit(void){
	DDRB|=0x17;
	SPCR|=0xD0;
	SS_1;
}

void SPI_TX(unsigned char data){
	SS_0;
	SPCR&=~0x80;
	SPDR=data;
	while(!(SPSR&0x80));
	SPCR|=0x80;
}

ISR(SPI_STC_vect){
	rx_buf=SPDR;
	//SS_1;
}



int main(){
	DDRA = 0xFF;
	SPI_Masterinit();
	USART0_Init(103);
	SREG = 0x80;
	
	char rx;
	
	while(1){
		USART0_Transmit(rx_buf);
		_delay_ms(10);
	}
}

