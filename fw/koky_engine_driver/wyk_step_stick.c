#include "wyk_step_stick.h"

void motor_init(void)
{
	MOTOR_ENA_DDR |= MOTOR_ENA_PIN;
	MOTOR_DIR_DDR |= MOTOR_DIR_PIN;
	MOTOR_STEP_DDR |= MOTOR_STEP_PIN;

	MOTOR_DRIVER_OFF;
	
	switch (MOTOR_USTEP)
	{
		case 1:
			MOTOR_MS1_0;
			MOTOR_MS2_0;
			MOTOR_MS3_0;
			break;
		case 2:
			MOTOR_MS1_1;
			MOTOR_MS2_0;
			MOTOR_MS3_0;
			break;
		case 4:
			MOTOR_MS1_0;
			MOTOR_MS2_1;
			MOTOR_MS3_0;
			break;
		case 8:
			MOTOR_MS1_1;
			MOTOR_MS2_1;
			MOTOR_MS3_0;
			break;
		default:
			MOTOR_MS1_1;
			MOTOR_MS2_1;
			MOTOR_MS3_1;
	}

	

	TCCR2A = (1<<COM2A0) | (1<<WGM21);				// toggle OCR2A CTC mode
	TCCR2B = 0;										// clock off
	OCR2A = 0;
	TCNT2 = 0;
	MOTOR_DIR0;
}

void motor_set_rpm(uint16_t rpm)					// rote per minute
{
	uint32_t buf;
	const uint32_t freq = F_CPU*60;
	const uint16_t div[] = {1, 8, 32, 64, 128, 256, 1024};
	const uint8_t div_len = 7;

	uint8_t i;

	if (rpm)
	{
		for (i=0; i<div_len; i++)
		{
			buf = freq / ( ((uint32_t)rpm) * MOTOR_PERIOD * 2 * div[i] );
			if (buf < 0xFF && buf > 0)
			{
				TCCR2B = i+1;						// clock/div[i]
				OCR2A = (uint8_t )buf;
				TCNT2 = 0;
				MOTOR_DRIVER_ON;
				break;
			}
		}
	}
	else
	{
		TCCR2B = 0x00;
		MOTOR_DRIVER_OFF;
	}
	actual_rmp = rpm;
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

	for (rpm=rpm_start; rpm!=(rpm_end+step) && !(EIFR & (1<<INTF0)); rpm+=step)
	{
		user_profile.rpm = rpm;
		if (!(user_profile.rpm % 20))
			TUI('s');
		motor_set_rpm(user_profile.rpm);
		_delay_ms(30);
	}
	TUI('s');
}
