/*
 * ATmega128_LED.c
 *
 * Created: 2019-04-07 오후 6:03:32
 * Author : HSJ
 */ 

#include <avr/io.h>


int main(void){
	DDRA = 0x01;	// LED : PA0
	PORTA = 0x01;	// LED ON
	while (1); // Infinite Loop
}

