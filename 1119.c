/*
 * _1119.c
 *
 * Created: 2025-11-19 오후 1:36:20
 *  Author: mki30
 */

// 가변저항으로 ADC 실습
#if 0
#define F_CPU 16000000UL // 16MHz Clock Speed ⏱️
#define FOSC 16000000UL // 16MHz Clock Speed ⏱️

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//USART define
#define BAUD_RATE 103
#define MAX_TXBUF_SIZE 128
#define ON 1
#define OFF 0
#define PLANE 9

char tx_buf[MAX_TXBUF_SIZE]; //USART
volatile int buf_len = 0; //USART
volatile unsigned char send_char = 0; //USART
volatile unsigned char sint_flag = 0; //USART
//decode -> 0: NORMAL, 1: Command Reception, 2: Data Reception 3: Termination state
volatile int decode = 0; //USART
volatile uint8_t _command = 0; //USART
volatile int _data = 0; //USART
volatile unsigned char Tov_val = 0; //Timer
volatile unsigned char on_off = 0;

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

unsigned int GETADCDATA(unsigned char aIn){
	volatile unsigned int result;
	ADMUX = aIn;
	ADMUX |= (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADCSRA |= (1 << ADSC);
	while(!(ADCSRA & (1 << ADIF)));
	_delay_ms(300);
	result = ADCL + (ADCH << 8);
	ADCSRA = 0x00;
	return result;
}
int main(){
	UBRR0H = (unsigned char) (BAUD_RATE >> 8);
	UBRR0L = (unsigned char) (BAUD_RATE);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	while(1){
		_delay_ms(500);
		Printf("ADC : %d\n", GETADCDATA(5));		
	}
}
#endif

// ADC 결과 보기
#if 0
#define F_CPU 16000000UL // 16MHz Clock Speed ⏱️
#define FOSC 16000000UL // 16MHz Clock Speed ⏱️

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//USART define
#define BAUD_RATE 103
#define MAX_TXBUF_SIZE 128
#define ON 1
#define OFF 0
#define PLANE 9

char tx_buf[MAX_TXBUF_SIZE]; //USART
volatile int buf_len = 0; //USART
volatile unsigned char send_char = 0; //USART
volatile unsigned char sint_flag = 0; //USART
//decode -> 0: NORMAL, 1: Command Reception, 2: Data Reception 3: Termination state
volatile int decode = 0; //USART
volatile uint8_t _command = 0; //USART
volatile int _data = 0; //USART
volatile unsigned char Tov_val = 0; //Timer
volatile unsigned char on_off = 0;

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

unsigned int GETADCDATA(unsigned char aIn){
	volatile unsigned int result;
	ADMUX = aIn;
	ADMUX |= (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADCSRA |= (1 << ADSC);
	while(!(ADCSRA & (1 << ADIF)));
	_delay_ms(300);
	result = ADCL + (ADCH << 8);
	ADCSRA = 0x00;
	return result;
}

void disp_led(unsigned int _value){
	PORTC |= 0x0f;
	PORTD |= 0xf0;
	
	// 128 -> 1024 8등분하기
	// 그러면 1024를 9등분하면? -> 113.7... 올림 -> 114가 됨, 이렇게 해서 동작 1가지를 더 추가해줌
	for(int i = 0; i < (_value / 114); i++){
		if(i < 4){
			PORTC &= ~(0x01 << i);
		}
		else{
			PORTD &= ~(0x01 << i);
		}
	}
}

int main(){
	DDRD |= 0xf0;
	DDRC |= 0x0f;
	
	PORTC |= 0x0f;
	PORTD |= 0xf0;
	
	UBRR0H = (unsigned char) (BAUD_RATE >> 8);
	UBRR0L = (unsigned char) (BAUD_RATE);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	while(1){
		unsigned int adc_value = GETADCDATA(5);
		Printf("ADC : %d\n", adc_value);
		disp_led(adc_value);
		_delay_ms(100);
	}
}
#endif

// ADC 과제
#if 1
#define F_CPU 16000000UL // 16MHz Clock Speed ⏱️
#define FOSC 16000000UL // 16MHz Clock Speed ⏱️

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

//USART define
#define BAUD_RATE 103
#define MAX_TXBUF_SIZE 128
#define ON 1
#define OFF 0
#define PLANE 9

#define ODO 956
#define SI 1014
#define RA 1136
#define SOL 1277
#define FA 1433
#define ME 1517
#define RE 1704
#define ZDO 1912
#define FREQ(x) (unsigned int)(16000000/(2*8*(1+x)))

char tx_buf[MAX_TXBUF_SIZE]; //USART
volatile int buf_len = 0; //USART
volatile unsigned char send_char = 0; //USART
volatile unsigned char sint_flag = 0; //USART
//decode -> 0: NORMAL, 1: Command Reception, 2: Data Reception 3: Termination state
volatile int decode = 0; //USART
volatile uint8_t _command = 0; //USART
volatile int _data = 0; //USART
volatile unsigned char Tov_val = 0; //Timer
volatile unsigned char on_off = 0;
volatile unsigned int note_freq[8] ={ZDO, RE, ME, FA, SOL, RA, SI, ODO};
volatile unsigned int note = 7;
volatile unsigned int count = 0;

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

void play(unsigned int y){
	DDRB |= 0x02;
	TCCR1A = (1 << COM1A0);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << WGM11);
	ICR1 = y;
	OCR1A = 0;
}

void stop(){
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	TCNT1 = 0;
	ICR1 = 0;
	DDRB &= ~(0x02);
}

unsigned int GETADCDATA(unsigned char aIn){
	volatile unsigned int result;
	ADMUX = aIn;
	ADMUX |= (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADCSRA |= (1 << ADSC);
	while(!(ADCSRA & (1 << ADIF)));
	_delay_ms(300);
	result = ADCL + (ADCH << 8);
	ADCSRA = 0x00;
	return result;
}

void disp_led(unsigned int _value){
	PORTC |= 0x0f;
	PORTD |= 0xf0;
	
	// 128 -> 1024 8등분하기
	// 그러면 1024를 9등분하면? -> 113.7... 올림 -> 114가 됨, 이렇게 해서 동작 1가지를 더 추가해줌
	for(int i = 0; i <= (_value / 128); i++){
		if(i < 4){
			PORTC &= ~(0x01 << i);
		}
		else{
			PORTD &= ~(0x01 << i);
		}
	}
}

void CONTROL_NOTE(unsigned int _value){
	for(int i = 0; i <= (_value / 128); i++){
		play(note_freq[i]);
	}
}

ISR(INT0_vect){
	cli();
	_delay_ms(20);
	if((PIND & 0x04) == 0){
		if((count % 2) == 0){
			PORTD = 0x0f;
			PORTC = 0xf0;
			Printf("Play The Buzzer\n");
			on_off = ON;
			count++;
		}
		else{
			PORTD = 0xf0;
			PORTC = 0x0f;
			Printf("Stop The Buzzer\n");
			on_off = OFF;
			count = 0;
		}
	}
	EIFR |= 0x01;
	sei();
}

int main(){
	cli();
	
	DDRD = 0xf0;
	DDRC = 0x0f;
	
	PORTD = 0xf0;
	PORTC = 0x0f;
	
	EICRA = 0x0A;
	EIFR = 0x01;
	EIMSK = 0x03;
	
	UBRR0H = (unsigned char) (BAUD_RATE >> 8);
	UBRR0L = (unsigned char) (BAUD_RATE);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	sei();
	
	while(1){
		unsigned int adc_value = GETADCDATA(5);
		
		if(on_off == ON){
		CONTROL_NOTE(adc_value);
		disp_led(adc_value);
		_delay_ms(100);
		}
		
		else{
			stop();
		}
	}
}
#endif