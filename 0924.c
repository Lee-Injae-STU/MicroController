/*
 * GccApplication1.c
 *
 * Created: 2025-09-24 오후 1:36:07
 * Author : mki30
 */ 

#include <avr/io.h>
#define FOSC 16000000UL
#define F_CPU 16000000UL
#include <util/delay.h>

//delay 함수
void my_delay(int _ms){
	for(int  i= 0; i< _ms; i++){
		_delay_ms(1);
	}
}


int main(void)
{
	DDRD = 0xff;
	
	while(1){
		for(int i = 0; i<; i++){
			PORTD = 0xff;
			my_delay();
			PORTD = 0x00;
			my_delay();	
		}
	}

#if 0
void led_alternating_onoff(int d, int r, int p){
	//d = display 초기값
	//r = 반복횟수
	DDRD = 0xff;
	//p = 주기
	for(int i = 0; i < r; i++){
		PORTD = d;
		my_delay(p);
		PORTD = ~d;
		my_delay(p);
	}
	
}


int main(){
	
	while(1){
		led_alternating_onoff(0xaa, 3, 200);
		led_alternating_onoff(0xf0, 4, 300);
		led_alternating_onoff(0x3c, 5, 400);
	}
}
#endif

#if 0
int main(){
	DDRD = 0xff;
	
	while(1){
		
		// 3번 교대로
		for(int i =0; i<3; i++){
				PORTD = 0xaa;
				_delay_ms(500);
				PORTD = 0x55;
				_delay_ms(500);	
		}
		//4번 상위 하위 번갈아서
		for(int i=0; i<4;i++){
				PORTD = 0xf0;
				_delay_ms(500);
				PORTD = 0x0f;
				_delay_ms(500);
		}
		//5번 끝 중앙 번갈아서
		for(int i=0; i<5;i++){
				PORTD = 0x3c;
				_delay_ms(500);
				PORTD = 0xc3;
				_delay_ms(500);
		}
		
	}
}
#endif

#if 0
int main(){
	DDRD = 0xff;
	
	while(1){
		for(int i=0; i<8; i++){
			PORTD = ~(0x80 >> i);
			_delay_ms(100);
		}		
	}
}
#endif