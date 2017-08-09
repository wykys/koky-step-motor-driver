/*
 * koky_engine_driver.c
 *
 * Created: 03.07.2017 14:24:52
 * Author : wykys
 */

#include "settings.h"

#define ENCODER_TICK_FOR_UPDATE		5

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/fuse.h>

FUSES =
{
	.low = 0xFF,
	.high = 0xD6,
	.extended = 0xFD
};

#include "TUI.h"
#include "wyk_lcd.h"
#include "wyk_step_stick.h"

// ============================================================================
#define BTN			(1<<PD2)
#define BTN_PORT	PORTD
#define BTN_PIN		PIND

#define ENC_A		(1<<PD4)
#define ENC_B		(1<<PD3)

#define ENC_A_PIN	PIND
#define ENC_B_PIN	PIND

#define ENC_A_PORT	PORTD
#define ENC_B_PORT	PORTD
// ============================================================================


ISR(INT0_vect)
{
	TUI_back_timer_clear();
	TUI('b');
}

ISR(INT1_vect)
{
	static uint8_t step = 0;
	static uint8_t dir = 0;

	TUI_back_timer_clear();
	if (ENC_A_PIN & ENC_A)
	{
		
		if (!dir)
		{
			step++;
			if (step >= ENCODER_TICK_FOR_UPDATE)
				TUI('-');
		}
		else
		{
			dir = 0;
			step = 0;
		}
	}
	else
	{
		if (dir)
		{
			step++;
			if (step >= ENCODER_TICK_FOR_UPDATE)
				TUI('+');
		}
		else
		{
			dir = 1;
			step = 0;
		}
	}
}

// encoder initialization
void encoder_init(void)
{
	// set button
	BTN_PORT |= BTN;		// pull-up
	EICRA = (1<<ISC01);		// falling edge
	EIMSK = (1<<INT0);		// int enable from INT0

	// set encoder	
	ENC_A_PORT |= ENC_A;	// pull-up
	ENC_B_PORT |= ENC_B;	// pull-up

	EICRA |= (1<<ISC11);	// falling edge
	EIMSK |= (1<<INT1);		// int enable from INT0
}

int main(void)
{
	DDRB = 0x00;
	DDRC = 0x00;
	DDRD = 0x00;

	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;

	motor_init();
	encoder_init();
	lcd_init();

	sei();

	TUI_inti();
	TUI('s');
		
	for(;;)
    {
		;
    }
}
