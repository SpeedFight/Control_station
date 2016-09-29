/**
 * @file	uart.h
 * @Author	SpeedFight (speedfight_2@wp.pl)
 * @date	13.08.16
 */
#ifndef _uart_h
#define _uart_h

#include <avr/io.h>

/**
 * @struct comm_typedef
 * @brief This structure contains all efinitions to uart library.
 *        Watch example to know how to use it.
 * @var (*init)
 *      Function pointer to init function. Initialize uart.
 *
 * @var (*send)
 *      Function pointer to send function. Send cstrinf via uart
 *
 * @var *received
 *      Pointer to array with input cstring (end by \0)
 *
 * @var *received_data_pack_flag
 *      Pointer to variable signalize end of incoming data transmission
 *      When 1 -> there is new data in *received
 *      @warning Clear by user!
 *
 */
typedef struct
{
	void (*init)();
	void (*send)(char *message);
	char *received;
	volatile uint8_t *received_data_pack_flag;

	void (*set_input_buffer_pointer_to_beginning)();
	void (*set_null_to_buff_beginning)();
}comm_typedef;

/**
 * @brief Function to initialize
 * @details
 *
 * @param [in] *uart pointer to comm_typedef struct
 */
void uart_init_struct(comm_typedef *uart);

/**
 * @example
 * @code
 *  comm_typedef uart;
 *  uart_init_struct(&uart);
 *  uart.init();
 *  uart.send("hello");
 */
#endif
