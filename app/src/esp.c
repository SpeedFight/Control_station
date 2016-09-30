/**
 * @file	esp.c
 * @Author	SpeedFight (speedfight_2@wp.pl)
 * @date	19.08.16
 */
#include "../inc/esp.h"
#include <avr/io.h>
#include <util/delay.h>
//#include "stdio.h"
#include <string.h>
#include <stdlib.h>


/**
 * @addtogroup esp_definitions
 * @{
 */

#define MAX_TRY 5  //how many times resend message

#define PIN_RESET	1
#define PORT_RESET	C

#define PIN_CH_PD	0
#define PORT_CH_PD	C

//DON'T CHANGE CODE BELLOW!!!//
//merge macros
#define _ESP_PIN(a)	    PIN   ## a
#define _ESP_PORT(a)	PORT     ## a
#define _ESP_DDR(a)	    DDR	  ## a

#define ESP_PIN(a) 	_ESP_PIN(a)
#define ESP_PORT(a)	_ESP_PORT(a)
#define ESP_DDR(a) 	_ESP_DDR(a)

//led on/off macros
#define RESET_OFF	ESP_PORT(PORT_RESET) &= ~(1<<ESP_PIN(PIN_RESET));
#define RESET_ON	ESP_PORT(PORT_RESET) |= (1<<ESP_PIN(PIN_RESET));

/** @}*/

/**
 * @addtogroup function_to_uart_definitions
 * @{
 */
static void (*send_uart)(char *);    //pointer to send via uart function

static char *input_buff;    //pointer to received buff
static volatile uint8_t *received_data_pack_flag; //flag to signalize incoming message
static void (*p_set_input_buffer_pointer_to_beginning)();
static void (*p_set_null_to_buff_beginning)();
/** @}*/



/**
 * #brief Short impulse on reset pin, make esp to reset
 */
static void reset()
{
        RESET_ON;
        _delay_ms(1000);
        RESET_OFF;
        RESET_ON;
}

/**
 * @brief Return size of string given as argument
 */
 static uint16_t size_of_string(char *string)
 {
     uint16_t size;
     for(size=0;(*string);string++)
     {
         size++;
     }

     return size;
 }


/**
 * @brief return true if find *check string in input buff
 */
static uint8_t check_input_buff(char *check)
{
    if(strstr(input_buff,check))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static void clear_input_buff()
{
    p_set_input_buffer_pointer_to_beginning();
    p_set_null_to_buff_beginning();
    *received_data_pack_flag=0;
}

/**
 * @brief return true if find *check string in input buff, and if true, also clear input buff
 */
static uint8_t check_input_buff_and_clear(char *check)
{
    if(strstr(input_buff,check))
    {
        clear_input_buff();
        return 1;
    }
    else
        return 0;
}

/**
 * @brief send given message by uart, and wait to specific answer
 */
static uint8_t esp_accept_comand(char *string_to_send,char *ok_string, uint8_t wait_time)
{
    for(uint8_t try=0;try<MAX_TRY;try++)
    {
        clear_input_buff();
        send_uart(string_to_send);

        for(uint32_t try_time=0;try_time<((uint32_t)wait_time*10000);try_time++)
        {
            _delay_us(100);
            if(*received_data_pack_flag)
            {
                try_time=0;
                for(;try_time<((uint32_t)wait_time*10000);try_time++)
                {
                    _delay_us(100);

                    if(check_input_buff_and_clear(ok_string))
                        return 1;

                    //here and more if's
                }
            }
        }
        //if(!(try%4))
            //reset();

        //_delay_ms(1500);
    }
    return 0;
}

/**
 * @brief send given message by uart, and return answer
 */
static char *esp_accept_comand_return_answer(char *string_to_send, uint8_t wait_time)
{
    for(uint8_t try=0;try<MAX_TRY;try++)
    {
        clear_input_buff();
        send_uart(string_to_send);

        for(uint32_t try_time=0;try_time<((uint32_t)wait_time*10000);try_time++)
        {
            _delay_us(100);
            if(*received_data_pack_flag)
            {
                try_time=0;
                for(;try_time<((uint32_t)wait_time*10000);try_time++)
                {
                    _delay_us(100);

                }
                return input_buff;
            }
        }
        //if(!(try%4))
            //reset();

        //_delay_ms(1500);
    }
    return 0;
}

/**
 * @brief send given message by uart, and wait to specific answer, if false don't reset
 */
static uint8_t esp_accept_comand_one_try(char *string_to_send,char *ok_string, uint8_t wait_time)
{

        clear_input_buff();
        send_uart(string_to_send);

        for(uint32_t try_time=0;try_time<((uint32_t)wait_time*10000);try_time++)
        {
            _delay_us(100);
            if(*received_data_pack_flag)
            {
                try_time=0;
                for(;try_time<((uint32_t)wait_time*10000);try_time++)
                {
                    _delay_us(100);

                    if(check_input_buff_and_clear(ok_string))
                        return 1;

                    //here and more if's
                }
            }
        }

    return 0;
}

static uint8_t reset_until_ready()
{
    reset();
    for(uint32_t try_time=0;try_time<(1UL*10000);try_time++)
    {
        _delay_us(100);
        if(*received_data_pack_flag)
        {
            //try_time=0;
            for(;try_time<(1UL*10000);try_time++)
            {
                _delay_us(100);

                if(check_input_buff("ready"))
                {
                    return 1; //////////////////////////////////////
                    for(;try_time<(5UL*10000);try_time++)
                    {
                        _delay_us(100);
                        if(check_input_buff_and_clear("WIFI"))
                        {
                            //PORTD &=~(1<<PIN6);
                            return 1;
                        }
                    }
                }

                //here and more if's
            }
        }
        try_time=0;
        reset();

    }
    return 0;
}

static uint8_t ping()
{
    return (esp_accept_comand("AT+PING=\"www.google.pl\"\r\n","OK",13));
}

static uint8_t check_connection(void)
{
    for(uint8_t try=0;try<MAX_TRY;try++)
    {
        clear_input_buff();
        send_uart("AT+CWJAP?\r\n");

        for(uint32_t try_time=0;try_time<(2UL*10000);try_time++)
        {
            _delay_us(100);
            if(*received_data_pack_flag)
            {
                try_time=0;
                for(;try_time<(2UL*10000);try_time++)
                {
                    _delay_us(100);

                    if(check_input_buff_and_clear("CWJAP:\""))
                    {
                        return 1;
                    }

                    if(check_input_buff_and_clear("busy"))
                        try_time++;

                    if(check_input_buff_and_clear("No AP"))
                        try_time++;

                    /*
                    if(strstr(input_buff,"CWJAP:1"))//connection timeout
                    ;

                    if(strstr(input_buff,"CWJAP:2"))//wrong password
                    ;

                    if(strstr(input_buff,"CWJAP:3"))//can not found target ap
                    ;

                    if(strstr(input_buff,"CWJAP:4"))//connect fail
                    ;
                    */

                    //here and more if's
                }
            }
        }
        //if(!(try%4))
            //reset();

        //_delay_ms(1500);
    }
    return 0;
}
/*
static uint8_t log_to_wifi(char *ssid,char *password) //poprawić
{
        send_uart("AT+CWMODE=1\r\n");
        _delay_us(150);
        send_uart("AT+CWJAP=\"");send_uart(ssid);send_uart("\",\"");send_uart(password);send_uart("\"\r\n");
        _delay_ms(DELAY_TIME);

        //sprawdzenie czy sie udało++
        return 1;
}
*/

static uint8_t log_to_TCP(char *ip, char *port)
{
    uint8_t wait_time =10; //wait max 10 seconds

    //esp_accept_comand("AT+CIPMODE=0\r\n","OK",2); //niewymagane
    //esp_accept_comand("AT+CIPMUX=0\r\n","OK",2);  //niewymagane

    for(uint8_t try=0;try<MAX_TRY;try++)
    {
        clear_input_buff();
        send_uart("AT+CIPSTART=\"TCP\",\"");send_uart(ip);send_uart("\",");send_uart(port);send_uart("\r\n");

        for(uint32_t try_time=0;try_time<((uint32_t)wait_time*10000);try_time++)
        {
            _delay_us(100);
            if(*received_data_pack_flag)
            {
                try_time=0;
                for(;try_time<((uint32_t)wait_time*10000);try_time++)
                {
                    _delay_us(100);

                    if(check_input_buff_and_clear("OK"))
                        return 1;

                    if(check_input_buff_and_clear("ALREADY"))//ALREADY CONNECTED
                        return 1;

                    if(check_input_buff_and_clear("CLOSED"))
                        return 0;

                    //here and more if's
                }
            }
        }
        //if(!(try%4))
            //reset();

        //_delay_ms(1500);
    }
    return 0;
}

/**
 * @brief send specific message by esp to tcp serwer
 */
static uint8_t send_field_to_TCP(char *message,char *specific_answer,char *ip, char *port)
{
    uint16_t size=size_of_string(message);
    size+=2; //add size of \r\n
    char size_string[4];

    itoa (size, size_string, 10);

    if (!(log_to_TCP(ip,port)))
        return 0;   //if error

    send_uart("AT+CIPSEND=");
    send_uart(size_string);
    if (!(esp_accept_comand_one_try("\r\n","OK",2)))
        return 0;

    if (!(esp_accept_comand(message,specific_answer,13)))
        return 0;

    return 1;
}

/**
 * @brief Send message to TCP serwer, but control about message take specific function
 */
static uint8_t fnct_send_field_to_TCP(void (*other_send_function)(),
uint16_t message_length,
char *specific_answer,
char *ip,
char *port)
{
    char size_string[4]={'0','0','0','\0'};

    itoa ((message_length+2), size_string, 10);

    if (!(log_to_TCP(ip,port)))
        return 0;   //if error

    send_uart("AT+CIPSEND=");
    send_uart(size_string);
    //send_uart("\r\n");
    if (!(esp_accept_comand("\r\n","OK",2)))
        return 0;

    other_send_function();
    //send_uart("\r\n");
    if (!(esp_accept_comand("\r\n",specific_answer,13)))
        return 0;

    return 1;
}

//function return answer return from thingspeak talkback
static char *fnct_send_field_to_TCP_return_answer(void (*other_send_function)(),
uint16_t message_length,
char *ip,
char *port)
{
    char size_string[4];//={'0','0','0','\0'};
    static char answer[20];

    itoa ((message_length+2), size_string, 10);

    if (!(log_to_TCP(ip,port)))
        return 0;   //if error

    send_uart("AT+CIPSEND=");
    send_uart(size_string);
    //send_uart("\r\n");
    if (!(esp_accept_comand("\r\n","OK",2)))
        return 0;

    other_send_function();

    return strcpy(answer,esp_accept_comand_return_answer("\r\n",2));
    //return esp_accept_comand_return_answer("\r\n",2); //pointer to input buffer which collect specific answer given by thingspeak talkback

}

void esp_on()
{
    ESP_PORT(PORT_CH_PD) |= (1<<ESP_PIN(PIN_CH_PD)); //enable esp
}

void esp_off()
{
    ESP_PORT(PORT_CH_PD) &= !(1<<ESP_PIN(PIN_CH_PD)); //enable esp
}

/**
 * @brief Init struct
 * @detail
 */
 void esp_init_struct(  void (*uart_send_function)(char *),  //pointer to send function
                 void (*set_beggining_input_buffer)(),
                 void (*set_null_to_buff_beginning)(),
                 char * ptr_input_array, //pointer to input data buffer
                 volatile uint8_t *received_data_flag,
                 esp_typedef *esp)       //pointer to struct
{
    ESP_DDR(PORT_RESET) |=(1<<ESP_PIN(PIN_RESET));
    ESP_DDR(PORT_CH_PD) |=(1<<ESP_PIN(PIN_CH_PD));

        send_uart=uart_send_function;

        received_data_pack_flag=received_data_flag;

        input_buff=ptr_input_array;

        p_set_input_buffer_pointer_to_beginning=set_beggining_input_buffer; //set function pointer

        p_set_null_to_buff_beginning=set_null_to_buff_beginning;

        esp->test_internet=&ping;
        esp->test_ap=&check_connection;
        esp->reset_until_ready=&reset_until_ready;
        esp->send_to_TCP=&send_field_to_TCP;
        esp->fnct_send_to_TCP=&fnct_send_field_to_TCP;
        esp->fnct_send_field_to_TCP_return_answer=&fnct_send_field_to_TCP_return_answer;

        esp->esp_on=&esp_on;
        esp->esp_off=&esp_off;
}
