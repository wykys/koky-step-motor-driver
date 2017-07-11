#include <stdio.h>

#define F_CPU 24000000UL
#define MOTOR_STEP		1.8								// stupe� oto�en� motoru
#define MOTOR_USTEP		16								// obr�cen� hodnota z m�du full 1; half 2; quadret 4...
#define MOTOR_PERIOD	360/(MOTOR_STEP/MOTOR_USTEP)	// po�et krok� na oto�ku

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

void set_speed(uint16_t speed) // speed otacky za minutu
{
	double buf, freq;
	const uint16_t div[] = {1, 8, 64, 256, 1024};
	const uint8_t div_len = 5;

	uint8_t i;

  printf("Speed: \t%d\t", speed);

	if (speed)
	{
		freq = F_CPU*60;

		for (i=0; i<div_len; i++)
		{
			buf = freq / ( speed * MOTOR_PERIOD * 2 * div[i] );
			if (buf < 0xFF && buf > 0)
			{
				//TCCR1B = (div_len-i) | (1<<WGM12);	// clock/div[i] in CTC mode
				//OCR1A = (uint8_t )buf;
				//TCNT1 = 0;
				//rot_min = speed;
				//DRIVER_ON;
        printf("Splňuje kritéria\tbuf:\t%f\t%d\tdiv:\t%d\n", buf, (uint8_t)buf, div[i]);
				break;
			}
		}
	}
	else
	{
		//DRIVER_OFF;
	}
}

int main(void)
{
  uint8_t i;
  for (i=0; i<0xFF; i++)
  {
    set_speed(i);
  }
  return 0;
}
