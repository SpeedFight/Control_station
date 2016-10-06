/**
* @file	relay.c
* @Author	SpeedFight (speedfight_2@wp.pl)
* @date	06.10.16
*/

#include "../inc/relay.h"

//define relay pin
#define RELAY_PIN	4
#define RELAY_PORT	D

//DON'T CHANGE CODE BELLOW!!!//
//merge macros
#define _SET_PIN(a)	    PIN	## a
#define _SET_PORT(a)	PORT ## a
#define _SET_DDR(a)	    DDR	## a

//necessary addition level of abstaraction
//_RELAY_PIN(a) -> PINa
//If you write just _REALY_PIN([another define]) -> _REALY_PIN[another define]
#define SET_PIN(a) 	_SET_PIN(a)
#define SET_PORT(a)	_SET_PORT(a)
#define SET_DDR(a) 	_SET_DDR(a)

//led on/off macros
#define RELAY_ON	SET_PORT(RELAY_PORT) &= ~(1<<SET_PIN(RELAY_PIN));
#define RELAY_OFF	SET_PORT(RELAY_PORT) |= (1<<SET_PIN(RELAY_PIN));

static void configure_relay()
{
    SET_DDR(RELAY_PORT) |=(1<<SET_PIN(RELAY_PIN));	//set pin as output
}

static inline void on()
{
    RELAY_ON;
}

static inline void off()
{
    RELAY_OFF;
}

void relay_init_struct(relay_typedef *relay)
{
    configure_relay();
    relay->on=&on;
    relay->off=&off;
}
