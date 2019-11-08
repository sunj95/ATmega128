/*
 * LAB_Interrupt.c
 *
 * Created: 2019-11-01 오전 11:04:10
 * Author : HSJ
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int switch1 = 0;

// Interrupt Initialize
void Interrupt_Init(){
	EIMSK = (1 << INT0) | (1 << INT1);   // External Interrupt 1, 0 Enable
	EICRA = (1 << ISC01) | (1 << ISC11); // INT1, INT0 Falling Edge Interrupt
}

// INT0 Falling Edge Interrupt
ISR(INT0_vect){
	if(switch1 == 0) switch1 = 1; // switch1 variable control
	else switch1 = 0;
}

int main(void)
{
	int array[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80}; // array for LED
	int cnt = 0; // count for led
	int way = 0; // variable for know direction of led
	DDRA = 0xFF; // Initialize LED
	DDRD = 0x00; // Initialize Switch
	Interrupt_Init(); // Initialize Interrupt
	SREG = 0x80; // Global Interrupt Enable
	
    while (1) 
    {
	    if(switch1 == 0)PORTA = PORTA; // if state 0, Stop LED
		else if (switch1 == 1){		   // if state 1,
			PORTA = array[cnt];		   // shift LED by array
			if(way == 0) cnt++;		   // if direction is up, cnt++
			else if(way == 1) cnt--;   // if direction is down, cnt--
			if(cnt == 8){			   // if LED goes to upper-end
				if(way == 0){		   // if direction is up
					cnt = 7;		   // led down
					way = 1;		   // change direction
				}
			}
			else if (cnt == -1){		// if LED is floor
				if(way == 1){			// if direction is down
					cnt = 0;			// led up
					way = 0;			// change direction
				}
			}
			_delay_ms(1000);			// Delay for LED display
		}
    }
}
