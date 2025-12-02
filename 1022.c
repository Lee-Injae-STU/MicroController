/*
 * MIDTERM.c
 *
 * Created: 2025-10-22 오후 6:11:23
 *  Author: mki30
 */

/*
sw1 -> pb0(pcint0)
sw2 -> pb1(pcint1)
sw3 -> pb2(pcint2)
sw4-> pd2(INT 0)
SW5 -> PD3(INT 1)
*/

#include <avr/io.h>
#define FOSC 16000000UL
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

#define SW1_EVENT 0x01 // -> START OR STOP
#define SW3_EVENT 0x03 // -> 양방향 SHIFT
#define SW4_EVENT 0x04
#define SW5_EVENT 0x05 // -> START OR STOP
#define SPECIAL 0x06

uint8_t status = 0;
volatile unsigned int period = 500;
volatile int start = 0;

void my_delay(int _ms){
	for(int i =0; i< _ms; i++){
		_delay_ms(1);
	}
}

//default
void LED_SHIFT(int t){
		for(int j =0; j<4; j++){
			PORTD = 0xf0;
			PORTC = ~(0x01 << j);
			my_delay(t);
		}
		for(int j =4; j<8; j++){
			PORTC = 0x0f;
			PORTD = ~(0x01 << j);
			my_delay(t);
		}
}

//sw3
void LED_SHIFT_SW3(int t){
		for(int j =0; j<4; j++){
			PORTD = 0xf0;
			PORTC = ~(0x01 << j);
			my_delay(t);
		}
		for(int j =4; j<8; j++){
			PORTC = 0x0f;
			PORTD = ~(0x01 << j);
			my_delay(t);
		for(int j =7; j>=4; j--){
			PORTC = 0x0f;
			PORTD = ~(0x01 << j);
			my_delay(t);
		}
		for(int j =3; j>=0; j--){
			PORTD = 0xf0;
			PORTC = ~(0x01 << j);
			my_delay(t);
		}
	
}
//sw4
#if 0
ISR(INT0_vect){ //외부인터럽트 0번의 ISR
	_delay_ms(15);
	if(PIND & 0x04) return;
	
	cli(); //clear interrupt
	EIFR |= 0x01; //interrupt flag set
	sei(); //set interrupt
	
	status = SW4_EVENT;
}
#endif


#if 0
//sw5
ISR(INT1_vect){
	_delay_ms(15);
	if(PIND & 0x08) return;
	
	cli();
	EIFR |= 0x02;
	sei();
	
	status = SW5_EVENT;
}
#endif

int main(){
	
	DDRD = (DDRD & 0x0f) | 0xf0; // 4-7출력
	DDRC = (DDRC & 0xf0) | 0x0f; // 0-3 출력
	DDRB &= (unsigned char)~0x07; // 0, 1, 2번 핀 입력
	
	PORTD = (PORTD & 0x0f) | 0xf0;
	PORTC = (PORTC & 0xf0) | 0x0f;
	

	// 외부 인터럽트 설정
	EICRA = 0x0a; // sense control(rising edge)
	EIMSK = 0x03; // 과거 플래그 클리어
	EIFR = 0x03; //INT0, INT1 허용
	
	_delay_ms(20);
	
	//PCI 설정(PORTB용)
	PCIFR |= 0x01;
	PCMSK0 |= 0x07;
	PCICR |= 0x01;
	
	_delay_ms(20);
		
	sei(); //전체 인터럽트 enable
	
	while(1){
		LED_SHIFT_SW3(period);
	}
}

