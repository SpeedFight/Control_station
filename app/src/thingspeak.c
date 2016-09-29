#include "../inc/thingspeak.h"
#include <avr/io.h>
#include <util/delay.h>
//#include "stdio.h"
#include <string.h>
#include <stdlib.h>

/**
 * @addtogroup thingsepak_def
 * @{
 */
data_field_typedef *p_data1=NULL;

#ifdef DATA_FIELD_2
data_field_typedef *p_data2=NULL;
#endif

#ifdef DATA_FIELD_3
data_field_typedef *p_data2=NULL;
data_field_typedef *p_data3=NULL;
#endif

#ifdef DATA_FIELD_4
data_field_typedef *p_data2=NULL;
data_field_typedef *p_data3=NULL;
data_field_typedef *p_data4=NULL;
#endif
/** @}*/

thingspeak_typedef *thingspeak; //pointer to struct contain ip,port etc...

/**
 * @addtogroup static_string_def
 * @brief part of message for thingspeak
 * @{
 */
static const char thingspeak_GET_http[]="GET https://api.thingspeak.com/";
static const char thingspeak_Update_api_key[]="update?api_key=";
static const char thingspeak_field[]="&field";

static uint8_t (*uart_send)(char *message); //pointer to uart send function
/** @}*/

/**
 * @brief Return size of string given as argument
 */
static uint16_t size_of_string(const char *string)
{
    uint16_t size;
    for(size=0;(*string);string++)
    {
        size++;
    }

    return size;
}

/**
 * @brief Return size of message
 */
static uint16_t post_message_length(){
    uint16_t length;
    length=size_of_string(thingspeak_GET_http);
    length+=size_of_string(thingspeak_Update_api_key);
    length+=size_of_string(thingspeak->api_key);
    length+=size_of_string(thingspeak_field);
    length+=size_of_string(p_data1->field_no);
    length+=1; //'='
    length+=size_of_string(p_data1->field_value);

    #ifdef DATA_FIELD_2
        length+=size_of_string(thingspeak_field);
        length+=size_of_string(p_data2->field_no);
        length+=1; //'='
        length+=size_of_string(p_data2->field_value);
    #endif

    #ifdef DATA_FIELD_3
        length+=size_of_string(thingspeak_field);
        length+=size_of_string(p_data2->field_no);
        length+=1; //'='
        length+=size_of_string(p_data2->field_value);

        length+=size_of_string(thingspeak_field);
        length+=size_of_string(p_data3->field_no);
        length+=1; //'='
        length+=size_of_string(p_data3->field_value);
    #endif

    #ifdef DATA_FIELD_4
        length+=size_of_string(thingspeak_field);
        length+=size_of_string(p_data2->field_no);
        length+=1; //'='
        length+=size_of_string(p_data2->field_value);

        length+=size_of_string(thingspeak_field);
        length+=size_of_string(p_data3->field_no);
        length+=1; //'='
        length+=size_of_string(p_data3->field_value);

        length+=size_of_string(thingspeak_field);
        length+=size_of_string(p_data4->field_no);
        length+=1; //'='
        length+=size_of_string(p_data4->field_value);
    #endif

    return length;
}

/**
 * @brief format and send message
 */
static void send_post(void){
    uart_send(thingspeak_GET_http);
    uart_send(thingspeak_Update_api_key);
    uart_send(thingspeak->api_key);
    uart_send(thingspeak_field);
    uart_send(p_data1->field_no);
    uart_send("=");
    uart_send(p_data1->field_value);

    #ifdef DATA_FIELD_2
        uart_send(thingspeak_field);
        uart_send(p_data2->field_no);
        uart_send("=");
        uart_send(p_data2->field_value);
    #endif

    #ifdef DATA_FIELD_3
        uart_send(thingspeak_field);
        uart_send(p_data2->field_no);
        uart_send("=");
        uart_send(p_data2->field_value);

        uart_send(thingspeak_field);
        uart_send(p_data3->field_no);
        uart_send("=");
        uart_send(p_data3->field_value);
    #endif

    #ifdef DATA_FIELD_4
        uart_send(thingspeak_field);
        uart_send(p_data2->field_no);
        uart_send("=");
        uart_send(p_data2->field_value);

        uart_send(thingspeak_field);
        uart_send(p_data3->field_no);
        uart_send("=");
        uart_send(p_data3->field_value);

        uart_send(thingspeak_field);
        uart_send(p_data4->field_no);
        uart_send("=");
        uart_send(p_data4->field_value);
    #endif
}


/**
 * @brief Init function
 */
#ifdef DATA_FIELD_1
void thingspeak_init_struct(void (*uart_send_function)(char *),
                                thingspeak_typedef *thingspeak_struct,
                                data_field_typedef *data1)
#endif


#ifdef DATA_FIELD_2
void thingspeak_init_struct(void (*uart_send_function)(char *),
                                thingspeak_typedef *thingspeak_struct,
                                data_field_typedef *data1,
                                data_field_typedef *data2)
#endif

#ifdef DATA_FIELD_3
void thingspeak_init_struct(void (*uart_send_function)(char *),
                                thingspeak_typedef *thingspeak_struct,
                                data_field_typedef *data1,
                                data_field_typedef *data2,
                                data_field_typedef *data3)
#endif

#ifdef DATA_FIELD_4
void thingspeak_init_struct(void (*uart_send_function)(char *),
                                thingspeak_typedef *thingspeak_struct,
                                data_field_typedef *data1,
                                data_field_typedef *data2,
                                data_field_typedef *data3,
                                data_field_typedef *data4)
#endif
{
    uart_send=uart_send_function;

    thingspeak=thingspeak_struct;
    thingspeak_struct->post_message_length=&post_message_length;
    thingspeak_struct->send_post=&send_post;

    p_data1=data1;

    #ifdef DATA_FIELD_2
    p_data2=data2;
    #endif

    #ifdef DATA_FIELD_3
    p_data2=data2;
    p_data3=data3;
    #endif

    #ifdef DATA_FIELD_4
    p_data2=data2;
    p_data3=data3;
    p_data4=data4;
    #endif

}
//↓↓↓↓↓↓thingspeak talkback part //receiver
