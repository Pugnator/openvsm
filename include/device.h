#ifndef DEVICE_H
#define DEVICE_H
#include <vsm_api.h>

typedef struct VSM_PIN
{
	BOOL is_digital;
	char* name;
	int8_t number;
	ABSTIME on_time;
	ABSTIME off_time;
	STATE default_state;
	IDSIMPIN* pin;
} VSM_PIN;

IINSTANCE* model_instance;
IDSIMCKT* model_dsim;
extern VSM_PIN device_pins[16];

void 
device_simulate ( void );
#endif
