/*
 * LAB1.c
 *
 * Created: 2019-09-20 오전 3:10:11
 * Author : HSJ
 */ 

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRC = 0x00; // Use Port C as Input
	DDRF = 0xFF; // Use Port F as Output
	
	while(1)
	{
		if( (PINC & 0x01) == 0x01 ) // Check switch 0 pushed
			PORTF = 0xFF; // if pushed, LED On
		else // if switch 0 not pushed
			PORTF = 0x00; // LED Off
		_delay_ms(500);
	}
}

