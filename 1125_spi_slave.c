/*
 * _1125_slave.c
 *
 * Created: 2025-11-25 오후 1:56:47
 *  Author: mki30
 */ 

// SPI통신 - slave
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 16000000UL // 16MHz Clock Speed ⏱️
#define FOSC 16000000UL // 16MHz Clock Speed ⏱️

volatile uint8_t spi_recv_data = 0;
volatile uint8_t led_len = 0;
volatile uint8_t sorf = 0;
volatile uint8_t temp = 0;

void SPI_SlaveInit(){
	DDRB |= (1 << PORTB4);
	DDRB &= (1 << PORTB2);
	PORTB &= ~(1 << PORTB2);
	
	SPCR = (1 << SPE) | (1 << SPIE);
	sei();
	SPDR = 0x4E;
}

void disp_led(unsigned int _value){
	PORTC |= 0x0f;
	PORTD |= 0xf0;
	for(int i = 0; i < _value; i++){
		if(i < 4){
			PORTC &= ~(0x01 << i);
		}
		else{
			PORTD &= ~(0x01 << i);
		}
	}
}

ISR(SPI_STC_vect){
	spi_recv_data = SPDR;
	
	if(spi_recv_data <= 8){
		led_len = spi_recv_data;
		sorf = 'S';
	}
	else{
		sorf = 'F';
	}
	
	SPDR = sorf;
}

int main(){
	DDRD |= 0xf0;
	DDRC |= 0x0f;
	
	PORTC |= 0x0f;
	PORTD |= 0xf0;
	
	SPI_SlaveInit();
	
	while(1){
		disp_led(led_len);
	}
}