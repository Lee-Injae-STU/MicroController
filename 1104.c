/*
 * _1104.c
 *
 * Created: 2025-11-04 오후 1:27:29
 *  Author: mki30
 */ 

// RX 인터럽트 -> 루프백 테스트
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

ISR(USART_RX_vect){
	char _uart_rcv = 0;
	_uart_rcv = UDR0;
	tx_char(_uart_rcv);
	sei();
}


int main(){
	cli();
	
	DDRD |= 0x02;
	DDRC &= ~(0x0D);
	
	EICRA = 0x0a;
	EIFR = 0x03;
	EIMSK = 0x03;
	
	UBRR0H = (unsigned char) (BAUD_RATE >> 8); //Baud Rate Setting
	UBRR0L = (unsigned char) (BAUD_RATE);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0); // RX, TX Enable
	UCSR0B |= (1 << RXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Character Size: 8-bit setting
	
	unsigned char rx_char = 0;
	sei();
	
	while(1){
	}
}
#endif

// TX로 보드 제어
#if 1
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
volatile unsigned char sint_flag = 0;

void my_delay(int _ms){
	for(int i =0; i< _ms; i++){
		_delay_ms(1);
	}
}

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

void led_all_onoff(int n){
	for(int i = 0; i < n; i++){
		PORTD = 0x0f;
		PORTC = 0xf0;
		my_delay(500);
		PORTD = 0xf0;
		PORTC = 0x0f;
		my_delay(500);
	}
}

void led_alternating(int d, int n){
	for(int i = 0; i < n; i++){
		PORTD = d;
		PORTC = d>>4;
		my_delay(300);
		PORTD =~PORTD;
		PORTC = ~PORTC;
		my_delay(300);
	}

}

void led_shift(int n){
	for(int i =0; i<n; i++){
		for(int i =0; i<8; i++){
			PORTC = ~(0x01 << i);
			PORTD = ~(0x01 << i);
			my_delay(100);
		}
	}
}

void command(char *buff, uint8_t len){
	char tmp[MAX_TXBUF_SIZE];
	memset(tmp, '\0', MAX_TXBUF_SIZE);
	memcpy((char*)tmp, (char*)buff, len);
	if(strstr((char*)tmp, "LED_ON_OFF") != NULL){
		tx_str(tx_buf, buf_len);
		led_all_onoff(3);
	}
	else if(strstr((char*)tmp, "LED_ALT") != NULL){
		tx_str(tx_buf, buf_len);
		led_alternating(0xaa, 3);
	}
	else if(strstr((char*)tmp, "LED_SHIFT") != NULL){
		tx_str(tx_buf, buf_len);
		led_shift(3);
	}
}

ISR(USART_RX_vect){
	cli();
	unsigned char _usart_rcv = 0;
	_usart_rcv = UDR0;
	if(_usart_rcv == '\n' || buf_len >= (MAX_TXBUF_SIZE - 1)){
		tx_buf[buf_len++] = _usart_rcv;
		sint_flag = 1;
	}
	else{
		tx_buf[buf_len++] = _usart_rcv;
	}
	sei();
}


int main(){
	cli();
	
	DDRD |= 0xf2;
	DDRD &= ~(0x0d);
	DDRC |= 0x0f;
	
	PORTC |= 0x0f;
	PORTD |= 0xf0;
	
	EICRA = 0x0a;
	EIFR = 0x03;
	EIMSK = 0x03;
	
	UBRR0H = (unsigned char) (BAUD_RATE >> 8);
	UBRR0L = (unsigned char) (BAUD_RATE);
	UCSR0A = 0x00;
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0B |= (1 << RXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	sei();
	my_delay(100);
	
	while(1){
		if(sint_flag){
			tx_buf[buf_len] = '\0';
			
			command(tx_buf, buf_len);
			buf_len = 0;
			memset(tx_buf, '\0', MAX_TXBUF_SIZE);
			sint_flag = 0;
			PORTC |= 0x0f;
			PORTD |= 0xf0;
		}
	}
}
#endif

