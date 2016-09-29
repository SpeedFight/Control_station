#ifndef _esp_h
#define _esp_h
/**
 * @file	esp.h
 * @Author	SpeedFight (speedfight_2@wp.pl)
 * @date	19.08.16
 */

#include <avr/io.h>

/**
 * @struct esp_typedef
 * @brief This structure contains all efinitions to esp library.
 *        Watch example to know how to use it.
 * @var (*test_internet)
 *      Function check connection to internet
 *
 * @var (*test_ap)
 *      Check connection to wifi router
 *
 * @var (*reset_until_ready)
 *      reset esp that many time as get responsive answer
 *
 * @var (*send_to_TCP)
 *      send message to tcp serwer
 *
 * @var (*fnct_send_to_TCP)
 *      send message to tcp serwer by specific function
 */
typedef struct{
        //uint8_t (*connect_to_wifi)(char *ssid, char *password);
        uint8_t (*test_internet)();
        uint8_t (*test_ap)();
        uint8_t (*reset_until_ready)();
        uint8_t (*send_to_TCP)(char *message,char *specific_answer,char *ip, char *port);
        uint8_t (*fnct_send_to_TCP)(void (*other_send_function)(),
                uint16_t message_length,char *specific_answer,char *ip,char *port);

        void (*esp_on)();
        void (*esp_off)();
}esp_typedef;

/**
 * @brief Function to initialize
 * @details
 *
 * @param [in] void (*uart_send_function)(char *)
 *             pointer to send function
 *
 * @param [in] void (*set_beggining_input_buffer)()
 *             pointer to function which set input buff to beginning
 *
 * @param [in] void (*set_null_to_buff_beginning)()
 *             pointer to function which set null to beginning of input buff
 *
 * @param [in] char * ptr_input_array
 *             Pointer to input array
 *
 * @param [in,out] volatile uint8_t *received_data_flag
 *
 * @param [in,out] esp_typedef *esp
 *                 pointer to esp_typedef struct
 */
void esp_init_struct(  void (*uart_send_function)(char *),  //pointer to send function
                void (*set_beggining_input_buffer)(),
                void (*set_null_to_buff_beginning)(),
                char * ptr_input_array, //pointer to input data buffer
                volatile uint8_t *received_data_flag,
                esp_typedef *esp);       //pointer to struct

/**
 * @example
 * @code
 *  comm_typedef uart;
 *  uart_init_struct(&uart);
 *  uart.init();
 *
 * esp_typedef esp;
    esp_init_struct(uart.send,
         uart.set_input_buffer_pointer_to_beginning,
         uart.set_null_to_buff_beginning,
         uart.received,
         uart.received_data_pack_flag,
         &esp);


 *  esp.reset_until_ready();
 *  esp.test_ap();
 *  esp.test_internet();
 *  esp.send_to_TCP(message,"+IPD,2:",ip,port);
 */

#endif
