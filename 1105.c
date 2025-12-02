/*
 * _1105.c
 *
 * Created: 2025-11-05 오후 1:36:31
 *  Author: mki30
 */ 

//프로토콜
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
#define LED_ONOFF 0x01
#define LED_ALT 0x02
#define LED_SHIFT 0x04
#define STX 0x02
#define ETX 0x03

char tx_buf[MAX_TXBUF_SIZE];
volatile int buf_len = 0;
volatile unsigned char send_char = 0;
volatile unsigned char sint_flag = 0;
//decode -> 0: NORMAL, 1: Data Reception, 2: Terminationstate
volatile int decode = 0;
volatile uint8_t _command = 0;

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
		my_delay(500);
		PORTD =~PORTD;
		PORTC = ~PORTC;
		my_delay(500);
	}

}

void led_shift(int n){
	for(int i =0; i<n; i++){
		for(int i =0; i<8; i++){
			PORTC = ~(0x01 << i);
			PORTD = ~(0x01 << i);
			my_delay(300);
		}
	}
}

void hex_command(uint8_t cd){
	if(cd == LED_ONOFF){
		led_all_onoff(3);
	}
	else if(cd == LED_ALT){
		led_alternating(0xaa, 3);
	}
	else if(cd == LED_SHIFT){
		led_shift(3);
	}
	else{
		Printf("Invalid Command\n");
	}
}

ISR(USART_RX_vect){
	cli();
	unsigned char _usart_rcv = 0;
	_usart_rcv = UDR0;
	if(decode == 0 && _usart_rcv == STX){
		decode = 1; // change state decode into 1
	}
	else if(decode == 1){
		_command = _usart_rcv;
		decode = 2; // change state decode into 2
	}
	else if(decode == 2){
		if(_usart_rcv == ETX){
			sint_flag = 1;
		}
		else{
			Printf("RX error\n");
		}
		decode = 0; // return to normal state
	}
	sei();
}


int main(){
	cli();
	//RX는 입력, TX는 출력으로 설정
	DDRD |= 0xf2;
	DDRD &= ~(0x0d);
	DDRC |= 0x0f;
	//LED는 일단 끈 상태로 시작
	PORTC |= 0x0f;
	PORTD |= 0xf0;
	//외부 인터럽트 설정
	EICRA = 0x0a;
	EIFR = 0x03;
	EIMSK = 0x03;
	//UART 설정
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
			hex_command(_command);
			sint_flag = 0;
			_command = 0;
			//기본 커멘드 후 LED모두 종료
			PORTC = 0x0f;
			PORTD = 0xf0;
		}
	}
}
#endif

//프로토콜 과제
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
#define LED_ONOFF 0x01
#define LED_ALT 0x02
#define LED_SHIFT 0x04
#define STX 0x02
#define ETX 0x03

char tx_buf[MAX_TXBUF_SIZE];
volatile int buf_len = 0;
volatile unsigned char send_char = 0;
volatile unsigned char sint_flag = 0;
//decode -> 0: NORMAL, 1: Command Reception, 2: Data Reception 3: Termination state
volatile int decode = 0;
volatile uint8_t _command = 0;
volatile int _data = 0;

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
		my_delay(500);
		PORTD =~PORTD;
		PORTC = ~PORTC;
		my_delay(500);
	}

}

void led_shift(int n){
	for(int i =0; i<n; i++){
		for(int i =0; i<8; i++){
			PORTC = ~(0x01 << i);
			PORTD = ~(0x01 << i);
			my_delay(300);
		}
	}
}

void hex_command(uint8_t cd){
	if(cd == LED_ONOFF){
		led_all_onoff(_data);
	}
	else if(cd == LED_ALT){
		led_alternating(_data, 3);
	}
	else if(cd == LED_SHIFT){
		led_shift(_data);
	}
	else{
		Printf("Invalid Command\n");
	}
}

ISR(USART_RX_vect){
	cli();
	unsigned char _usart_rcv = 0;
	_usart_rcv = UDR0;
	if(decode == 0 && _usart_rcv == STX){
		decode = 1; // change state decode into 1
	}
	else if(decode == 1){
		_command = _usart_rcv;
		decode = 2; // change state decode into 2
	}
	else if(decode == 2){
		_data = _usart_rcv;
		decode = 3;
	}
	else if(decode == 3){
		if(_usart_rcv == ETX){
			sint_flag = 1;
		}
		else{
			Printf("RX error\n");
		}
		decode = 0; // return to normal state
	}
	sei();
}


int main(){
	cli();
	//RX는 입력, TX는 출력으로 설정
	DDRD |= 0xf2;
	DDRD &= ~(0x0d);
	DDRC |= 0x0f;
	//LED는 일단 끈 상태로 시작
	PORTC |= 0x0f;
	PORTD |= 0xf0;
	//외부 인터럽트 설정
	EICRA = 0x0a;
	EIFR = 0x03;
	EIMSK = 0x03;
	//UART 설정
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
			hex_command(_command);
			sint_flag = 0;
			_command = 0;
			_data = 0;
			//기본 커멘드 후 LED모두 종료
			PORTC = 0x0f;
			PORTD = 0xf0;
		}
	}
}
#endif