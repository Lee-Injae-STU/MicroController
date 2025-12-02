/*
 * _1111.c
 *
 * Created: 2025-11-11 오후 1:35:28
 *  Author: mki30
 */ 

//8bit timer normal mode
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

ISR(TIMER0_OVF_vect){
	Tov_val++;
	TCNT0 = 0x64; // =100
	
	if(Tov_val >= 100){
		//LED 500ms blink
		PORTC = 0xf0;
		PORTD = 0x0f;
		_delay_ms(500);
		PORTC = 0x0f;
		PORTD = 0xf0;
		//카운트 초기화
		Tov_val = 0;
	}
	sei();
}

int main(){
	cli();
	DDRD |= 0xf0; //PD 4 ~ 7 : LED 출력
	DDRC |= 0x0f; //PC 0 ~ 3 : LED 출력
	
	//LED는 일단 끈 상태로 시작
	PORTC |= 0x0f;
	PORTD |= 0xf0;
	
	//Timer0: 8bit + Normal(TOV) + prescaler=1024
	TCCR0A = 0x00;
	TCCR0B = (1<<CS02) | (1<<CS00); //1024분주
	TCNT0 = 100;// = 10ms 프리로드
	TIFR0 = (1 << TOV0); // 기존 플래그 클리어
	TIMSK0 = (1 << TOIE0); // TOV 인터럽트 허용
	
	sei();
	
	while(1){
	}
}
#endif

//8bit timer ctc mode
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

ISR(TIMER0_COMPA_vect){
	Tov_val++;
	
	if(Tov_val >= 100){
		//LED 500ms blink
		PORTC = 0xf0;
		PORTD = 0x0f;
		_delay_ms(500);
		PORTC = 0x0f;
		PORTD = 0xf0;
		//카운트 초기화
		Tov_val = 0;
	}
	sei();
}

int main(){
	cli();
	DDRD |= 0xf0; //PD 4 ~ 7 : LED 출력
	DDRC |= 0x0f; //PC 0 ~ 3 : LED 출력
	
	//LED는 일단 끈 상태로 시작
	PORTC |= 0x0f;
	PORTD |= 0xf0;
	
	//Timer0: 8bit + CTC + prescaler=1024
	TCCR0A = (1<<WGM01);
	TCCR0B = (1<<CS02) | (1<<CS00); //1024분주
	OCR0A = 155;
	TIFR0 = (1 << OCIE0A); // 기존 플래그 클리어
	TIMSK0 = (1 << OCF0A); // TOV 인터럽트 허용
	
	sei();
	
	while(1){
	}
}
#endif

//16bit timer normal mode
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

ISR(TIMER1_OVF_vect){
	Tov_val++;
	TCNT1 = 0xff64;
	
	if(Tov_val >= 100){
		//LED 500ms blink
		PORTC = 0xf0;
		PORTD = 0x0f;
		_delay_ms(500);
		PORTC = 0x0f;
		PORTD = 0xf0;
		//카운트 초기화
		Tov_val = 0;
	}
	sei();
} 1 <= 

int main(){
	cli();
	DDRD |= 0xf0; //PD 4 ~ 7 : LED 출력
	DDRC |= 0x0f; //PC 0 ~ 3 : LED 출력
	
	//LED는 일단 끈 상태로 시작
	PORTC |= 0x0f;
	PORTD |= 0xf0;
	
	//Timer0: 8bit + CTC + prescaler=1024
	TCCR1A = 0x00;
	TCCR1B = (1<<CS12) | (1<<CS10); //1024분주
	TCNT1 = 0xff64;
	TIFR1 = (1 << TOV1); // 기존 플래그 클리어
	TIMSK1 = (1 << TOIE1); // TOV 인터럽트 허용
	
	sei();
	
	while(1){
	}
}
#endif

//16bit timer CTC mode
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

ISR(TIMER1_COMPA_vect){
	Tov_val++;
	
	if(Tov_val >= 100){
		//LED 500ms blink
		PORTC = 0xf0;
		PORTD = 0x0f;
		_delay_ms(500);
		PORTC = 0x0f;
		PORTD = 0xf0;
		//카운트 초기화
		Tov_val = 0;
	}
	sei();
}

int main(){
	cli();
	DDRD |= 0xf0; //PD 4 ~ 7 : LED 출력
	DDRC |= 0x0f; //PC 0 ~ 3 : LED 출력
	
	//LED는 일단 끈 상태로 시작
	PORTC |= 0x0f;
	PORTD |= 0xf0;
	
	//Timer0: 8bit + CTC + prescaler=1024
	TCCR1A = 0x00;
	TCCR1B = (1<<WGM12)| (1<<CS12) | (1<<CS10);//256분주
	OCR1A = 155;
	TIFR1 = (1 << OCIE1A); // 기존 플래그 클리어
	TIMSK1 = (1 << OCF1A); // TOV 인터럽트 허용
	
	sei();
	
	while(1){
	}
}
#endif