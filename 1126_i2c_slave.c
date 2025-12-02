/*
 * _1126_slave.c
 *
 * Created: 2025-11-26 오후 1:35:46
 *  Author: mki30
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define F_CPU 16000000UL // 16MHz Clock Speed ⏱️
#define FOSC 16000000UL // 16MHz Clock Speed ⏱️

#define SLAVE_ADDR 0x20

volatile char twi_data = 0;

void TWI_Slave_Init(uint8_t addr){
	TWAR = (addr << 1);
	TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
}

ISR(TWI_vect){
	uint8_t twi_status = TWSR & 0xf8;
	switch (twi_status){
		case 0x60:
		case 0x68:
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;
		
		case 0x80:
		twi_data = TWDR;
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;
		
		case 0xa0:
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;
		
		case 0xa8:
		case 0xb0:
		TWDR = twi_data;
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;
		
		case 0xb8:
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE);
		break;
		
		case 0xc0:
		case 0xc8:
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;
		
		default:
		TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
		break;
	}
}

int main(){
	TWI_Slave_Init(SLAVE_ADDR);
	sei();
	
	while (1){
	}
}
