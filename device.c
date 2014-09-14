#include <vsm_api.h>

enum
{
	A, B, C
} PINS;

VSM_PIN device_pins[]=
{
	{.is_digital=TRUE, .name = "A", .number = 1, .on_time=15000000000, .off_time=22000000000},
	{.is_digital=TRUE, .name = "B", .number = 2, .on_time=15000000000, .off_time=22000000000},
	{.is_digital=TRUE, .name = "C", .number = 3, .on_time=15000000000, .off_time=22000000000},
	{.name = 0}
};

void device_simulate (void)
{
	if(FALSE == is_pin_floating(device_pins[A].pin) && FALSE == is_pin_floating(device_pins[B].pin))
	{
		if(TRUE == is_pin_high(device_pins[A].pin) && TRUE == is_pin_high(device_pins[B].pin))
			set_pin_state(device_pins[C], SLO);
		else
			set_pin_state(device_pins[C], SHI);
	}	

}
