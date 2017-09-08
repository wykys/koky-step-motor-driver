/*
 * Knihovna pro ovládání ventilu
 *
 * autor: wykys
 * verze: 1.0
 * datum: 15.8.2017
 */

#ifndef WYK_VALVE_H_INCLUDED
#define WYK_VALVE_H_INCLUDED

#include "settings.h"
#include <avr/io.h>

#define VALVE_PIN	(1<<PD1)
#define VALVE_PORT	PORTD
#define VALVE_DDR	DDRD

#define VALVE_OUT_0 VALVE_PORT &= ~VALVE_PIN
#define VALVE_OUT_1 VALVE_PORT |= VALVE_PIN

#define  VALVE_ON	VALVE_OUT_1
#define  VALVE_OFF	VALVE_OUT_0

// ========================================================
void valve_init(void);
// ========================================================

#endif // WYK_VALVE_H_INCLUDED
