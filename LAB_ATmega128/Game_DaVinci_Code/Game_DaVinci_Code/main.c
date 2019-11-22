/*
 * Game_DaVinci_Code.c
 *
 * Created: 2019-11-18 오후 6:45:33
 * Author : HSJ
 */ 

#include <avr/io.h>
#include <util/delay.h>
#define NUM_CARD 24
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
	char rx_1;
	char random_value;
	char p1_card_cnt = 0;
	char p2_card_cnt = 0;
	char temp = 0;
	char get_number_1[3];
	char get_number_2[3];
	
	struct card_deck deck[NUM_CARD];
	struct card_deck p1_deck[NUM_CARD/2];
	struct card_deck p2_deck[NUM_CARD/2];
	
	for(int i=0;i<NUM_CARD;i++){				// 카드 덱 초기화
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
	for(int i=0;i<NUM_CARD/2;i++){
		p1_deck[1].color = -1;
		p1_deck[1].number = -1;
		p1_deck[1].owner = 1;
		p1_deck[1].opened = 0;
		p1_deck[1].color = -1;
		p1_deck[1].number = -1;
		p1_deck[1].owner = 2;
		p1_deck[1].opened = 0;
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
			USART0_Transmit_String("Waiting for Player 2\r\n");
			USART1_Transmit_String("Press Y to Start\r\n");
			rx = USART1_Receive();
			if(rx == 'Y') state = 2;
		} // state 1 end
		if(state == 2){ // state 2 : give card to players
			while(p1_card_cnt < 4){	// give Player 1 Cards
				random_value = TCNT0%NUM_CARD;
				if(deck[random_value].owner == 0){
					deck[random_value].owner = 1;
					p1_card_cnt++;
				}
			}
			while(p2_card_cnt < 4){	// give Player 2 Cards
				random_value = TCNT0%NUM_CARD;
				if(deck[random_value].owner == 0){
					deck[random_value].owner = 2;
					p2_card_cnt++;
				}
			}
			temp = 0;
			USART0_Transmit_String("P1 Deck : ");
			for(int i=0; i<NUM_CARD; i++){
				if(deck[i].owner == 1){
					p1_deck[temp] = deck[i];
					temp++;
					USART_Transmit_number(0, p1_deck[temp].color); USART0_Transmit(0);
					USART_Transmit_number(0, p1_deck[temp].number); USART0_Transmit(0);
					USART0_Transmit_String("   "); _delay_ms(1);
				}
			}
			temp = 0;
			USART1_Transmit_String("P2 Deck : ");
			for(int i=0; i<NUM_CARD; i++){
				if(deck[i].owner == 2){
					p2_deck[temp] = deck[i];
					USART_Transmit_number(1, p2_deck[temp].color); USART1_Transmit(0);
					USART_Transmit_number(1, p2_deck[temp].number); USART1_Transmit(0);
					USART1_Transmit_String("   "); _delay_ms(1);
					temp++;
				}
			}
			USART1_Transmit_String("\r\n");
			USART1_Transmit_String("P2 Deck : ");
			for(int i=0;i<4;i++){
				USART_Transmit_number(1, p2_deck[i].color); _delay_ms(1);
				USART_Transmit_number(1, p2_deck[i].number); _delay_ms(1);
				USART1_Transmit_String("   "); _delay_ms(1);
			}
			state = 3;
		} // state 2 end
		if(state == 3){ // state : give 1 card to player 1
			temp = p1_card_cnt + 1;
			while(p1_card_cnt < temp){
				random_value = TCNT0%NUM_CARD;
				if(deck[random_value].owner == 0){
					deck[random_value].owner = 1;
					p1_card_cnt++;
				}
			}
			USART0_Transmit_String("P1 Deck : ");
			for(int i=0; i<NUM_CARD; i++){
				if(deck[i].owner == 1){
					p1_deck[temp] = deck[i];
					temp++;
					USART_Transmit_number(0, p1_deck[temp].color); USART0_Transmit(0);
					USART_Transmit_number(0, p1_deck[temp].number); USART0_Transmit(0);
					USART0_Transmit_String("   "); _delay_ms(1);
				}
			}
			state = 5;
		}
		
		if(state == 4){ // state : give 1 card to player 2
			temp = p2_card_cnt + 1;
			while(p2_card_cnt < temp){
				random_value = TCNT0%NUM_CARD;
				if(deck[random_value].owner == 0){
					deck[random_value].owner = 2;
					p2_card_cnt++;
				}
			}
			USART1_Transmit_String("P2 Deck : ");
			for(int i=0; i<NUM_CARD; i++){
				if(deck[i].owner == 2){
					p2_deck[temp] = deck[i];
					USART_Transmit_number(1, p2_deck[temp].color); USART1_Transmit(0);
					USART_Transmit_number(1, p2_deck[temp].number); USART1_Transmit(0);
					USART1_Transmit_String("   "); _delay_ms(1);
					temp++;
				}
			}
		}
		if(state == 5){ // state 3
			_delay_ms(1);
			USART0_Transmit_String("\r\nChoose Card to Open : "); // 몇 번째에 있는 카드 선택인지
			rx = USART0_Receive();
			if((rx >= '0')&&(rx <= '9')){
				get_number_1[0] = rx-48;
				state = 6;	
			}
		} // state 3 end
		if(state == 6){ // state 4
			rx = USART0_Receive();
			if((rx >= '0')&&(rx <= '9')){
				get_number_1[1] = rx-48;
				get_number_1[2] = get_number_1[0]*10 + get_number_1[1] - 1;
				USART1_Transmit_String("\r\nArray number : ");
				USART_Transmit_number(1,get_number_1[2]);
				_delay_ms(1);
				state = 7;
			}			
		} // state 4 end
		if(state == 7){ // state 5
			USART0_Transmit_String("\r\nNumber? : "); // 카드가 무엇인지
			rx = USART0_Receive();
			if((rx >= '0')&&(rx <= '9')){
				get_number_2[0] = rx-48;
				state = 8;
			}
		} // state 5 end
		if(state == 8){ // state 8
			rx = USART0_Receive();
			if((rx >= '0')&&(rx <= '9')){
				get_number_2[1] = rx-48;
				get_number_2[2] = get_number_2[0]*10 + get_number_2[1];
				USART1_Transmit_String("\r\nTarget number : "); _delay_ms(1);
				USART_Transmit_number(1,get_number_2[2]); _delay_ms(1);
				USART1_Transmit_String("\r\nReal number : "); _delay_ms(1);
				USART_Transmit_number(1,p2_deck[(int)get_number_1[2]].number); _delay_ms(1);
				state = 9;
			}
		} // state 8 end
		if(state == 9){ // state 5
			temp = p2_deck[(int)get_number_1[2]].number;
			if(temp == get_number_2[2]){
				USART1_Transmit_String("\r\nCorrect\r\n");
				state = 5;
			}
			else{
				USART1_Transmit_String("\r\nWrong\r\n");
				state = 4;
			}
			
		}
		
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
	TCCR0 = (1<<CS0);
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

