/*
 * _1126.c
 *
 * Created: 2025-11-26 오후 1:35:16
 *  Author: mki30
 */ 

// I2C통신 - master
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#define F_CPU 16000000UL // 16MHz Clock Speed ⏱️
#define FOSC 16000000UL // 16MHz Clock Speed ⏱️

#define BAUD_RATE 103
#define SLAVE_ADDR 0x20

//I2C 통신상태 확인 매크로 (일부 필수 상태만 확인)
#define START_ACK 0x08
#define REP_START_ACK 0x10
#define MT_SLA_ACK 0x18
#define MT_DATA_ACK 0x28
#define MR_SLA_ACK 0x40
#define MR_DATA_NACK 0x58

void UART_Init(){
	UBRR0H = 0;
	UBRR0L = BAUD_RATE;
	UCSR0A = 0x00;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

char rx_char(){
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

void tx_char(unsigned char txChar){
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = txChar;
}

void Printf(char *fmt, ...){
	va_list ap;
	char buf[128];
	memset(buf, 0, 128);
	
	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);
	
	for(uint8_t i = 0; buf[i] != '\0'; i++){
		tx_char(buf[i]);
	}
}

void TWI_Master_Init(){
	TWBR = 72;
	TWSR = 0x00;
}

uint8_t TWI_Start(){
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT)));
	return(TWSR & 0xF8);
}

void TWI_Stop(){
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

uint8_t TWI_Write_Address(uint8_t address){
	TWDR = address;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR &(1 << TWINT)));
	return(TWSR & 0xF8);
}

uint8_t TWI_Write_Data(uint8_t data){
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT)));
	return (TWSR & 0xF8);
}

uint8_t TWI_Read_Data_ACK(){
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	while(!(TWCR & (1 << TWINT)));
	return TWDR;
}

uint8_t TWI_Read_Data_NACK(){
	TWCR = (1 << TWINT) | (1 << TWEN);
	while(!(TWCR & (1 << TWINT)));
	return TWDR;
}

int main(){
	char tx_data, rx_data;
	UART_Init();
	TWI_Master_Init();
	sei();
	
	Printf("ATMega328P Master I2C Loopback Test Start\n");
	Printf("Slave Address: 0x%X\r\n", SLAVE_ADDR);
	
	while(1){
		Printf("\r\n>> Enter 1 char (will be sent to slave): ");
		tx_data = rx_char();
		Printf("Input DAta: %c\r\n", tx_data);
		
		if(TWI_Start() == START_ACK){
			if(TWI_Write_Address((SLAVE_ADDR << 1) | 0x00) == MT_SLA_ACK){
				if(TWI_Write_Data(tx_data) == MT_DATA_ACK){
					Printf("  - Data sent: %c\r\n", tx_data);
				}
			}
		}
		
		TWI_Stop();
		_delay_ms(10);
		
		if(TWI_Start() == START_ACK){
			if(TWI_Write_Address((SLAVE_ADDR << 1) | 0x01) == MR_SLA_ACK){
				rx_data = TWI_Read_Data_NACK();
				if((TWSR & 0xF8) == MR_DATA_NACK){
					Printf("  - Data recieved from slave: %c\r\n", rx_data);
				}
			}
		}
		
		TWI_Stop();
		
		if(tx_data == rx_data){
			Printf("LOOPBACK SUCCESS\r\n");
		}
		else{
			Printf("LOOPBACK FAIL! (Sent: %c, Received: %c)\r\n", tx_data, rx_data);
		}
	}
}