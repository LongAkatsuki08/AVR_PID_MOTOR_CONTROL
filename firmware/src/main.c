// Output Port pin LED_O
#define PORT_LED_O      PORTB
#define DDR_LED_O       DDRB
#define BIT_LED_O       6
// Output Port pin Buzzer
#define PORT_BUZ       PORTB
#define DDR_BUZ        DDRB
#define BIT_BUZ        7


#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>

#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/iom128.h>

#include "myLCD.h"
#define USART0_BAUD         115200ul
#define USART0_UBBR_VALUE   ((F_CPU/(USART0_BAUD<<4))-1)


void init_IO()
{
    DDR_LED_O |= (1<<BIT_LED_O);
    DDR_BUZ |= (1 << BIT_BUZ);
}


void USART0_vInit(void)
{
    // Set baud rate
    UBRR0H = (uint8_t)(USART0_UBBR_VALUE>>8);
    UBRR0L = (uint8_t)USART0_UBBR_VALUE;

    // Set frame format to 8 data bits, no parity, 1 stop bit
    UCSR0C = (0<<USBS)|(1<<UCSZ1)|(1<<UCSZ0);

    // Enable receiver and transmitter
    UCSR0B = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE0);
    sei();
}


void USART0_vSendByte(uint8_t u8Data1)
{
    // Wait if a byte is being transmitted
    while((UCSR0A&(1<<UDRE0)) == 0) //
    {
        ;
    }

    // Transmit data
    UDR0 = u8Data1;
}

uint8_t USART0_vReceiveByte()
{
    // Wait until a byte has been received
    while((UCSR0A&(1<<RXC0)) == 0)
    {
    	;
    }

    // Return received data
    return UDR0;
}
uint8_t u8Data;
int main(void)
{


    // Initialise USART
    USART0_vInit();
    init_IO();
    init_LCD();
    // Send string
    USART0_vSendByte('A');
    USART0_vSendByte('V');
    USART0_vSendByte('R');
    USART0_vSendByte('\r');
    USART0_vSendByte('\n');

    // Repeat indefinitely
    for(;;)
    {
    	PORT_LED_O ^= (1<<BIT_LED_O);
    		_delay_ms(500);
    }
}

ISR(USART0_RX_vect){

	// Echo received characters
	        u8Data = USART0_vReceiveByte();
	        if(u8Data == 'b')
	        {
	           	clr_LCD();
	            move_LCD(1,1);
	            printf_LCD("Recieved Data:%c",u8Data);
	            PORT_BUZ ^=(1<<BIT_BUZ);
	        }

	        USART0_vSendByte(u8Data);
}
