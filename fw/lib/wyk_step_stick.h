/*
 * Knihovna pro nastavování step stick
 *
 * autor: wykys
 * verze: 1.0
 * datum: 10.7.2017
 */

#ifndef WYK_STEP_STICK_H_INCLUDED
#define WYK_STEP_STICK_H_INCLUDED

#include "settings.h"
#include <avr/io.h>
#include <util/delay.h>
#include "TUI.h"

#define MOTOR_STEP_PORT   PORTB
#define	MOTOR_STEP_DDR    DDRB
#define MOTOR_STEP_PIN    (1<<PB3)

#define MOTOR_DIR_PORT    PORTD
#define	MOTOR_DIR_DDR     DDRD
#define MOTOR_DIR_PIN     (1<<PD5)

#define MOTOR_ENA_PORT    PORTD
#define	MOTOR_ENA_DDR     DDRD
#define MOTOR_ENA_PIN     (1<<PD7)

#define MOTOR_MS1_PORT    PORTB
#define	MOTOR_MS1_DDR     DDRB
#define MOTOR_MS1_PIN     (1<<PB0)

#define MOTOR_MS2_PORT    PORTB
#define	MOTOR_MS2_DDR     DDRB
#define MOTOR_MS2_PIN     (1<<PB1)

#define MOTOR_MS3_PORT    PORTB
#define	MOTOR_MS3_DDR     DDRB
#define MOTOR_MS3_PIN     (1<<PB2)

#define MOTOR_STEP0       MOTOR_STEP_PORT &= ~MOTOR_STEP_PIN
#define MOTOR_STEP1       MOTOR_STEP_PORT |= MOTOR_STEP_PIN

#define MOTOR_DIR0        MOTOR_DIR_PORT &= ~MOTOR_DIR_PIN
#define MOTOR_DIR1        MOTOR_DIR_PORT |= MOTOR_DIR_PIN

#define MOTOR_DRIVER_ON   MOTOR_ENA_PORT &= ~MOTOR_ENA_PIN
#define MOTOR_DRIVER_OFF  MOTOR_ENA_PORT |= MOTOR_ENA_PIN

#define MOTOR_MS1_0       MOTOR_MS1_PORT &= ~MOTOR_MS1_PIN
#define MOTOR_MS1_1       MOTOR_MS1_PORT |= MOTOR_MS1_PIN

#define MOTOR_MS2_0       MOTOR_MS2_PORT &= ~MOTOR_MS2_PIN
#define MOTOR_MS2_1       MOTOR_MS2_PORT |= MOTOR_MS2_PIN

#define MOTOR_MS3_0       MOTOR_MS3_PORT &= ~MOTOR_MS3_PIN
#define MOTOR_MS3_1       MOTOR_MS3_PORT |= MOTOR_MS3_PIN

#define MOTOR_STEP        1.8                           // stupen otoceni motoru
#define MOTOR_USTEP       16                            // obracena hodnota z modu full 1; half 2; quadret 4...
#define MOTOR_PERIOD      360/(MOTOR_STEP/MOTOR_USTEP)  // pocet kroku na otocku


void motor_init(void);
void motor_set_rpm(uint16_t rpm);
void motor_smoothly_rmp(uint16_t rpm_start, uint16_t rpm_end);

#endif // WYK_STEP_STICK_H_INCLUDED
