/*
 * PWM_Servo_Motor.c
 *
 * Created: 2019-11-08 오전 11:04:41
 * Author : HSJ
 */ 

#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL

void USART_Init(unsigned int ubrr)
{
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)(ubrr);
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (3 << UCSZ0);
}

void USART_Transmit(char data){
	while(!((UCSR0A)&(1<<UDRE0)));
	UDR0 = data;
}

void USART_Transmit_String(char *str){
	while(*str != '\0') USART_Transmit(*str++);
}

char USART_Receive(){
	while(!(UCSR0A & (1<<RXC0)));
	return UDR0;
}

void Timer_Init(){
	TCCR0 = (1 << WGM00)|(1 << WGM01)|(1 << COM01)|(4 << CS0);
	TIMSK = (1 << OCIE0);
}

int main(void)
{
	Timer_Init();
	DDRB = 0xFF;
	USART_Init(103);
	unsigned char rx_data;		// for USART Data save
	unsigned int t_rx_data;		// Data conversion
	unsigned int rx_cnt = 0;	// count for buffer
	unsigned int rx_buffer[3] = {0,}; // buffer for data save
	unsigned int servo = 0;		// for servo degree
	while(1)
	{
		rx_data = USART_Receive(); // get data
		t_rx_data = rx_data - 48;  // converse ascii to integer
		if((t_rx_data >= 0) && (t_rx_data <= 9)){ // if data is single number
			rx_buffer[rx_cnt] = t_rx_data;		  // save data at buffer
			rx_cnt++;							  // buffer count +
			if(rx_cnt == 3){					 // if buffer is full
				servo = rx_buffer[2]*100 + rx_buffer[1]*10 + rx_buffer[0];	// make servo value
				rx_cnt = 0;						  // initialize buffer count
			}
		}
		OCR0 = servo;							// push servo degree
		
	}
	
}

