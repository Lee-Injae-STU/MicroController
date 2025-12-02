/*
 * _1028.c
 *
 * Created: 2025-10-28 오후 1:33:34
 *  Author: mki30
 */ 

//1028 uart통신 연습(rx)
#if 0
#define F_CPU 16000000UL // 16MHz Clock Speed ⏱️
#define FOSC 16000000UL // 16MHz Clock Speed ⏱️

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define BAUD_RATE 103

volatile unsigned char send_char = 0;

ISR(INT0_vect){
	cli();
	
	_delay_ms(20);
	if((PIND & 0x04) == 0){
		send_char = 'A';
	}
	
	EIFR |= 0x01;
	sei();
}

ISR(INT1_vect){
	cli();
	
	_delay_ms(20);
	if((PIND & 0x08) == 0){
		send_char = 'B';
	}
	
	EIFR |= 0x02;
	sei();
}

int main(){
	cli();
	
	DDRD |= 0x02;
	DDRD &= ~(0x0D);
	
	EICRA = 0x0a;
	EIFR = 0x03;
	EIMSK = 0x03;
	
	UBRR0H = (unsigned char) (BAUD_RATE >> 8);
	UBRR0L = (unsigned char) (BAUD_RATE);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	sei();
	
	while(1){
		if(send_char){
			while(!(UCSR0A & (1 << UDRE0)));
			UDR0 = send_char;
			send_char = 0;
		}
	}
}
#endif

//1029 uart 통신연습(양방향 통신 LOOP BACK)
#if 1

#define F_CPU 16000000UL // 16MHz Clock Speed ⏱️
#define FOSC 16000000UL // 16MHz Clock Speed ⏱️

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BAUD_RATE 103

volatile unsigned char send_char = 0;


int main(){
	cli();
	
	DDRD |= 0x02;
	DDRC &= ~(0x0D);
	
	EICRA = 0x0a;
	EIFR = 0x03;
	EIMSK = 0x03;
	
	UBRR0H = (unsigned char) (BAUD_RATE >> 8);
	UBRR0L = (unsigned char) (BAUD_RATE);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	unsigned char rx_char = 0;
	sei();
	
	while(1){
		//rx blocking
		while(!(UCSR0A & (1 << RXC0)));
		rx_char = UDR0;
		
		//tx blocking
		while(!(UCSR0A & (1 << UDRE0)));
		UDR0 = rx_char;
	}
}
#endif


//1029 uart 통신연습 string 출력
#if 0

#define F_CPU 16000000UL // 16MHz Clock Speed ⏱️
#define FOSC 16000000UL // 16MHz Clock Speed ⏱️

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define BAUD_RATE 103
#define MAX_TXBUF_SIZE 128

char tx_buf[MAX_TXBUF_SIZE];
volatile int buf_len = 0;
volatile unsigned char send_char = 0;

void tx_char(unsigned char txChar){
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = txChar;
}

void tx_str(char *txStr, int len){
	for(int i = 0; i < len;  i++){
		tx_char(txStr[i]);
	}
}

// 나만의 printf만들기
void Printf(char *fmt, ...){
	va_list arg_ptr;
	uint8_t i, len;
	char sText[128];
	
	//Text Buffer
	for( i= 0; i<128; i++){
		sText[i] = 0;
	}
	va_start(arg_ptr, fmt);
	vsprintf(sText, fmt, arg_ptr);
	va_end(arg_ptr);
	
	len = strlen(sText);
	for( i =0; i<len; i++){
		tx_char(sText[i]);
	}
}

ISR(INT0_vect){
	cli();
	
	_delay_ms(20);
	if((PIND & 0x04) == 0){
		char *tmp;
		tmp = malloc(30);
		memset(tmp, '\0', 30);
		tmp = "INT0 PUSHED\n";
		buf_len = strlen(tmp);
		strncpy(tx_buf, tmp, buf_len);
	}
	
	EIFR |= 0x01;
	sei();
}

ISR(INT1_vect){
	cli();
	
	_delay_ms(20);
	if((PIND & 0x08) == 0){
		char *tmp;
		tmp = malloc(30);
		memset(tmp, '\0', 30);
		tmp = "INT1 PUSHED\n";
		buf_len = strlen(tmp);
		strncpy(tx_buf, tmp, buf_len);
		
		for(int i =0; i<128; i++){
			Printf("*");
		}
		Printf("\n");
		Printf("\n");
		Printf("Clock Speed = %lu \n", F_CPU);
		Printf("Interrupts = 0x%02x", EIMSK);
		Printf("\n");
		Printf("\n");
		for(int i =0; i<128; i++){
			Printf("*");
		}
	}
	
	EIFR |= 0x02;
	sei();
}


int main(){
	cli();
	
	DDRD |= 0x02;
	DDRC &= ~(0x0D);
	
	EICRA = 0x0a;
	EIFR = 0x03;
	EIMSK = 0x03;
	
	UBRR0H = (unsigned char) (BAUD_RATE >> 8);
	UBRR0L = (unsigned char) (BAUD_RATE);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	sei();
		
	while(1){
		if(buf_len > 0){
			tx_str(tx_buf, buf_len);
			memset(tx_buf, '\0', MAX_TXBUF_SIZE);
			buf_len = 0;
		}
	}
}
#endif