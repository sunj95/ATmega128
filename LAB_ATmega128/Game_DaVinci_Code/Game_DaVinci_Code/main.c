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
void USART_Transmit_number(char USART_NUM,char num);
void showP1toP2(void);
void showP2toP1(void);

int state = 0;
struct card_deck{
	char color;		// 0 = black, 1 = white
	char number;	// 0 ~ 11, 12 = Joker
	char owner;		// 0 = none(on field), 1 = player1, 2 = player2
	char opened;	// 0 = not opened(alive), 1 = opened(dead)
};

char rx;
char rx_1;
char random_value;
char p1_card_cnt = 0;
char p2_card_cnt = 0;
char p1_opened_cnt = 0;
char p2_opened_cnt = 0;
char temp = 0;
char get_number_1[3];
char get_number_2[3];
char p1_corr_flag = 0;
char p2_corr_flag = 0;
	
struct card_deck deck[NUM_CARD];
struct card_deck p1_deck[NUM_CARD/2];
struct card_deck p2_deck[NUM_CARD/2];

int main(void)
{
	USART0_Init(103);
	USART1_Init(103);
	Timer_Init();
	
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
		p2_deck[1].color = -1;
		p2_deck[1].number = -1;
		p2_deck[1].owner = 2;
		p2_deck[1].opened = 0;
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
			USART0_Transmit_String("P1 Deck : "); // display P1 deck to P1
			for(int i=0; i<NUM_CARD; i++){
				if(deck[i].owner == 1){
					p1_deck[temp] = deck[i];
					USART_Transmit_number(0, p1_deck[temp].color);
					USART_Transmit_number(0, p1_deck[temp].number); 
					USART0_Transmit_String("   ");
					temp++;
				}
			}
			USART0_Transmit_String("\r\n");
			temp = 0;
			USART1_Transmit_String("P2 Deck : "); // display P2 deck to P2
			for(int i=0; i<NUM_CARD; i++){
				if(deck[i].owner == 2){
					p2_deck[temp] = deck[i];
					USART_Transmit_number(1, p2_deck[temp].color);
					USART_Transmit_number(1, p2_deck[temp].number); 
					USART1_Transmit_String("   ");
					temp++;
				}
			}
			USART1_Transmit_String("\r\n");
			showP2toP1();
			showP1toP2();				
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
			temp = 0;
			USART0_Transmit_String("P1 Deck : ");
			for(int i=0; i<NUM_CARD; i++){
				if(deck[i].owner == 1){
					p1_deck[temp] = deck[i];
					USART_Transmit_number(0, p1_deck[temp].color);
					USART_Transmit_number(0, p1_deck[temp].number);
					USART0_Transmit_String("   ");
					temp++;
				}
			}
			USART0_Transmit_String("\r\n");
			showP2toP1();
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
			temp = 0;
			USART1_Transmit_String("P2 Deck : ");
			for(int i=0; i<NUM_CARD; i++){
				if(deck[i].owner == 2){
					p2_deck[temp] = deck[i];
					USART_Transmit_number(1, p2_deck[temp].color);
					USART_Transmit_number(1, p2_deck[temp].number);
					USART1_Transmit_String("   ");
					temp++;
				}
			}
			USART1_Transmit_String("\r\n");
			showP1toP2();
			state = 10;
		}
		if(state == 5){ // state 5 
			//_delay_ms(1);
			if(p1_corr_flag == 0) USART0_Transmit_String("Choose Card to Open : ");
			else if(p1_corr_flag == 1) USART0_Transmit_String("Choose Card to Open, Press P to Pass : "); // 몇 번째에 있는 카드 선택인지
			rx = USART0_Receive();
			USART0_Transmit(rx);
			if((rx >= '0')&&(rx <= '9')){
				get_number_1[0] = rx-48;
				state = 6;	
			}
			if(p1_corr_flag == 1 && rx == 'P'){
				USART0_Transmit_String("\r\n");
				USART0_Transmit_String("Passed\r\n");
				USART0_Transmit_String("-------------------------------------------\r\n");
				USART1_Transmit_String("Player 1 Passed, It's your turn\r\n");
				state = 4;
			}
		} // state 3 end
		if(state == 6){ // state 4
			p1_corr_flag = 0;
			rx = USART0_Receive();
			USART0_Transmit(rx);
			USART0_Transmit_String("\r\n");
			if((rx >= '0')&&(rx <= '9')){
				get_number_1[1] = rx-48;
				get_number_1[2] = get_number_1[0]*10 + get_number_1[1] - 1;
				//_delay_ms(1);
				state = 7;
			}			
		} // state 4 end
		if(state == 7){ // state 5
			USART0_Transmit_String("Number? : "); // 카드가 무엇인지
			rx = USART0_Receive();
			USART0_Transmit(rx);
			if((rx >= '0')&&(rx <= '9')){
				get_number_2[0] = rx-48;
				state = 8;
			}
		} // state 5 end
		if(state == 8){ // state 8
			rx = USART0_Receive();
			USART0_Transmit(rx);
			USART0_Transmit_String("\r\n");
			if((rx >= '0')&&(rx <= '9')){
				get_number_2[1] = rx-48;
				get_number_2[2] = get_number_2[0]*10 + get_number_2[1];
				state = 9;
			}
		} // state 8 end
		if(state == 9){ // state 5
			temp = p2_deck[(int)get_number_1[2]].number;
			if(temp == get_number_2[2]){
				USART0_Transmit_String("Correct\r\n");
				p2_deck[(int)get_number_1[2]].opened = 1;
				for(int i=0;i<NUM_CARD;i++){
					if(deck[i].number == p2_deck[(int)get_number_1[2]].number){
						if(deck[i].color == p2_deck[(int)get_number_1[2]].color){
							deck[i].opened = 1;
						}
					}
				}		
				showP2toP1();
				p2_opened_cnt++;
				p1_corr_flag = 1;
				if(p2_card_cnt == p2_opened_cnt) state = 30;
				else state = 5;
			}
			else{
				USART0_Transmit_String("Wrong\r\n");
				USART0_Transmit_String("Player 2's Turn\r\n");
				USART0_Transmit_String("-------------------------------------------\r\n");
				state = 4;
			}
			
		}
		if(state == 10){ // state 10
			//_delay_ms(1);
			if(p2_corr_flag == 0) USART1_Transmit_String("Choose Card to Open : ");
			else if(p2_corr_flag == 1) USART1_Transmit_String("Choose Card to Open, Press P to Pass : "); // 몇 번째에 있는 카드 선택인지
			rx = USART1_Receive();
			USART1_Transmit(rx);
			if((rx >= '0')&&(rx <= '9')){
				get_number_1[0] = rx-48;
				state = 11;
			}
			if(p2_corr_flag == 1 && rx == 'P'){
				USART1_Transmit_String("\r\n");
				USART1_Transmit_String("Passed\r\n");
				USART1_Transmit_String("-------------------------------------------\r\n");
				USART0_Transmit_String("Player 2 Passed, It's your turn\r\n");
				state = 3;
			}
		} // state 10 end
		if(state == 11){ // state 11
			p2_corr_flag = 0;
			rx = USART1_Receive();
			USART1_Transmit(rx);
			USART1_Transmit_String("\r\n");
			if((rx >= '0')&&(rx <= '9')){
				get_number_1[1] = rx-48;
				get_number_1[2] = get_number_1[0]*10 + get_number_1[1] - 1;
				//_delay_ms(1);
				state = 12;
			}
		} // state 11 end
		if(state == 12){ // state 12
			USART1_Transmit_String("Number? : "); // 카드가 무엇인지
			rx = USART1_Receive();
			USART1_Transmit(rx);
			if((rx >= '0')&&(rx <= '9')){
				get_number_2[0] = rx-48;
				state = 13;
			}
		} // state 12 end
		if(state == 13){ // state 13
			rx = USART1_Receive();
			USART1_Transmit(rx);
			USART1_Transmit_String("\r\n");
			if((rx >= '0')&&(rx <= '9')){
				get_number_2[1] = rx-48;
				get_number_2[2] = get_number_2[0]*10 + get_number_2[1];
				state = 14;
			}
		} // state 13 end
		if(state == 14){ // state 14
			temp = p1_deck[(int)get_number_1[2]].number;
			if(temp == get_number_2[2]){
				USART1_Transmit_String("Correct\r\n");
				p1_deck[(int)get_number_1[2]].opened = 1;
				for(int i=0;i<NUM_CARD;i++){
					if(deck[i].number == p1_deck[(int)get_number_1[2]].number){
						if(deck[i].color == p1_deck[(int)get_number_1[2]].color){
							deck[i].opened = 1;
						}
					}
				}
				showP1toP2();
				p1_opened_cnt++;
				p2_corr_flag = 1;
				if(p2_card_cnt == p2_opened_cnt) state = 31;
				else state = 10;
			}
			else{
				USART1_Transmit_String("Wrong\r\n");
				USART1_Transmit_String("Player 1's Turn\r\n");
				USART1_Transmit_String("-------------------------------------------\r\n");
				state = 3;
			}
			
		}
		if(state == 30){
			USART0_Transmit_String("******* P1 Win *******\r\n******* P2 Lose *******\r\n");
			USART1_Transmit_String("******* P1 Win *******\r\n******* P2 Lose *******\r\n");
			break;
		}
		if(state == 31){
			USART0_Transmit_String("******* P2 Win *******\r\n******* P2 Lose *******\r\n");
			USART1_Transmit_String("******* P2 Win *******\r\n******* P2 Lose *******\r\n");
			break;
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
		else if(USART_NUM == 1) USART1_Transmit_String("10");
	}
	else if(num == 11){
		if(USART_NUM == 0) USART0_Transmit_String("11");
		else if(USART_NUM == 1) USART1_Transmit_String("11");
	}
	else if(num == 12){
		if(USART_NUM == 0) USART0_Transmit_String("12");
		else if(USART_NUM == 1) USART1_Transmit_String("12");
	}
}

void showP2toP1 (void){
	USART0_Transmit_String("P2 Deck : "); // display P2 deck to P1
	for(int jj=0; jj<p2_card_cnt; jj++){
		if(p2_deck[jj].opened == 0) USART0_Transmit_String("XX");
		else{
			USART_Transmit_number(0, p2_deck[jj].color);
			USART_Transmit_number(0, p2_deck[jj].number);
		}
		USART0_Transmit_String("   ");
	}
	USART0_Transmit_String("\r\n");
}

void showP1toP2 (void){
	USART1_Transmit_String("P1 Deck : "); // display P1 deck to P2
	for(int jj=0; jj<p1_card_cnt; jj++){
		if(p1_deck[jj].opened == 0) USART1_Transmit_String("XX");
		else{
			USART_Transmit_number(1, p1_deck[jj].color);
			USART_Transmit_number(1, p1_deck[jj].number);
		}
		USART1_Transmit_String("   ");
	}
	USART1_Transmit_String("\r\n");
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

