/**
* @file	relay.c
* @Author	SpeedFight (speedfight_2@wp.pl)
* @date	06.10.16
*/

//define relay pin
#define RELAY_PIN	4
#define RELAY_PORT	D


//DON'T CHANGE CODE BELLOW!!!//
//merge macros
#define _RELAY_PIN(a)	PIN	## a
#define _RELAY_PORT(a)	PORT	## a
#define _RELAY_DDR(a)	DDR	## a

//necessary addition level of abstaraction
//_RELAY_PIN(a) -> PINa
//If you write just _REALY_PIN([another define]) -> _REALY_PIN[another define]
#define RELAY_PIN(a) 	_RELAY_PIN(a)
#define RELAY_PORT(a)	_RELAY_PORT(a)
#define RELAY_DDR(a) 	_RELAY_DDR(a)

//led on/off macros
#define RELAY_ON	RELAY_PORT(RELAY_PORT) &= ~(1<<RELAY_PIN(RELAY_PIN));
#define RELAY_OFF	RELAY_PORT(RELAY_PORT) |= (1<<RELAY_PIN(RELAY_PIN));

static void configure_relay()
{
    RELAY_DDR(RELAY_PORT) |=(1<<RELAY_PIN(RELAY_PIN));	//set pin as output
}

static inline void on()
{
    RELAY_ON;
}

static inline void off()
{
    RELAY_OFF;
}
