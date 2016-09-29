/**
* @file	thingspeak.h
* @Author	SpeedFight (speedfight_2@wp.pl)
* @date	04.09.16
*/
#ifndef _thingspeak_h
#define _thingspeak_h

 #include <avr/io.h>

 /**
  * @struct data_field_typedef
  * @brief Structure contain data set for one meter
  * @var *field_no pointer
  *      to one static string
  *
  * @var *field_value
  *      this field is for contain pointer to data from sensor in string format
  */
typedef struct{
    const char *field_no;
    char *field_value;
}data_field_typedef;

/**
 * @struct data_field_typedef
 * @brief Structure contain data set for one meter
 * @details Watch example!
 * @var *ip
 *      contain pointer for IP for thingspeak
 *
 * @var *port
 *      contain pointer for port for thingspeak
 *
 * @var *channel_id
 *      Specific channel id
 *
 * @var *api_key
 *      specific api key for channel
 *
 * @var (*post_message_length)
 *      Pointer to function which return length of string who will be send
 *
 * @var (*send_post)
 *      Pointer to function which send by uart formater string special for esp8266
 */
typedef struct{
     const char *ip;
     const char *port;
     const char *channel_id;
     const char *api_key;

     uint16_t (*post_message_length)();
     void (*send_post)();

 }thingspeak_typedef;


/**
 * @addtogroup data_fiel_definitions
 * @brief signalize how many data from sensor you want send in one time
 *        max 4, min 1
 * @detials for 1 data -> comment all
 *          for 2 data -> leave uncmmented DATA_FIELD_2
 *          .
 *          .
 *          .
 */
 //#define DATA_FIELD_2
 #define DATA_FIELD_3
 //#define DATA_FIELD_4


 #ifdef DATA_FIELD_1
 void thingspeak_init_struct(void (*uart_send_function)(char *),
                                thingspeak_typedef *thingspeak_struct,
                                 data_field_typedef *data1);
 #endif


 #ifdef DATA_FIELD_2
 void thingspeak_init_struct(void (*uart_send_function)(char *),
                                thingspeak_typedef *thingspeak_struct,
                                 data_field_typedef *data1,
                                 data_field_typedef *data2);
 #endif

 #ifdef DATA_FIELD_3
 void thingspeak_init_struct(void (*uart_send_function)(char *),
                                thingspeak_typedef *thingspeak_struct,
                                 data_field_typedef *data1,
                                 data_field_typedef *data2,
                                 data_field_typedef *data3);
 #endif

 #ifdef DATA_FIELD_4
 void thingspeak_init_struct(void (*uart_send_function)(char *),
                                thingspeak_typedef *thingspeak_struct,
                                 data_field_typedef *data1,
                                 data_field_typedef *data2,
                                 data_field_typedef *data3,
                                 data_field_typedef *data4);
 #endif

 /**
  * @example
  * @warning need uart.h esp.h
  * @code
  *
  *  const char ip[]="184.106.153.149";
  *  const char port[]="xx";
  *  const char channel_id[]="xxx";
  *  const char api_key[]="xxxx";
  *
  *
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

          thingspeak_typedef thingspeak={
      		.ip=ip,
      		.port=port,
      		.channel_id=channel_id,
      		.api_key=api_key
      	};

  	data_field_typedef temperature=	{.field_no="1"};
  	data_field_typedef humidity=	{.field_no="2"};
  	data_field_typedef pressure=	{.field_no="3"};
  	data_field_typedef light=		{.field_no="4"};

  	thingspeak_init_struct(uart.send,
  							&thingspeak,
  							&temperature,
  							&humidity,
  							&pressure,
  							&light);


  *  esp.reset_until_ready();
  *  esp.test_ap();
  *  esp.test_internet();
  *  esp.fnct_send_to_TCP(thingspeak.send_post,
                          thingspeak.post_message_length(),
                          "OK",ip,port)
  *
  *
  */
 #endif
