#include "../lib/wyk_valve.h"

void valve_init(void)
{
	VALVE_DDR |= VALVE_PIN;
	VALVE_OUT_0;
}
