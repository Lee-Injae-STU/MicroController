/*
 * _1125.c
 *
 * Created: 2025-11-25 오후 1:32:56
 *  Author: mki30
 */ 

//SPI 통신 - master
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#define F_CPU 16000000UL // 16MHz Clock Speed ⏱️
#define FOSC 16000000UL // 16MHz Clock Speed ⏱️

#define BAUD_RATE 103

volatile uint8_t uart_recv = 0;
volatile uint8_t uart_flag = 0;

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

void SPI_MasterInit(){
	DDRB |= (1 << PORTB3) | (1 << PORTB5) | (1 << PORTB2); //MOSI, SCK, SS 출력
	PORTB |= (1 << PORTB2); // SS HIGH -> Slave 비활성화
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);//Enable, Master, Fosc/16 
}

uint8_t SPI_MasterTransmit(uint8_t data){
	PORTB &= ~(1 << PORTB2); //SS LOW
	SPDR = data;
	while(!(SPSR & (1 << SPIF)));
	uint8_t recv = SPDR;
	PORTB |= (1 << PORTB2); // SS HIGH
	return recv;
}

ISR(USART_RX_vect){
	uart_recv = UDR0;
	uart_flag = 1;
}

int main(){
	//UART 설정
	UBRR0H = (unsigned char)(BAUD_RATE >> 8);
	UBRR0L = (unsigned char)(BAUD_RATE);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	SPI_MasterInit();
	sei();
	
	uint8_t value = 0;
	uint8_t reply = 0;
	
	while(1){
		if(uart_flag){
			uart_flag = 0;
			if(uart_recv >= '0' && uart_recv <= '9'){
				value = uart_recv - '0';
				Printf("\n[Master] Send value: %d\n", value);
					
				reply = SPI_MasterTransmit(value);
					
				_delay_ms(100);
				reply = SPI_MasterTransmit(value);
				
				if(reply == 'S'){
					Printf("[Master] Slave Response: SUCCESS\n");
				}
				else if(reply == 'F'){
					Printf("[Master] Slave Response: FAIL\n");
				}
				else{
					Printf("[Master] Unknown Response: %02X\n", reply);
				}		
			}
			else{
				Printf("\nCheck it is 0~9 Numbers\n");
			}
		}
	} 
}