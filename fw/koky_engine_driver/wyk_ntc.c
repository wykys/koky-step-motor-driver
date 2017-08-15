#include "wyk_ntc.h"

// HW constant
#define V_REF	5.0		// reference voltage
#define ADC_MAX	1023	// reference voltage
#define R_DIV	10e3	// resistor in voltage divider
// ========================================================
// NTC constant
#define A1		3.354016e-03
#define B1		2.569850e-04
#define C1		2.620131e-06
#define D1		6.383091e-08
#define R_REF	10e3
// ========================================================

// NTC initialization
void ntc_init(void)
{
	ADMUX = (1<<REFS0) | 5;							// referention AVCC, measure on channel ADC5
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1);	// ADEN - enable ADC, ACD clk = F_CPU/64
}

// NCT measure
int16_t ntc_measure(void)
{
	int16_t adc, ntc_res, temp;
	double volt, tmp_log;
	
	ADCSRA |= (1<<ADSC);        // start conversion
	while(ADCSRA & (1<<ADSC));  // wait to end conversion
	adc=ADC;					// value measurement
	
	// convert to voltage
	volt = (V_REF/ADC_MAX) * ((double)adc);
	ntc_res = (-(volt * R_DIV) / V_REF) / ((volt/V_REF) - 1);
	
	tmp_log = log(ntc_res/R_REF);
	temp = 1.0 / ( A1 + B1*tmp_log + C1*tmp_log*tmp_log + D1*tmp_log*tmp_log*tmp_log ) - 273.15;
	
	return temp;
}
