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
int switch1 = 0;

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
	TCCR0 = (4<<CS0) | (1 << WGM01);
	TIMSK = (1<<OCIE0);
	OCR0 = 249;
}

void Interrupt_Init(){
	EIMSK = (1 << INT0) | (1 << INT1);
	EICRA = (1 << ISC01) | (1 << ISC11);
}

ISR(TIMER0_COMP_vect){
	msec++;
	if(msec == 1000){
		msec = 0;
		sec++;
	}
	if(sec == 100){
		sec = 0;
	}
}
/* Assignment 1
ISR(INT0_vect){
	TIMSK = 0;
	PORTA = 0xFF;
}

ISR(INT1_vect){
	TIMSK = (1 << OCIE0);
	PORTA = 0x00;
}
Assignment 1 End */

ISR(INT0_vect){
	if(switch1 == 0) switch1 = 1;
	else switch1 = 0;
}

int main(void)
{
    int a, b;
	int array[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
	int cnt = 0;
	int way = 0;
	DDRA = 0xFF;
	DDRD = 0x00;
	
	USART_Init(103);
	Timer_Init();
	Interrupt_Init();
	
	USART_Transmit_String("Timer: ");
	_delay_ms(10);
	SREG = 0x80;
	
    while (1) 
    {
	    if(switch1 == 0)PORTA = PORTA;
		else if (switch1 == 1){
			PORTA = array[cnt];
			if(way == 0) cnt++;
			else if(way == 1) cnt--;
			if(cnt == 8){
				if(way == 0){
					cnt = 7;
					way = 1;
				}
			}
			else if (cnt == -1){
				if(way == 1){
					cnt = 0;
					way = 0;
				}
			}
			_delay_ms(1000);
		}
		/* Assignment 1
		a = sec/10 + 48;
		b = sec%10 + 48;
		USART_Transmit_String("Timer : ");
		USART_Transmit(a);
		USART_Transmit(b);
		USART_Transmit('\r');
		_delay_ms(100);
		Assignment 1 End */ 
    }
}

