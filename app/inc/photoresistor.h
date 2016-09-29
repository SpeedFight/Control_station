/**
* @file	photoresistor.h
* @Author	SpeedFight (speedfight_2@wp.pl)
* @date	24.09.16
*/

#ifndef _photoresistor_h
#define _photoresistor_h

#include <avr/io.h>

typedef struct
{
    void (*init)();
    void (*reset_average)();
    void (*start_measure)();
    char *(*get_brightness)();

}photoresistor_typedef;

void photoresistor_init_struct(photoresistor_typedef *photoresistor);

#endif
