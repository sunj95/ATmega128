/*
 * GccApplication1.c
 *
 * Created: 2019-12-05 오후 8:35:11
 * Author : HSJ
 */ 
#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

unsigned int msec,sec;

int state;
int PASSWORD[4] = {1, 2, 3, 4};
int PS[4] = {0, 0, 0, 0};
int PS_temp;
int LED_flicker[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
unsigned char Level[2] = { 1, 254 };

void USART_init(unsigned int ubrr)
{
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char) ubrr;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (3 << UCSZ0);
}

void USART_Transmit(char data)
{
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

void USART_Transmit_String(char *str)
{
	while( *str !='\0'){
		USART_Transmit(*str++);
	}
}

void ADC_SingleConversion_Init(unsigned char port){
	ADMUX = port;
}

void ADC_FreerunConversion_init(unsigned char port){
	ADMUX = port;
	ADCSRA = (1 << ADEN) | (1 << ADSC) | ( 1 << ADIE ) | (1 << ADPS1) | (1 << ADPS0);
}

int ADC_SingleConversion_read(){
	int output;
	
	ADCSRA |= (1 << ADSC) | (1 << ADEN); 
		
	while(ADCSRA & (1 << ADSC));
	output = ADCL + (ADCH << 8);
	ADCSRA &= ~(1 << ADEN);
	return output;
}

int ADC_FreerunConversion_read(){
	int output;
	
	while(ADCSRA & (1 << ADIF) == 0);
	output = (long)ADCL + ((long)ADCH << 8);
	ADCSRA &= ~(1 << ADEN);
	return output;
}

void USART_Transmit_number(int number){
	USART_Transmit((number / 1000) + '0');
	USART_Transmit(((number % 1000) / 100) + '0');
	USART_Transmit((number % 100) / 10 + '0');
	USART_Transmit((number % 10 + '0'));
}

void Timer_Init(){
	TCCR0 = ( 1 << WGM00 ) | ( 1 << WGM01 ) | ( 1 << COM01 ) | ( 4 << CS0 );
	TIMSK = ( 1 << OCIE0 );
}

void interrupt_Init(){
	EIMSK = ( 1 << INT0 ) | ( 1 << INT1 );
	EICRA = ( 1 << ISC01 ) | ( 1 << ISC11 );
}

void Check_Password(){
	if(PS == PASSWORD){
		USART_Transmit_String("$C\r");
		USART_Transmit_String("$G, 1, 7\r");
		USART_Transmit_String("$T,");
		USART_Transmit_String("Correct!");
		state = 8;
	}
	else{
		USART_Transmit_String("$C\r");
		USART_Transmit_String("$G, 1, 7\r");
		USART_Transmit_String("$T,");
		USART_Transmit_String("Wrong Password!");
		state = 1;
	}
}

void Open_Door(){
	for(int i = 0; i < 2; i++){
		OCR0 = Level[1];
		_delay_ms(5000);
	}
}

ISR(INT0_vect){
	PS_temp = 1;
}
ISR(INT1_vect){
	PS_temp = 2;
}
ISR(INT2_vect){
	PS_temp = 3;
}
ISR(INT3_vect){
	PS_temp = 4;
}
ISR(INT4_vect){
	PS_temp = 5;
}
ISR(INT5_vect){
	PS_temp = 6;
}
ISR(INT6_vect){
	PS_temp = 7;
}
ISR(INT7_vect){
	PS_temp = 8;
}

int main(void)
{
	int ADC_Output1;
	int ADC_Output2;
	int CS = 0;
	
	DDRD = 0x00; // 스위치 입력
	DDRF = 0x00; // 적외선 입력
	DDRA = 0xFF; // LCD 출력
	//DDRC = 0xFF; // LED 출력
	DDRB = 0xFF; // 모터 출력   
	
	SREG = 0x80;
	state = 0;
	
	USART_init(MYUBRR);
	interrupt_Init();
	Timer_Init();
	USART_Transmit_String("$I\r");
	
	while(1){
		
		ADC_SingleConversion_Init(0x01);
		ADC_Output1 = ADC_SingleConversion_read();
		_delay_ms(100);
		ADC_SingleConversion_Init(0x02);
		ADC_Output2 = ADC_SingleConversion_read();
		_delay_ms(100);
		
				if(state == 0){	
					if(ADC_Output1 > 600 || ADC_Output2 > 600 ){
						state = 1;
					}
				}
				else if(state == 1){
					USART_Transmit_String("$C\r");
					USART_Transmit_String("$G, 1, 2\r");
					USART_Transmit_String("$T,");
					USART_Transmit_String("Enter Password");
					USART_Transmit('\r');
					if((PIND & 0b10000001) == 0b10000001){
						state == 2;
					}
					else if(PIND == 0x01 | PIND == 0x02 | PIND == 0x04 | PIND == 0x08 | PIND == 0x10 | PIND == 0x20 | PIND == 0x40 | PIND == 0x80){
						PS[0] = PS_temp;
						state == 3;
					}
				}
				else if(state == 2){
					if(CS == 0){
						if(PIND == 0x01 | PIND == 0x02 | PIND == 0x04 | PIND == 0x08 | PIND == 0x10 | PIND == 0x20 | PIND == 0x40 | PIND == 0x80){
							PASSWORD[0] = PS_temp;
							USART_Transmit_String("$G, 2, 7\r");
							USART_Transmit_String("$T,");
							USART_Transmit_String(PS[0]);
							USART_Transmit('\r');
							CS == 1;
						}
					}
					else if(CS == 1){
						if(PIND == 0x01 | PIND == 0x02 | PIND == 0x04 | PIND == 0x08 | PIND == 0x10 | PIND == 0x20 | PIND == 0x40 | PIND == 0x80){
							PASSWORD[1] = PS_temp;
							USART_Transmit_String("$G, 2, 8\r");
							USART_Transmit_String("$T,");
							USART_Transmit_String(PS[1]);
							USART_Transmit('\r');
							CS == 2;
						}
					}
					else if(CS == 2){
						if(PIND == 0x01 | PIND == 0x02 | PIND == 0x04 | PIND == 0x08 | PIND == 0x10 | PIND == 0x20 | PIND == 0x40 | PIND == 0x80){
							PASSWORD[2] = PS_temp;
							USART_Transmit_String("$G, 2, 9\r");
							USART_Transmit_String("$T,");
							USART_Transmit_String(PS[2]);
							USART_Transmit('\r');
							CS == 3;
						}
					}
					else if(CS == 3){
						if(PIND == 0x01 | PIND == 0x02 | PIND == 0x04 | PIND == 0x08 | PIND == 0x10 | PIND == 0x20 | PIND == 0x40 | PIND == 0x80){
							PASSWORD[3] = PS_temp;
							USART_Transmit_String("$G, 2, 10\r");
							USART_Transmit_String("$T,");
							USART_Transmit_String(PS[3]);
							USART_Transmit('\r');
							_delay_ms(750);
							
							USART_Transmit_String("$C\r");
							USART_Transmit_String("$G, 1, 4\r");
							USART_Transmit_String("$T,");
							USART_Transmit_String("completed!");
							USART_Transmit('\r');
							state = 1;
						}
					}
				 }
				else if(state == 3){
						USART_Transmit_String("$G, 2, 7\r");
						USART_Transmit_String("$T,");
						USART_Transmit_String(PS[0]);
						USART_Transmit('\r');
						state = 4;
					}
				else if(state == 4){
					if(PIND == 0x01 | PIND == 0x02 | PIND == 0x04 | PIND == 0x08 | PIND == 0x10 | PIND == 0x20 | PIND == 0x40 | PIND == 0x80){
						PS[1] = PS_temp;
						USART_Transmit_String("$G, 2, 8\r");
						USART_Transmit_String("$T,");
						USART_Transmit_String(PS[1]);
						USART_Transmit('\r');
						state = 5;
					}
				}
				else if(state == 5){
					if(PIND == 0x01 | PIND == 0x02 | PIND == 0x04 | PIND == 0x08 | PIND == 0x10 | PIND == 0x20 | PIND == 0x40 | PIND == 0x80){
						PS[2] = PS_temp;
						USART_Transmit_String("$G, 2, 9\r");
						USART_Transmit_String("$T,");
						USART_Transmit_String(PS[2]);
						USART_Transmit('\r');
						state = 6;
					}
				}
				else if(state == 6){
					if(PIND == 0x01 | PIND == 0x02 | PIND == 0x04 | PIND == 0x08 | PIND == 0x10 | PIND == 0x20 | PIND == 0x40 | PIND == 0x80){
						PS[3] = PS_temp;
						USART_Transmit_String("$G, 2, 10\r");
						USART_Transmit_String("$T,");
						USART_Transmit_String(PS[3]);
						USART_Transmit('\r');
						state = 7;
					}
				}
				else if(state == 7){
					Check_Password();
					state = 8;
				}
				else if(state == 8){
					for(int i = 0; i < 8; i++){
						PORTF = LED_flicker[i];
						_delay_ms(100);
					}
					for(int i = 7; i >= 0; i--){
						PORTF = LED_flicker[i];
						_delay_ms(100);
					}
					Open_Door();
					state = 0;
				}
			}
	}
				