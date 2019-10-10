/*
 * USART_LCD.c
 *
 * Created: 2019-10-04 오전 2:06:11
 * Author : HSJ
 */ 

#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

void USART_Init(unsigned int ubrr){ // Set USART0 Registers
	UBRR0H = (unsigned char)(ubrr >> 8); // UBRR's High bits
	UBRR0L = (unsigned char)(ubrr);		 // UBRR;s Low bits -> UBRR : 103
	UCSR0B = (1<<RXEN0) | (1<<TXEN0); // Receive Enable, Transmit Enable
	UCSR0C = (3<<UCSZ0);			// Set Data bits to 8 bits
}

void USART_Transmit(char data){
	while(!((UCSR0A)  & (1<<UDRE0))); // Wait for Data Register empty
	UDR0 = data;
}

void USART_Transmit_String(char *str){
	while(*str != '\0') USART_Transmit(*str++);
}

char USART_Receive(void){
	while(!(UCSR0A & (1<<RXC0))); // Wait for Receive Buffer is empty
	return UDR0;
}
void LCD_Init(void){
	USART_Transmit_String("$I\r"); // LCD Initialize
	USART_Transmit_String("$C\r"); // Clear LCD
	USART_Transmit_String("$B,0\r"); // Cursor Off
	USART_Transmit_String("$D,1\r"); // Display On
	USART_Transmit_String("$L,1\r"); // Back Light On
}

void LCD_Transmit(int x, int y, char data){
	USART_Transmit_String("$G,");
	USART_Transmit((char)x+48); // convert integer number x to ascii number
	USART_Transmit(',');
	USART_Transmit((char)y+48); // convert integer number y to ascii number
	USART_Transmit('\r');
	USART_Transmit_String("$T,");
	USART_Transmit(data);		// transmit text
	USART_Transmit('\r');
}

void LCD_Transmit_String(int x, int y, char *string){
	USART_Transmit_String("$G,");
	USART_Transmit((char)x+48); // convert integer number x to ascii number
	USART_Transmit(',');
	USART_Transmit((char)y+48); // convert integer number y to ascii number
	USART_Transmit('\r');
	USART_Transmit_String("$T,");
	USART_Transmit_String(string);	// transmit string
	USART_Transmit('\r');
}

void LCD_Transmit_String_Flow(char *string){
	int string_L = strlen(string);		// get string length
	for(int i=0;i<string_L-31;i++){		// print string until string finish
		LCD_Transmit_String(1,1,string+i);	  // string print for x=1
		LCD_Transmit_String(2,1,string+i+16); // string print for x=2
		_delay_ms(100);
	}
}

int main(void){
	USART_Init(MYUBRR); // USART initialize with Baud rate 9600
	LCD_Init();
	char str[] = "Hwang Seon Jeong Department of Electronic Engineering";
	
	LCD_Init();
	LCD_Transmit_String_Flow(str);
	

	while(1){
		_delay_ms(100);
	}
}

