/*
 * _1112.c
 *
 * Created: 2025-11-12 오후 1:33:41
 *  Author: mki30
 */

// buzzer testing
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
#define LED_ONOFF 0x01
#define LED_ALT 0x02
#define LED_SHIFT 0x04
#define STX 0x02
#define ETX 0x03

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

ISR(INT0_vect){
	cli();
	_delay_ms(20);
	if((PIND & 0x04) == 0){
		on_off = ON;
		Printf("Play Button Pressed\n");
	}
	EIFR |= 0x01;
	sei();
}

ISR(INT1_vect){
	cli();
	_delay_ms(20);
	if((PIND & 0x08) == 0){
		on_off = OFF;
		Printf("Stop Button Pressed\n");
	}
	EIFR |= 0x02;
	sei();
}

int main(){
	cli();
	
	EICRA = 0x0A;
	EIFR = 0x03;
	EIMSK = 0x03;
	
	UBRR0H = (unsigned char) (BAUD_RATE >> 8);
	UBRR0L = (unsigned char) (BAUD_RATE);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	sei();
	
	while(1){
		if(on_off == ON){
			play(529);
		}
		else{
			stop();
		}
	}
}
#endif

//buzzer lab -> rasing a note step by step from pushing a switch
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

//미레 도레 미미미 레레레 미미미
#define ODO 956
#define RE 1014
#define ME 1136
#define FA 1277
#define SOL 1433
#define RA 1517
#define SI 1704
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
volatile unsigned int note_freq[8] ={ODO, RE, ME, FA, SOL, RA, SI, ZDO};
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

ISR(INT0_vect){
	cli();
	_delay_ms(20);
	if((PIND & 0x04) == 0){
		if(note <= 0){
			note = 7;
		}
		note--;
		//Printf("Decrease one note\n");

	}
	EIFR |= 0x01;
	sei();
}

ISR(INT1_vect){
	cli();
	_delay_ms(20);
	if((PIND & 0x08) == 0){
		note++;
		//Printf("Increase one note\n");
		if(note > 7){
			note = 0;
		}
	}
	EIFR |= 0x02;
	sei();
}

ISR(PCINT0_vect){
	unsigned char now_b;
	now_b = PINB;
	_delay_ms(15);
	
	if((now_b & 0x01) == 0){
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
}

int main(){
	cli();
	
	DDRD |= 0xf0;
	DDRC |= 0x0f;
	
	PORTD |= 0x0f;
	PORTC |= 0xf0;
	
	EICRA = 0x0A;
	EIFR = 0x03;
	EIMSK = 0x03;
	
	PCIFR |= 0x01;
	PCMSK0 |= 0x01;
	PCICR |= 0x01;
	
	UBRR0H = (unsigned char) (BAUD_RATE >> 8);
	UBRR0L = (unsigned char) (BAUD_RATE);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	sei();
	
	while(1){
		if(on_off == ON){
			play(note_freq[note]);
		}
		else{
			stop();
		}
		
		
	}
}
#endif