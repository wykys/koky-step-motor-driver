/*
 * Knihovna pro práci s NTC
 *
 * autor: wykys
 * verze: 1.0
 * datum: 15.8.2017
 */

#ifndef WYK_NTC_H_INCLUDED
#define WYK_NTC_H_INCLUDED

#include "settings.h"
#include <avr/io.h>
#include <math.h>

#define NTC_PIN		(1<<PC5)
#define NTC_PORT	PORTC
#define NTC_DDR		DDRC
// ========================================================


// ========================================================
void ntc_init(void);
int16_t ntc_measure(void);
// ========================================================

#endif // WYK_NTC_H_INCLUDED
