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
/*
ISR(USART0_RX_vect){
	rx_data = UDR0;
	if(rx_data == '\n'){
		servo = (servo_data[2]+48)*100 + (servo_data[1]+48)*10 + (servo_data[0]+48);
		for(int j=0;j<3;j++) servo_data[j] = '0';
		send_once = 0;
	}
	servo_data[count_rx] = rx_data;
	if(count_rx < 2) count_rx = 0;
	else count_rx++;
	UCSR0B |= (1 << UDRIE0);
}
*/
void Timer_Init(){
	TCCR0 = (1 << WGM00)|(1 << WGM01)|(1 << COM01)|(4 << CS0);
	TIMSK = (1 << OCIE0);
}

int main(void)
{
	Timer_Init();
	DDRB = 0xFF;
	USART_Init(103);
	char rx_data;
	char servo_data[3];
	int servo = 0;
	int count_rx = 0;
	int send_once = 0;
    /* Replace with your application code */
    while (1) 
    {
		if(send_once == 0){
			USART_Transmit("PWM : ");
	//		send_once = 1;
		}
		OCR0 = servo;
    }
}

