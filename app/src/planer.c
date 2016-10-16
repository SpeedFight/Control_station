/**
* @file	planer.c
* @Author	SpeedFight (speedfight_2@wp.pl)
* @date	25.09.16
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "../inc/uart.h"
#include "../inc/esp.h"
#include "../inc/thingspeak.h"
#include "../inc/planer.h"

#if defined(CZUJNIK1) || defined(CZUJNIK2) || defined(CZUJNIK3)
#include "../inc/photoresistor.h"
#include "../inc/dht.h"
#else
    #include "../inc/relay.h"
#endif

#include <string.h>
#include <stdlib.h>

//#define CZUJNIK1
//#define CZUJNIK2
//#define CZUJNIK3
#define TALKBACK

//#define IRQ_TIMER

const char ip[]="184.106.153.149";
const char port[]="80";

#ifdef CZUJNIK1
    const char channel_id[]="143012";
    const char api_key[]="8TPKDQ7OU004TBD5";
#endif

#ifdef CZUJNIK2
    const char channel_id[]="164416";
    const char api_key[]="TFLHHS8DBO3NRWRH";
#endif

#ifdef CZUJNIK3
    const char channel_id[]="164417";
    const char api_key[]="DKQUPX8IRUS6SHQJ";
#endif

#ifdef TALKBACK
    const char talkback_id[]="9837";
    const char api_key[]="G1PGOIQUKDE5182Y";

#endif

///

#if defined(CZUJNIK1) || defined(CZUJNIK2) || defined(CZUJNIK3)
static char str_temperature[4];
static char str_humidity[4];

int8_t no_temperature, no_humidity;
int8_t tmp_temp, tmp_hum;
#endif

#ifdef IRQ_TIMER
    typedef struct
    {
        uint8_t seconds;
        uint8_t minutes;
        uint8_t hours;
        uint8_t days;
    }time_typedef;

    volatile time_typedef time;

    //#define COMPARE_NUMBER (62500u-1u)
    #define COMPARE_NUMBER (12250u-1u)

    void start_timer()
    {
        TCCR1B |= (1<<WGM12); //CTC mode for 16bit timer A
        TCCR1B |= (1<<CS11) | (1<<CS10); //prescaler to 64

        OCR1A = COMPARE_NUMBER;

        TIMSK |= (1<<OCIE1A); //enable IRQ
    }

    uint8_t wait_minutes(uint8_t minutes)
    {
        static uint8_t actual_minutes;
        static uint8_t wait;
        static uint8_t flag;

        if(!(flag))
        {
            flag=1;
            actual_minutes=time.minutes;
        }

        if(!(actual_minutes==time.minutes))
        {
            flag=0;
            wait++;
        }

        if(wait>=minutes)
        {
            flag=0;
            wait=0;
            return 1;
        }

        return 0;
    }


    uint8_t wait_hours(uint8_t hours)
    {
        static uint8_t actual_hour;
        static uint8_t actual_minutes;
        static uint8_t wait;
        static uint8_t flag;


        if(!(flag))
        {
            flag=1;
            actual_hour=time.hours;
            if(!(wait))
                actual_minutes=time.minutes;

        }

        if(!(actual_hour==time.hours))
        {
            flag=0;
            wait++;
        }

        if(wait>=hours && actual_minutes>=time.minutes)
        {
            flag=0;
            wait=0;
            return 1;
        }

        return 0;
    }

    ISR (TIMER1_COMPA_vect)
    {
        time.seconds++;

        if(time.seconds>59u)
        {
            time.seconds=0;
            time.minutes++;
        }

        if(time.minutes>59u)
        {
            time.minutes=0;
            time.hours++;
        }

        if(time.hours>23u)
        {
            time.hours=0;
            time.days++;
        }
    }
#endif

uint8_t main_activity()
{
    comm_typedef uart;
    uart_init_struct(&uart);
    uart.init();


    //init esp
    esp_typedef esp;
    esp_init_struct(uart.send,
            uart.set_input_buffer_pointer_to_beginning,
            uart.set_null_to_buff_beginning,
            uart.received,
            uart.received_data_pack_flag,
            &esp);

#if defined(CZUJNIK1) || defined(CZUJNIK2) || defined(CZUJNIK3)

    thingspeak_typedef thingspeak={
        .ip=ip,
        .port=port,
        .channel_id=channel_id,
        .api_key=api_key
    };

    //init photoresistor
    photoresistor_typedef photoresistor;
    photoresistor_init_struct(&photoresistor);
    photoresistor.init();

    #ifdef CZUJNIK1
    data_field_typedef temperature=	{.field_no="1"};
    data_field_typedef humidity=	{.field_no="2"};
    data_field_typedef light=   	{.field_no="3"};
    #endif

    #ifdef CZUJNIK2
    data_field_typedef temperature=	{.field_no="1"};
    data_field_typedef humidity=	{.field_no="2"};
    data_field_typedef light=   	{.field_no="3"};
    #endif

    #ifdef CZUJNIK3
    data_field_typedef temperature=	{.field_no="1"};
    data_field_typedef humidity=	{.field_no="2"};
    data_field_typedef light=   	{.field_no="3"};
    #endif

    temperature.field_value=str_temperature;
    humidity.field_value=str_humidity;

    thingspeak_init_struct_and_data(uart.send,
                            &thingspeak,
                            &temperature,
                            &humidity,
                            &light);

#else

    thingspeak_typedef thingspeak={
        .ip=ip,
        .port=port,
        .channel_id=talkback_id,
        .api_key=api_key
    };
    thingspeak_init_struct(uart.send,&thingspeak);
    relay_typedef relay;    //configure relay
    relay_init_struct(&relay);
#endif

#ifdef IRQ_TIMER
    start_timer();
#endif

/////debug

    //char sec[3],min[3],hou[3];
    //char tmppp[4];

    uint8_t ile=0;
#ifdef TALKBACK
uint8_t relay_status=0;
char *answer;

//relay.on();
//_delay_ms(500);
//relay.off();

ERROR2:
esp.esp_on();

//wdt_reset();
if((esp.reset_until_ready()))
{

}
//wdt_reset();
//wdt_disable();
//*uart.received_data_pack_flag=0;

//esp.connect_to_wifi("Youkata","nicoyazawa");

*uart.received_data_pack_flag=0;

if(esp.test_ap())
{

}
*uart.received_data_pack_flag=0;

_delay_ms(1000);
if(esp.test_internet())
{

}
else
goto ERROR2;
*uart.received_data_pack_flag=0;


while(1)
{
        //wdt_reset();
        _delay_ms(500);
        ile++;

    if(ile>19u)
    {

        if(esp.test_internet())
        {

        }
        //else
        //goto ERROR2;
        ile=0;

        *uart.received_data_pack_flag=0;
        //wdt_enable(WDTO_2S);
        answer=
        esp.fnct_send_field_to_TCP_return_answer
        (thingspeak.send_request_all_talkback,
        thingspeak.talkback_request_all_message_length(),
        ip,
        port);

        //uart.send("AT+CIPCLOSE\r\n");
        //uart.send("wysłano\r\n");
        //uart.send("odebrano\r\n");
        //uart.send(answer);

        if(strstr(answer,"0x1"))
        {
            relay_status++;
            //uart.send("odp to 0x1\r\n");
            //relay.on();
        }
        if(strstr(answer,"0x2"))
        {
            relay_status=0;
            //uart.send("odp to 0x2\r\n");
            //relay.off();
        }


            if(relay_status)
            {
                relay.on();
            }

            if(!(relay_status))
            {
                relay.off();
            }

            //uart.send("koniec petli\r\n");
    }
    //uart.send("poza petlo\r\n");

}
#endif
}
