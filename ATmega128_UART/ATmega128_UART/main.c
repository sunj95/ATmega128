/*
 * ATmega128_UART.c
 *
 * Created: 2019-04-17 오후 4:23:02
 * Author : HSJ
 * UART1, Baud Rate : 9600, Data : 8bit, Parity : None, Stop bits : 1bit, Flow Control : None
 * 
 */ 

#include <avr/io.h>
#include <util/delay.h>

void USART_init(void) // USART 초기화
{
	DDRD = 0xFB;
	PORTD = 0x00;

	UCSR1A = 0x00;
	UCSR1B = 0x18;
	UCSR1C = 0x06;
	UBRR1H = 0;
	UBRR1L = 103;

	TxChar(0x0D);
	TxChar(0x0A);
}

void TxChar(unsigned char chr) // USART 1글자 전송
{
	while((UCSR1A & 0x20 ) == 0);
	UDR1 = chr;
	UCSR1A |= 0x20;
}

void TxString (unsigned char *str ) // USART 문자열 전송
{
	int i = 0;
	while(str[i] != 0)
	{
		TxChar(str[i]);
		i++;
	}
}

char RxChar(void)
{
	while(!(UCSR1A & 0x80));
	return UDR1;
}


main()
{
	USART_init();
	char temp = 0;
	while(1){
		TxString("Insert Number : \r\n");
		temp = RxChar();
		TxString("Inserted Number : ");
		TxChar(temp);
		TxString("\r\n");
		_delay_ms(2000);
	}
}
