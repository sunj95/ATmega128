/*
 * Game_DaVinci_Code.c
 *
 * Created: 2019-11-18 오후 6:45:33
 * Author : HSJ
 */ 

#include <avr/io.h>
#include <util/delay.h>

/*******************************************************************/
/*                       Function Prototypes                       */
/*******************************************************************/
void USART0_Init(unsigned int ubrr);
void USART0_Transmit(char data);
void USART0_Transmit_String(char *str);
char USART0_Receive(void);
void USART1_Init(unsigned int ubrr);
void USART1_Transmit(char data);
void USART1_Transmit_String(char *str);
char USART1_Receive(void);
void Timer_Init(void);

int state = 0;
struct card_deck{
	char color;		// 0 = black, 1 = white
	char number;	// 0 ~ 11, 12 = Joker
	char owner;		// 0 = none(on field), 1 = player1, 2 = player2
	char opened;	// 0 = not opened(alive), 1 = opened(dead)
};

int main(void)
{
	USART0_Init(103);
	USART1_Init(103);
	Timer_Init();
	char rx;
	char random_value;
	char p1_card_cnt = 0;
	char p2_card_cnt = 0;
	struct card_deck deck[26];
	
	for(int i=0;i<26;i++){				// 카드 덱 초기화
		if(i%2 == 0){
			deck[i].color = 0;
			deck[i].number = i/2;
		}
		else if(i%2 == 1){
			deck[i].color = 1;
			deck[i].number = i/2;
		}
		deck[i].owner = 0;
		deck[i].opened = 0;
	}
    while (1) 
    {
		if(state == 0){ // state 0 : checking player 1 connected
			USART0_Transmit_String("Press Y to Start\r\n");
			USART1_Transmit_String("Waiting for Player 1\r\n");
			rx = USART0_Receive();
			if(rx == 'Y') state = 1;
		} // state 0 end
		if(state == 1){ // state 1 : checking player 1 connected
			USART0_Transmit_String("Waiting for Player 0\r\n");
			USART1_Transmit_String("Press Y to Start\r\n");
			rx = USART1_Receive();
			if(rx == 'Y') state = 2;
		} // state 1 end
		if(state == 2){ // state 2 : give card to players
			USART0_Transmit_String("State 2\r\n");
			while(p1_card_cnt < 4){	// give Player 1 Cards
				random_value = TCNT0%26;
				if(deck[random_value].owner == 0){
					deck[random_value].owner = 1;
					p1_card_cnt++;
					USART_Transmit_number(0, deck[random_value].color);
					USART0_Transmit_String("  ");
					USART_Transmit_number(0, deck[random_value].number);
					USART0_Transmit_String("  ");
				}
			}
			USART0_Transmit_String("\r\n");
			while(p2_card_cnt < 4){	// give Player 2 Cards
				random_value = TCNT0%26;
				if(deck[random_value].owner == 0){
					deck[random_value].owner = 2;
					p2_card_cnt++;
					USART_Transmit_number(1, deck[random_value].color);
					USART1_Transmit_String("  ");
					USART_Transmit_number(1, deck[random_value].number);
					USART1_Transmit_String("  ");
				}
			}
			USART0_Transmit_String("\r\n");
			state = 3;
		} // state 2 end
		
    } // while end
} // main end


/*******************************************************************/
/*                          USER Functions                         */
/*******************************************************************/
void USART_Transmit_number(char USART_NUM,char num){
	if(num == 0){
		if(USART_NUM == 0) USART0_Transmit('0');
		else if(USART_NUM == 1) USART1_Transmit('0');
	}
	else if(num == 1){
		if(USART_NUM == 0) USART0_Transmit('1');
		else if(USART_NUM == 1) USART1_Transmit('1');
	}
	else if(num == 2){
		if(USART_NUM == 0) USART0_Transmit('2');
		else if(USART_NUM == 1) USART1_Transmit('2');
	}
	else if(num == 3){
		if(USART_NUM == 0) USART0_Transmit('3');
		else if(USART_NUM == 1) USART1_Transmit('3');
	}
	else if(num == 4){
		if(USART_NUM == 0) USART0_Transmit('4');
		else if(USART_NUM == 1) USART1_Transmit('4');
	}
	else if(num == 5){
		if(USART_NUM == 0) USART0_Transmit('5');
		else if(USART_NUM == 1) USART1_Transmit('5');
	}
	else if(num == 6){
		if(USART_NUM == 0) USART0_Transmit('6');
		else if(USART_NUM == 1) USART1_Transmit('6');
	}
	else if(num == 7){
		if(USART_NUM == 0) USART0_Transmit('7');
		else if(USART_NUM == 1) USART1_Transmit('7');
	}
	else if(num == 8){
		if(USART_NUM == 0) USART0_Transmit('8');
		else if(USART_NUM == 1) USART1_Transmit('8');
	}
	else if(num == 9){
		if(USART_NUM == 0) USART0_Transmit('9');
		else if(USART_NUM == 1) USART1_Transmit('9');
	}
	else if(num == 10){
		if(USART_NUM == 0) USART0_Transmit_String("10");
		else if(USART_NUM == 1) USART0_Transmit_String("10");
	}
	else if(num == 11){
		if(USART_NUM == 0) USART0_Transmit_String("11");
		else if(USART_NUM == 1) USART0_Transmit_String("11");
	}
	else if(num == 12){
		if(USART_NUM == 0) USART0_Transmit_String("12");
		else if(USART_NUM == 1) USART0_Transmit_String("12");
	}
}




/*******************************************************************/
/*                         Timer Functions                         */
/*******************************************************************/

void Timer_Init(){
	TCCR0 = (4<<CS0);
	TIMSK = (1<<TOIE0);
	TCNT0 = 0;
}

/*******************************************************************/
/*                         USART Functions                         */
/*******************************************************************/

void USART0_Init(unsigned int ubrr){ // Set USART0 Registers
	UBRR0H = (unsigned char)(ubrr >> 8); // UBRR's High bits
	UBRR0L = (unsigned char)(ubrr);		 // UBRR;s Low bits -> UBRR : 103
	UCSR0B = (1<<RXEN0) | (1<<TXEN0); // Receive Enable, Transmit Enable
	UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);			// Set Data bits to 8 bits
}

void USART0_Transmit(char data){
	while(!((UCSR0A)  & (1<<UDRE0))); // Wait for Data Register empty
	UDR0 = data;
}

void USART0_Transmit_String(char *str){
	while(*str != '\0') USART0_Transmit(*str++);
}

char USART0_Receive(void){
	while(!(UCSR0A & (1<<RXC0))); // Wait for Receive Buffer is empty
	return UDR0;
}

void USART1_Init(unsigned int ubrr){ // Set USART0 Registers
	UBRR1H = (unsigned char)(ubrr >> 8); // UBRR's High bits
	UBRR1L = (unsigned char)(ubrr);		 // UBRR;s Low bits -> UBRR : 103
	UCSR1B = (1<<RXEN1) | (1<<TXEN1); // Receive Enable, Transmit Enable
	UCSR1C = (1<<UCSZ10) | (1<<UCSZ11);			// Set Data bits to 8 bits
}

void USART1_Transmit(char data){
	while(!((UCSR1A)  & (1<<UDRE1))); // Wait for Data Register empty
	UDR1 = data;
}

void USART1_Transmit_String(char *str){
	while(*str != '\0') USART1_Transmit(*str++);
}

char USART1_Receive(void){
	while(!(UCSR1A & (1<<RXC1))); // Wait for Receive Buffer is empty
	return UDR1;
}

