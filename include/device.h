#ifndef DEVICE_H
#define DEVICE_H
#include <vsm_api.h>

typedef struct VSM_PIN
{
	BOOL is_digital;
	char* name;
	ABSTIME on_time;
	ABSTIME off_time;
	IDSIMPIN* pin;
} VSM_PIN;

IINSTANCE* model_instance;
IDSIMCKT* model_dsim;
extern VSM_PIN device_pins[32];

#endif
