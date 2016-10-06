/**
* @file	relay.h
* @Author	SpeedFight (speedfight_2@wp.pl)
* @date	06.10.16
*/

#ifndef _relay_h
#define _relay_h

#include <avr/io.h>

typedef struct
{
    void (*on)();
    void (*off)();
}relay_typedef;

void relay_init_struct(relay_typedef *relay);

#endif
