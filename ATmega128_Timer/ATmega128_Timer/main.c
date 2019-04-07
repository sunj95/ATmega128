/*
 * ATmega128_Timer.c
 *
 * Created: 2019-04-07 오후 7:42:37
 * Author : HSJ
 */ 

// TIMER1 사용
// Normal 모드, prescale /256, Overflow 사용
// 1초간격으로 깜빡이기

// 1초에 16,000,000클럭 → 1초에 62,500 (prescale 적용)
// MAX 값 : 0xFFFF = 65,535 → OVF 설정값 : FFFF-62,500 = 3,035(0x0BDB)

#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned int flag_LED = 0; // 깜빡이기위한 flag

ISR(TIMER1_OVF_vect){
	if(flag_LED == 0){
		PORTA = 0x01; // LED ON
		flag_LED = 1;
	}
	else if (flag_LED == 1){
		PORTA = 0x00; // LED OFF
		flag_LED = 0;
	}
	TCNT0 = 0x0BDB;
}


void main(void)
{
	DDRA = 0x01; // 타이머 확인을 위한 LED 제어
	PORTA = 0x00;
	TCCR1A = 0x00; // Normal mode
	TCCR1B = 0x04; // prescale /256
	TIMSK = 0x04; // Overflow Interrupt Enable
	
	TCNT0 = 0x0000; // 타이머 초기화
	TCNT0 = 0x0BDB; // 타이머 1초 설정
	
	sei(); // 전역 인터럽트 on
    while (1) 
    {
    }
}

