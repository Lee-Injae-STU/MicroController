/*
 * GccApplication1.c
 *
 * Created: 2025-09-30 오후 1:35:12
 * Author : mki30
 */ 

#include <avr/io.h>
#define FOSC 16000000UL
#define F_CPU 16000000UL
#include <util/delay.h>

void my_delay(int _ms){
	for(int i =0; i< _ms; i++){
		_delay_ms(1);
	}
}

void LED_ALL_ON_OFF(int n, int t){
	for(int i = 0; i < n; i++){
		PORTD = 0x0f;
		PORTC = 0x0f;
		my_delay(t);
		PORTD = 0x00;
		PORTC = 0x00;
		my_delay(t);
	}
}

void LED_ALTERNATING(int d, int r, int t){
	for(int i = 0; i < r; i++){
		PORTD = d;
		PORTC = d>>4;
		my_delay(t);
		PORTD =~PORTD;
		PORTC = ~PORTC;
		my_delay(t);
	}
}

void LED_SHIFT(int n, int t){
	for(int i = 0; i < n; i++){
		for(int j = 0; j < 4; j++){
			PORTD = 0x0f;
			PORTC = ~(0x01 << j);
			my_delay(t);
		}
		for(int j = 0; j < 4; j++){
			PORTC = 0x0f;
			PORTD = ~(0x01 << j);
			my_delay(t);
		}
		for(int j = 0; j < 4; j++){
			PORTC = 0x0f;
			PORTD = ~(0x08 >> j);
			my_delay(t);
		}
		for(int j = 0; j < 4; j++){
			PORTD = 0x0f;
			PORTC = ~(0x08 >> j);
			my_delay(t);
		}
		my_delay(t);
	}
	
}

int main(void)
{	
	DDRD = 0x0f;
	DDRC = 0x0f;
	while(1){
		LED_ALL_ON_OFF(3, 500);
		LED_ALTERNATING(0xaa, 3, 200);
		LED_ALTERNATING(0x0f, 3, 200);
		LED_ALTERNATING(0xC3, 3, 200);
		LED_SHIFT(5, 50);
	}
}


