/**
 * @file	uart.c
 * @Author	SpeedFight (speedfight_2@wp.pl)
 * @date	13.08.16
 */

#include "../inc/uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/**
 * To use it:
 * 1. set baud rate
 * 2. set buffer size
 * 3. set TX/RX PIN/PORT
 */

/**
 * @addtogroup uart_definitions
 * @{
 */
#define BAUD_UART 19200u		//4Mhz -> 19200 -> OK
#define UBRR_value (F_CPU/16/BAUD_UART-1)

#define TIMER_COMPARE_VALUE	(255-1) //(1024/F_CPU)*10 = 0.05s (for ~1Mhz)
					 //(clk_div/F_CPU)*mulipy_by
					 //set value 0-255
					 //it describe time after lock input buffer
					 //e.g. max quiet time without no new byte in input
					 //after this time, last char in input array become '\0'

#define BUFFER_SIZE 200u		//input buffer size
//BUG -> cant receive more than ~910 bytes


//led for signalization
#define TX_PIN	4
#define TX_PORT	D
#define RX_PIN	3
#define RX_PORT	D

//DON'T CHANGE CODE BELLOW!!!//
//merge macros
#define _LED_PIN(a)	PIN	## a
#define _LED_PORT(a)	PORT	## a
#define _LED_DDR(a)	DDR	## a

//necessary addition level of abstaraction
//_LED_PIN(a) -> PINa
//If you write just _LED_PIN([another define]) -> _LED_PIN[another define]
#define LED_PIN(a) 	_LED_PIN(a)
#define LED_PORT(a)	_LED_PORT(a)
#define LED_DDR(a) 	_LED_DDR(a)

//led on/off macros
#define TX_LED_ON	LED_PORT(TX_PORT) &= ~(1<<LED_PIN(TX_PIN));
#define TX_LED_OFF	LED_PORT(TX_PORT) |= (1<<LED_PIN(TX_PIN));
#define RX_LED_ON	LED_PORT(RX_PORT) &= ~(1<<LED_PIN(RX_PIN));
#define RX_LED_OFF	LED_PORT(RX_PORT) |= (1<<LED_PIN(RX_PIN));

#define NULL '\0'


#define MANUAL	0
#define AUTO	1

///@}

static char uart_receive_data[BUFFER_SIZE];

static volatile uint16_t element;
static volatile uint8_t uart_data_pack_received;
static uint8_t receive_mode=MANUAL;

static volatile uint8_t i;	//volatile to calculate multiply 8bit timer time;
#define TIMER 10	//multiple time after last received data
/*
	(1/(4e6/1024))*255*10 =
 */

/**
 * @brief Inicjalizacja modułu uart
 */
static void init_uart(void)
{
	LED_DDR(TX_PORT) |=(1<<LED_PIN(TX_PIN));	//set led pins as output
	LED_DDR(RX_PORT) |=(1<<LED_PIN(RX_PIN));

	TX_LED_OFF;
	RX_LED_OFF;

	UBRRL =(uint8_t) UBRR_value;		//set baud rate
	UBRRH =(uint8_t)(UBRR_value>>8);

	UCSRB = (1<<RXEN) | (1<<TXEN) | (1<<RXCIE);	//enable TX,RX and TX IRQ
	UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0) ;	//8 bit data frame, no parity check, 1bit stop and start

	//8bit timer0 configuration
	TCCR0 |=(1<<CS02) | (0<<CS01) | (1<<CS00); //clock/1024 prescaler

	sei(); //enale IRQ
	TCNT0 = (uint8_t)0; //Timer0 counter register value
}

/**
 * @brief Wysłanie cstringa uartem
 */
static void send(char *in_message)
{
	if(*in_message)//check first element
	{
		char *message=(char *)in_message;
		TX_LED_ON;
		do
		{

			while (!( UCSRA & (1<<UDRE)));	//Wait to send previous data
			//only then you can write/read to UDR


			//while(!(uart_data_pack_received));

			UDR = *message;			//write data to output buffer

		}while(*(++message));			//end when you meet cstring end ('\0')

		TX_LED_OFF;
	}

}

static void input_buffer_pointer_to_beginning()
{
	element=0;
}

static void set_null_to_begin()
{
	uart_receive_data[0]=NULL;
	for(uint16_t i=0;i<(BUFFER_SIZE-1);i++){
		uart_receive_data[i]=NULL;
	}
}

/**
 * @brief Receive uart data byte interrupt routine
 * @detals
 */
ISR(USART_RXC_vect)
{
	RX_LED_ON;

	uart_receive_data[element++]=UDR;	//put input data to input array
	uart_receive_data[element]=NULL;
	if(element>BUFFER_SIZE-1) element=0;

	if(receive_mode==MANUAL)
	{
		uart_data_pack_received=1;

		RX_LED_OFF;
	}
	else	//AUTO
	{
		uart_data_pack_received=0;
		i=0;
		//TIFR |=(0<<TOV0);	//clear overflow flag
		TIMSK |=(1<<TOIE0);  	//enable timer0 overflow IRQ
		TCNT0 = (uint8_t)TIMER_COMPARE_VALUE; //Timer0 counter register value
	}
}

/**
 * @brief timer irq routine
 * @detils Interrupt after last receive byte from uart, set
 *         last iput array element as \0
 */
ISR(TIMER0_OVF_vect)
{
	RX_LED_OFF;
	uart_receive_data[element]=NULL;	//set end of input data array

	if(++i>TIMER)
	{

		uart_data_pack_received=1;	//Input data ready to read!
		element=0;	//clear
		i=0;
		TIMSK &=~(1<<TOIE0); //disable timer0 overflow IRQ
		TCNT0 = (uint8_t)0; //Timer0 counter register value
		RX_LED_OFF;
	}
}
/**
 * @brief Initialize pointers to uart struct
 */
void uart_init_struct(comm_typedef *uart)
{
	uart->init=&init_uart;
	uart->send=&send;
	uart->received=uart_receive_data;
	uart->received_data_pack_flag=&uart_data_pack_received;
	uart->set_input_buffer_pointer_to_beginning=&input_buffer_pointer_to_beginning;
	uart->set_null_to_buff_beginning=&set_null_to_begin;
}
