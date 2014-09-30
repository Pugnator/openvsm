#ifndef DEVICE_H
#define DEVICE_H
#include <vsm_api.h>

typedef struct VSM_PIN 
{
	BOOL is_digital; ///< Pin is a digital one
	char* name;	///< The name of the pin in graphical model
	ABSTIME on_time; ///< Pin switch on-time
	ABSTIME off_time; ///< Pin switch off-time
	IDSIMPIN* pin; ///< DSIM pin pointer itself
} VSM_PIN; ///< OpenVSM pin structure

IINSTANCE* model_instance; ///< Global model instance object
IDSIMCKT* model_dsim; ///< Global DSIM object
extern VSM_PIN device_pins[32]; ///< Global array of device pins, index starts from 1 not from 0

#endif
