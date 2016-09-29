/**
* @file	photoresistor.c
* @Author	SpeedFight (speedfight_2@wp.pl)
* @date	24.09.16
*/

#include "../inc/photoresistor.h"
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t no_brightness;


#define ADC_PORT    7u  //adc7

//lowest voltage on voltage divider (photoresistor) ~0.2V
//((0.8*1023)/3.3)=62
//2 zakresy
//3.3->1.2V 1023 -> 372
//1.2->0    371 -> 1
#define ADC_LOW1  372u
#define ADC_LOW2  62u

#define ADC_MAX (1023u)

static void init()
{
    ADMUX |=(1<<REFS0) //AVCC with external capacitor at AREF pin
            |ADC_PORT; //set adc port to use //default: adc7

    ADCSRA |=   (1<<ADPS2) | (1<<ADPS0) //prescaler to 32
                |(1<<ADEN)  //enable adc
                |(1<<ADIE); ///enable interrupt

}

static void start_measure()
{
    ADCSRA |=(1<<ADSC); //start conversion
}

static void reset_average()
{
    no_brightness = 0;
}

static char *get_string_brightness()
{
    static char str_brightness[5];
    itoa (no_brightness, str_brightness, 10);
    return  str_brightness;
}

ISR(ADC_vect)
{
    uint16_t adc_tmp=0;

    adc_tmp = ADCL;
    adc_tmp |=ADCH<<8u;

    if(!(adc_tmp))
        adc_tmp++;

    if(!no_brightness)
    {
        if(adc_tmp>371u)
        {
            no_brightness = ((uint16_t)no_brightness*100u/(ADC_MAX- ADC_LOW1))/2u;
        }
        else
        {
            no_brightness = ((uint16_t)no_brightness*100u/ADC_MAX)/2u;
        }
    }


    if(adc_tmp>371u)
    {
        no_brightness = ((uint16_t)no_brightness +(((uint16_t)adc_tmp - ADC_LOW1)*100u/(ADC_MAX-ADC_LOW1)))/2u;
    }
    else
    {
        no_brightness = ((uint16_t)no_brightness +(((uint16_t)adc_tmp)*100u/ADC_MAX))/2u;
    }


/*
    //average measures
    if(!no_brightness)
        no_brightness = adc_tmp;//((adc_tmp - ADC_LOW)*100u/ADC_MAX);
    else
        //no_brightness = ((uint16_t)no_brightness +(((uint16_t)adc_tmp - ADC_LOW)*100u/ADC_MAX))/2u;
        no_brightness = ((uint16_t)no_brightness +((uint16_t)adc_tmp/2u));
*/
}

void photoresistor_init_struct(photoresistor_typedef *photoresistor)
{
    photoresistor->init=&init;
    photoresistor->reset_average=&reset_average;
    photoresistor->start_measure=&start_measure;
    photoresistor->get_brightness=&get_string_brightness;
}
