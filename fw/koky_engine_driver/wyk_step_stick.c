#include "wyk_step_stick.h"

void motor_init(void)
{
	MOTOR_ENA_DDR |= MOTOR_ENA_PIN;
	MOTOR_DIR_DDR |= MOTOR_DIR_PIN;
	MOTOR_STEP_DDR |= MOTOR_STEP_PIN;

	MOTOR_DRIVER_OFF;

	TCCR0A = (1<<COM0A0) | (1<<WGM01);				// toggle OCR1 CTC mode
	TCCR0B = 0;										// clock off CTC mode
	OCR0A = 0;
	TCNT0 = 0;
	MOTOR_DIR0;
}

void motor_set_rpm(uint16_t rpm)					// rote per minute
{
	uint32_t buf;
	const uint32_t freq = F_CPU*60;
	const uint16_t div[] = {1, 8, 64, 256, 1024};
	const uint8_t div_len = 5;

	uint8_t i;

	if (rpm)
	{
		for (i=0; i<div_len; i++)
		{
			buf = freq / ( ((uint32_t)rpm) * MOTOR_PERIOD * 2 * div[i] );
			if (buf < 0xFF && buf > 0)
			{
				TCCR0B = i+1;						// clock/div[i]
				OCR0A = (uint8_t )buf;
				TCNT0 = 0;
				MOTOR_DRIVER_ON;
				break;
			}
		}
	}
	else
	{
		TCCR0B = 0x00;
		MOTOR_DRIVER_OFF;
	}
}


void motor_smoothly_rmp(uint16_t rpm_start, uint16_t rpm_end)
{
	uint16_t rpm;
	int8_t step;
	
	if (rpm_start < rpm_end)
	{
		step = 1;
	}
	else
	{
		step = -1;
	}

	for (rpm=rpm_start; rpm!=(rpm_end+step); rpm+=step)
	{
		user_profile.rpm = rpm;
		if (!(user_profile.rpm % 20))
			TUI('s');
		motor_set_rpm(user_profile.rpm);
		_delay_ms(30);
	}
	TUI('s');
}
