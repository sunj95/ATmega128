/*
 * TimerCounter.c
 *
 * Created: 2019-10-11 오전 11:08:27
 * Author : HSJ
 */ 
#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#define COMPARE_MATCH 1

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

unsigned int msec, sec;

void USART_Init(unsigned int ubrr){
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	UCSR0C = (3<<UCSZ0);
}

void USART_Transmit(char data){
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

void USART_Transmit_String(char *str){
	while(*str != '\0')
		USART_Transmit(*str++);
}

#ifdef OVER_FLOW
void Timer_Init(){
	TCCR0 = (4<<CS0);
	TIMSK = (1<<TOIE0);
	TCNT0 = 6;
}

ISR(TIMER0_OVF_vect){
	msec++;
	if(msec == 1000){
		msec = 0;
		sec++
	}
	if(sec == 100){
		sec = 0;
	}
	TCNT0 = 6;
}
#elif COMPARE_MATCH
void Timer_Init(){
	TCCR0 = (4<<CS0);
	TIMSK = (1<<OCIE0);
	OCR0 = 249;
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
#endif

int led_array[8] = {0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F, 0xFF};
void LED_Init(void){
	DDRF = 0xFF;
	PORTF = 0x00;
}

void Switch_Init(void){
	DDRC = 0x00;
}

int main(void)
{
	USART_Init(MYUBRR);
	Timer_Init();
	LED_Init();
	Switch_Init();
	SREG = 0x80;
	int switch_state[2] = {0,0};
    /* Replace with your application code */
    while (1) 
    { 
		PORTF = led_array[sec%8];
		
		if((PINC&0x01)==0x01){									 // check switch 1 pushed
			if(switch_state[0] == 0) switch_state[0] = 1;		 // if pushed 0 -> 1
			else if(switch_state[0] == 1) switch_state[0] = 0;	 // if pushed 1 -> 0
		}
		if((PINC&0x04)==0x04){									 // check switch 3 pushed
			if(switch_state[1] == 0) switch_state[1] = 1;		 // if pushed 0 -> 1
			else if(switch_state[1] == 1) switch_state[1] = 0;	 // if pushed 1 -> 0
		}
		if((PINC&0x02)==0x02){			// if switch 2 pushed
			msec = 0;					// initialize msec
			sec = 0;					// initialize sec
		}
		_delay_ms(10);					// delay for prevent switch pushed repeatedly
		
		if(switch_state[0] == 1) TCCR0 = (0<<CS0);	// if switch 1 push pause timer
		else TCCR0 = (4<<CS0);						// if switch 1 pushed again, resume timer
		
		if(switch_state[1] == 1) UCSR0B = (0<<RXEN0) | (0<<TXEN0);	// if switch 3 pushed turn off USART
		else UCSR0B = (1<<RXEN0) | (1<<TXEN0);						// if switch 3 pushed again, turn on USART
		
		USART_Transmit_String("Timer : ");
		USART_Transmit(sec/10+48);
		USART_Transmit(sec%10+48);
		USART_Transmit('\r');
		_delay_ms(100);
	}
}

