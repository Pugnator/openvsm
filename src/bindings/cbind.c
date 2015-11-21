/**
 *
 * @file   c_bind.c
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @copyright Copyright 2014 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the BSD 2-Clause license.
 *
 */

#include <vsmapi.h>

/**********************************************************************************************//**
 * \fn	const char* state_to_string ( STATE pinstate )
 *
 * \brief	State to string.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param	pinstate	The pinstate.
 *
 * \return	null if it fails, else a char*.
 **************************************************************************************************/

const char* state_to_string ( STATE pinstate )
{
	switch ( pinstate )
	{
		case UNDEFINED:
			return "UNDEFINED";
		case TSTATE:
			return "TSTATE";
		case FSTATE:
			return "FSTATE";
		case PLO:
			return "PLO";
		case ILO:
			return "ILO";
		case SLO:
			return "SLO";
		case WLO:
			return "WLO";
		case FLT:
			return "FLT";
		case WHI:
			return "WHI";
		case SHI:
			return "SHI";
		case IHI:
			return "IHI";
		case PHI:
			return "PHI";
		case WUD:
			return "WUD";
		case SUD:
			return "SUD";
		default:
			return "wrong state";
	}
}

/**********************************************************************************************//**
 * \fn	bool vsm_register ( ILICENCESERVER* ils )
 *
 * \brief	Vsm register.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	ils	If non-null, the ils.
 *
 * \return	true if it succeeds, false if it fails.
 **************************************************************************************************/

bool
vsm_register ( ILICENCESERVER* ils )
{

	if ( FALSE ==  ils->vtable->authorize ( ils, 0, model_key, VSM_API_VERSION ) )
	{
		return FALSE;
	}
	return TRUE;
}

/**********************************************************************************************//**
 * \fn	void set_pin_state ( IDSIMMODEL* model, VSM_PIN pin, STATE state )
 *
 * \brief	Sets pin state.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 * \param	pin			 	The pin.
 * \param	state		 	The state.
 **************************************************************************************************/

void set_pin_state ( IDSIMMODEL* model, VSM_PIN pin, STATE state )
{
	ABSTIME curtime = 0;
	systime ( model, &curtime );
	pin.pin->vtable->setstate2 ( pin.pin, 0, curtime, pin.on_time, state );
}

/**********************************************************************************************//**
 * \fn	void set_pin_bool ( IDSIMMODEL* model, VSM_PIN pin, bool level )
 *
 * \brief	Sets pin bool.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 * \param	pin			 	The pin.
 * \param	level		 	true to level.
 **************************************************************************************************/

void set_pin_bool ( IDSIMMODEL* model, VSM_PIN pin, bool level )
{
	ABSTIME curtime = 0;
	systime ( model, &curtime );
	pin.pin->vtable->setstate2 ( pin.pin, 0, curtime, pin.on_time, level ? LOGIC_HI : LOGIC_LO );
}

/**********************************************************************************************//**
 * \fn	void systime ( IDSIMMODEL* model, ABSTIME* at )
 *
 * \brief	Systimes.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 * \param [in,out]	at   	If non-null, at.
 **************************************************************************************************/

void systime ( IDSIMMODEL* model, ABSTIME* at )
{
	model->model_dsim->vtable->sysvar ( model->model_dsim, 0, ( DOUBLE* ) at, DSIMTIMENOW );
}

/************************************************************************************************
 * \fn	char* get_device_id ( IDSIMMODEL* model )
 *
 * \brief	Gets device identifier.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 *
 * \return	null if it fails, else the device identifier.
 **************************************************************************************************/

char* get_device_id ( IDSIMMODEL* model )
{
	return model->model_instance->vtable->id ( model->model_instance, 0 );
}

/************************************************************************************************
 * \fn	char* get_string_param ( IDSIMMODEL* model, char* field_name )
 *
 * \brief	Gets string parameter.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	  	If non-null, the model.
 * \param [in,out]	field_name	If non-null, name of the field.
 *
 * \return	null if it fails, else the string parameter.
 **************************************************************************************************/

char* get_string_param ( IDSIMMODEL* model, char* field_name )
{
	return model->model_instance->vtable->getstrval ( model->model_instance, 0, field_name, "?" );
}

/**********************************************************************************************//**
 * \fn	bool get_bool_param ( IDSIMMODEL* model, char* field_name )
 *
 * \brief	Gets bool parameter.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	  	If non-null, the model.
 * \param [in,out]	field_name	If non-null, name of the field.
 *
 * \return	true if it succeeds, false if it fails.
 **************************************************************************************************/

bool get_bool_param ( IDSIMMODEL* model, char* field_name )
{
	return model->model_instance->vtable->getboolval ( model->model_instance, 0, field_name, FALSE );
}

/**********************************************************************************************//**
 * \fn	double get_num_param ( IDSIMMODEL* model, char* field_name )
 *
 * \brief	Gets number parameter.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	  	If non-null, the model.
 * \param [in,out]	field_name	If non-null, name of the field.
 *
 * \return	The number parameter.
 **************************************************************************************************/

double get_num_param ( IDSIMMODEL* model, char* field_name )
{
	double result = 0;
	model->model_instance->vtable->getnumval ( model->model_instance, 0, &result, field_name, 0.0 );
	return result;
}

/**********************************************************************************************//**
 * \fn	int32_t get_hex_param ( IDSIMMODEL* model, char* field_name )
 *
 * \brief	Gets hexadecimal parameter.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	  	If non-null, the model.
 * \param [in,out]	field_name	If non-null, name of the field.
 *
 * \return	The hexadecimal parameter.
 **************************************************************************************************/

int32_t get_hex_param ( IDSIMMODEL* model, char* field_name )
{
	return ( int32_t ) model->model_instance->vtable->gethexval ( model->model_instance, 0, field_name, 0 );
}

/**********************************************************************************************//**
 * \fn	int64_t get_init_param ( IDSIMMODEL* model, char* field_name )
 *
 * \brief	Gets initialise parameter.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	  	If non-null, the model.
 * \param [in,out]	field_name	If non-null, name of the field.
 *
 * \return	The initialise parameter.
 **************************************************************************************************/

int64_t get_init_param ( IDSIMMODEL* model, char* field_name )
{
	return ( int64_t ) model->model_instance->vtable->getinitval ( model->model_instance, 0, field_name, 0 );
}

/**********************************************************************************************//**
 * \fn	void load_image ( IDSIMMODEL* model, char* filename, uint8_t* buffer, size_t buffer_size )
 *
 * \brief	Loads an image.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model   	If non-null, the model.
 * \param [in,out]	filename	If non-null, filename of the file.
 * \param [in,out]	buffer  	If non-null, the buffer.
 * \param	buffer_size			Size of the buffer.
 **************************************************************************************************/

void load_image ( IDSIMMODEL* model, char* filename, uint8_t* buffer, size_t buffer_size )
{
	model->model_instance->vtable->loadmemory ( model->model_instance, 0, filename, buffer, buffer_size, 0, 0 );
}

/**********************************************************************************************//**
 * \fn	IPOPUP* create_popup ( IDSIMMODEL* model, CREATEPOPUPSTRUCT* cps )
 *
 * \brief	Creates a popup.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 * \param [in,out]	cps  	If non-null, the CPS.
 *
 * \return	null if it fails, else the new popup.
 **************************************************************************************************/

IPOPUP* create_popup ( IDSIMMODEL* model, CREATEPOPUPSTRUCT* cps )
{
	return ( ( IPOPUP* ) model->model_instance->vtable->createpopup ( model->model_instance, 0, cps ) );
}

/**********************************************************************************************//**
 * \fn	IMEMORYPOPUP* create_memory_popup ( IDSIMMODEL* model, const char* title, const int32_t id )
 *
 * \brief	Creates memory popup.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 * \param	title		 	The title.
 * \param	id			 	The identifier.
 *
 * \return	null if it fails, else the new memory popup.
 **************************************************************************************************/

IMEMORYPOPUP* create_memory_popup ( IDSIMMODEL* model, const char* title, const int32_t id )
{
	CREATEPOPUPSTRUCT* cps = malloc ( sizeof *cps );
	cps->caption = ( char* ) title;
	cps->flags = PWF_VISIBLE | PWF_HIDEONANIMATE | PWF_SIZEABLE| PWF_AUTOREFRESH ;
	cps->type = PWT_MEMORY;
	cps->height = 32;
	cps->width = 16;
	cps->id = id;
	IMEMORYPOPUP* popup = ( IMEMORYPOPUP* ) create_popup ( model, cps );
	free ( cps );
	return popup;
}

/**********************************************************************************************//**
 * \fn	IDEBUGPOPUP* create_debug_popup ( IDSIMMODEL* model, const char* title, const int32_t id )
 *
 * \brief	Creates debug popup.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 * \param	title		 	The title.
 * \param	id			 	The identifier.
 *
 * \return	null if it fails, else the new debug popup.
 **************************************************************************************************/

IDEBUGPOPUP* create_debug_popup ( IDSIMMODEL* model, const char* title, const int32_t id )
{
	CREATEPOPUPSTRUCT* cps = malloc ( sizeof *cps );
	cps->caption = ( char* ) title;
	cps->flags = PWF_VISIBLE | PWF_HIDEONANIMATE | PWF_SIZEABLE| PWF_AUTOREFRESH ;
	cps->type = PWT_DEBUG;
	cps->height = 200;
	cps->width = 640;
	cps->id = id;
	IDEBUGPOPUP* popup = create_popup ( model, cps );
	free ( cps );
	return popup;
}

/**********************************************************************************************//**
 * \fn	IDEBUGPOPUP* create_source_popup ( IDSIMMODEL* model, const char* title, const int32_t id )
 *
 * \brief	Creates source popup.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 * \param	title		 	The title.
 * \param	id			 	The identifier.
 *
 * \return	null if it fails, else the new source popup.
 **************************************************************************************************/

IDEBUGPOPUP* create_source_popup ( IDSIMMODEL* model, const char* title, const int32_t id )
{
	CREATEPOPUPSTRUCT* cps = malloc ( sizeof *cps );
	cps->caption = ( char* ) title;
	cps->flags = PWF_VISIBLE | PWF_HIDEONANIMATE | PWF_SIZEABLE| PWF_AUTOREFRESH ;
	cps->type = PWT_SOURCE;
	cps->height = 200;
	cps->width = 640;
	cps->id = id;
	IDEBUGPOPUP* popup = create_popup ( model, cps );
	free ( cps );
	return popup;
}

/**********************************************************************************************//**
 * \fn	IDEBUGPOPUP* create_status_popup ( IDSIMMODEL* model, const char* title, const int32_t id )
 *
 * \brief	Creates status popup.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 * \param	title		 	The title.
 * \param	id			 	The identifier.
 *
 * \return	null if it fails, else the new status popup.
 **************************************************************************************************/

IDEBUGPOPUP* create_status_popup ( IDSIMMODEL* model, const char* title, const int32_t id )
{
	CREATEPOPUPSTRUCT* cps = malloc ( sizeof *cps );
	cps->caption = ( char* ) title;
	cps->flags = PWF_VISIBLE | PWF_HIDEONANIMATE | PWF_SIZEABLE | PWF_AUTOREFRESH ;
	cps->type = PWT_STATUS;
	cps->height = 200;
	cps->width = 200;
	cps->id = id;
	IDEBUGPOPUP* popup = create_popup ( model, cps );
	free ( cps );
	return popup;
}

/**********************************************************************************************//**
 * \fn	IDEBUGPOPUP* create_var_popup ( IDSIMMODEL* model, const char* title, const int32_t id )
 *
 * \brief	Creates variable popup.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 * \param	title		 	The title.
 * \param	id			 	The identifier.
 *
 * \return	null if it fails, else the new variable popup.
 **************************************************************************************************/

IDEBUGPOPUP* create_var_popup ( IDSIMMODEL* model, const char* title, const int32_t id )
{
	CREATEPOPUPSTRUCT* cps = malloc ( sizeof *cps );
	cps->caption = ( char* ) title;
	cps->flags = PWF_VISIBLE | PWF_HIDEONANIMATE | PWF_SIZEABLE| PWF_AUTOREFRESH ;
	cps->type = PWT_VAR;
	cps->height = 200;
	cps->width = 200;
	cps->id = id;
	IDEBUGPOPUP* popup = create_popup ( model, cps );
	free ( cps );
	return popup;
}

/**********************************************************************************************//**
 * \fn	void delete_popup ( IDSIMMODEL* model, POPUPID id )
 *
 * \brief	Deletes the popup.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 * \param	id			 	The identifier.
 **************************************************************************************************/

void delete_popup ( IDSIMMODEL* model, POPUPID id )
{
	model->model_instance->vtable->deletepopup ( model->model_instance, 0, id );
}

/**********************************************************************************************//**
 * \fn	void set_memory_popup ( IMEMORYPOPUP* popup, size_t offset, void* buffer, size_t size )
 *
 * \brief	Sets memory popup.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	popup 	If non-null, the popup.
 * \param	offset		  	The offset.
 * \param [in,out]	buffer	If non-null, the buffer.
 * \param	size		  	The size.
 **************************************************************************************************/

void set_memory_popup ( IMEMORYPOPUP* popup, size_t offset, void* buffer, size_t size )
{
	popup->vtable->setmemory ( popup, 0, offset, buffer, size );
}

/**********************************************************************************************//**
 * \fn	bool add_source_file ( ISOURCEPOPUP* popup, char* filename, bool lowlevel )
 *
 * \brief	Adds a source file.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	popup   	If non-null, the popup.
 * \param [in,out]	filename	If non-null, filename of the file.
 * \param	lowlevel			true to lowlevel.
 *
 * \return	true if it succeeds, false if it fails.
 **************************************************************************************************/

bool add_source_file ( ISOURCEPOPUP* popup, char* filename, bool lowlevel )
{
	bool result = popup->vtable->addsrcfile ( popup, 0, filename, lowlevel );
	popup->vtable->setpcaddress ( popup, 0, 0 );
	return result;
}

/**********************************************************************************************//**
 * \fn	bool set_vdm_handler ( IDSIMMODEL* model )
 *
 * \brief	Handler, called when the set vdm.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 *
 * \return	true if it succeeds, false if it fails.
 **************************************************************************************************/

bool set_vdm_handler ( IDSIMMODEL* model )
{
	return model->model_instance->vtable->setvdmhlr ( model->model_instance, 0, &ICPU_DEVICE );
}

/**********************************************************************************************//**
 * \fn	void set_pc_address ( ISOURCEPOPUP* popup, size_t address )
 *
 * \brief	Sets PC address.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	popup	If non-null, the popup.
 * \param	address		 	The address.
 **************************************************************************************************/

void set_pc_address ( ISOURCEPOPUP* popup, size_t address )
{
	popup->vtable->setpcaddress ( popup, 0, address );
}

/**********************************************************************************************//**
 * \fn	void repaint_memory_popup ( IMEMORYPOPUP* popup )
 *
 * \brief	Repaint memory popup.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	popup	If non-null, the popup.
 **************************************************************************************************/

void repaint_memory_popup ( IMEMORYPOPUP* popup )
{
	popup->vtable->repaint ( popup, 0 );
}

/**********************************************************************************************//**
 * \fn	void print_to_debug_popup ( IDEBUGPOPUP* popup, const char* message )
 *
 * \brief	Print to debug popup.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	popup	If non-null, the popup.
 * \param	message		 	The message.
 **************************************************************************************************/

void print_to_debug_popup ( IDEBUGPOPUP* popup, const char* message )
{
	popup->vtable->print ( popup, ( char* ) message );
}

/**********************************************************************************************//**
 * \fn	void dump_to_debug_popup ( IDEBUGPOPUP* popup, const uint8_t* buf, uint32_t offset, uint32_t size )
 *
 * \brief	Dumps to debug popup.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	popup	If non-null, the popup.
 * \param	buf			 	The buffer.
 * \param	offset		 	The offset.
 * \param	size		 	The size.
 **************************************************************************************************/

void dump_to_debug_popup ( IDEBUGPOPUP* popup, const uint8_t* buf, uint32_t offset, uint32_t size )
{
	popup->vtable->dump ( popup, 0, buf + offset, size, 16 );
}

/**********************************************************************************************//**
 * \fn	const char* logic_type_to_string ( LOGIC_TYPE type )
 *
 * \brief	Logic type to string.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param	type	The type.
 *
 * \return	null if it fails, else a char*.
 **************************************************************************************************/

const char* logic_type_to_string ( LOGIC_TYPE type )
{
	switch ( type )
	{
		case TTL:
			return "TTL";
		case CMOS:
			return "CMOS";
		case I2L:
			return "I2L";
		default:
			return "Unknown";
	}
}

/**********************************************************************************************//**
 * \fn	void set_logic_type ( IDSIMMODEL* model, LOGIC_TYPE type )
 *
 * \brief	Sets logic type.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 * \param	type		 	The type.
 **************************************************************************************************/

void set_logic_type ( IDSIMMODEL* model, LOGIC_TYPE type )
{
	model->ltype = type;
	switch ( type )
	{
		case TTL:
			model->logic_high = SHI;
			model->logic_low = SLO;
			model->logic_flt = FLT;
			break;
		case CMOS:
			model->logic_high = IHI;
			model->logic_low = ILO;
			model->logic_flt = FLT;
			break;
		case I2L:
			model->logic_high = WHI;
			model->logic_low = WLO;
			model->logic_flt = FLT;
			break;
		default:
			print_error ( model, "Unknown logic type specified: %d", type );
			break;
	}
}

/**********************************************************************************************//**
 * \fn	void toggle_pin_state ( IDSIMMODEL* model, VSM_PIN pin )
 *
 * \brief	Toggle pin state.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 * \param	pin			 	The pin.
 **************************************************************************************************/

void toggle_pin_state ( IDSIMMODEL* model, VSM_PIN pin )
{
	if ( TRUE == is_pin_high ( pin.pin ) )
	{
		set_pin_state ( model, pin, LOGIC_LO );
		return;
	}
	else if ( TRUE == is_pin_low ( pin.pin ) )
	{
		set_pin_state ( model, pin, LOGIC_HI );
		return;
	}
}

/**********************************************************************************************//**
 * \fn	STATE get_pin_state ( IDSIMPIN* pin )
 *
 * \brief	Gets pin state.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	pin	If non-null, the pin.
 *
 * \return	The pin state.
 **************************************************************************************************/

STATE get_pin_state ( IDSIMPIN* pin )
{
	return pin->vtable->getstate ( pin, 0 );
}

/**********************************************************************************************//**
 * \fn	int get_pin_bool ( VSM_PIN pin )
 *
 * \brief	Gets pin bool.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param	pin	The pin.
 *
 * \return	The pin bool.
 **************************************************************************************************/

int get_pin_bool ( VSM_PIN pin )
{
	if ( TRUE == is_pin_high ( pin.pin ) )
	{
		return 1;
	}
	else if ( TRUE == is_pin_low ( pin.pin ) )
	{
		return 0;
	}
	/* return -1 if floating */
	return -1;
}

/**********************************************************************************************//**
 * \fn	bool is_pin_active ( IDSIMPIN* pin )
 *
 * \brief	Queries if a pin is active.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	pin	If non-null, the pin.
 *
 * \return	true if a pin is active, false if not.
 **************************************************************************************************/

bool is_pin_active ( IDSIMPIN* pin )
{
	return pin->vtable->isactive ( pin, 0 );
}

/**********************************************************************************************//**
 * \fn	bool is_pin_inactive ( IDSIMPIN* pin )
 *
 * \brief	Queries if a pin is inactive.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	pin	If non-null, the pin.
 *
 * \return	true if a pin is inactive, false if not.
 **************************************************************************************************/

bool is_pin_inactive ( IDSIMPIN* pin )
{
	return pin->vtable->isinactive ( pin, 0 );
}

/**********************************************************************************************//**
 * \fn	bool is_pin_posedge ( IDSIMPIN* pin )
 *
 * \brief	Query if 'pin' is pin posedge.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	pin	If non-null, the pin.
 *
 * \return	true if pin posedge, false if not.
 **************************************************************************************************/

bool is_pin_posedge ( IDSIMPIN* pin )
{
	return pin->vtable->isposedge ( pin, 0 );
}

/**********************************************************************************************//**
 * \fn	bool is_pin_negedge ( IDSIMPIN* pin )
 *
 * \brief	Query if 'pin' is pin negedge.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	pin	If non-null, the pin.
 *
 * \return	true if pin negedge, false if not.
 **************************************************************************************************/

bool is_pin_negedge ( IDSIMPIN* pin )
{
	return pin->vtable->isnegedge ( pin, 0 );
}

/**********************************************************************************************//**
 * \fn	bool is_pin_edge ( IDSIMPIN* pin )
 *
 * \brief	Query if 'pin' is pin edge.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	pin	If non-null, the pin.
 *
 * \return	true if pin edge, false if not.
 **************************************************************************************************/

bool is_pin_edge ( IDSIMPIN* pin )
{
	return pin->vtable->isedge ( pin, 0 );
}

/**********************************************************************************************//**
 * \fn	void set_callback ( IDSIMMODEL* model, RELTIME picotime, EVENTID id )
 *
 * \brief	Callback, called when the set.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 * \param	picotime	 	The picotime.
 * \param	id			 	The identifier.
 **************************************************************************************************/

void set_callback ( IDSIMMODEL* model, RELTIME picotime, EVENTID id )
{
	/* Add event to the model's hash table in order to keep count of registered events */
	model->model_dsim->vtable->setcallback ( model->model_dsim, 0, picotime, model, id );
}

/**********************************************************************************************//**
 * \fn	void print_info ( IDSIMMODEL* model, const char* format, ... )
 *
 * \brief	Print information.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 * \param	format		 	Describes the format to use.
 * \param	...			 	Variable arguments providing additional information.
 **************************************************************************************************/

void print_info ( IDSIMMODEL* model, const char* format, ... )
{
	char* string;
	va_list args;
	va_start ( args, format );
	if ( 0 > vasprintf ( &string, ( char* ) format, args ) ) string = NULL;
	va_end ( args );
	model->model_instance->vtable->log ( model->model_instance, string );
	free ( string );
}

/**********************************************************************************************//**
 * \fn	void print_message ( IDSIMMODEL* model, const char* format, ... )
 *
 * \brief	Print message.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 * \param	format		 	Describes the format to use.
 * \param	...			 	Variable arguments providing additional information.
 **************************************************************************************************/

void print_message ( IDSIMMODEL* model, const char* format, ... )
{
	char* string;
	va_list args;
	va_start ( args, format );
	if ( 0 > vasprintf ( &string, ( char* ) format, args ) ) string = NULL;
	va_end ( args );
	model->model_instance->vtable->message ( model->model_instance, string );
	free ( string );
}

/**********************************************************************************************//**
 * \fn	void print_warning ( IDSIMMODEL* model, const char* format, ... )
 *
 * \brief	Print warning.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 * \param	format		 	Describes the format to use.
 * \param	...			 	Variable arguments providing additional information.
 **************************************************************************************************/

void print_warning ( IDSIMMODEL* model, const char* format, ... )
{
	char* string;
	va_list args;
	va_start ( args, format );
	if ( 0 > vasprintf ( &string, ( char* ) format, args ) ) string = NULL;
	va_end ( args );
	model->model_instance->vtable->warning ( model->model_instance, string );
	free ( string );
}

/**********************************************************************************************//**
 * \fn	void print_error ( IDSIMMODEL* model, const char* format, ... )
 *
 * \brief	Print error.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model	If non-null, the model.
 * \param	format		 	Describes the format to use.
 * \param	...			 	Variable arguments providing additional information.
 **************************************************************************************************/

void print_error ( IDSIMMODEL* model, const char* format, ... )
{
	char* string;
	va_list args;
	va_start ( args, format );
	if ( 0 > vasprintf ( &string, ( char* ) format, args ) ) string = NULL;
	va_end ( args );
	model->model_instance->vtable->error ( model->model_instance, string );
	free ( string );
}

/**********************************************************************************************//**
 * \fn	IDSIMPIN* get_pin ( IDSIMMODEL* model, char* pin_name )
 *
 * \brief	Gets a pin.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	model   	If non-null, the model.
 * \param [in,out]	pin_name	If non-null, name of the pin.
 *
 * \return	null if it fails, else the pin.
 **************************************************************************************************/

IDSIMPIN* get_pin ( IDSIMMODEL* model, char* pin_name )
{
	return model->model_instance->vtable->getdsimpin ( model->model_instance, 0, pin_name, TRUE );
}

/**********************************************************************************************//**
 * \fn	bool is_pin_low ( IDSIMPIN* pin )
 *
 * \brief	Query if 'pin' is pin low.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	pin	If non-null, the pin.
 *
 * \return	true if pin low, false if not.
 **************************************************************************************************/

bool is_pin_low ( IDSIMPIN* pin )
{
	return islow ( pin->vtable->istate ( pin, 0 ) );
}

/**********************************************************************************************//**
 * \fn	bool is_pin_high ( IDSIMPIN* pin )
 *
 * \brief	Query if 'pin' is pin high.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	pin	If non-null, the pin.
 *
 * \return	true if pin high, false if not.
 **************************************************************************************************/

bool is_pin_high ( IDSIMPIN* pin )
{
	return ishigh ( pin->vtable->istate ( pin, 0 ) );
}

/**********************************************************************************************//**
 * \fn	bool is_pin_floating ( IDSIMPIN* pin )
 *
 * \brief	Query if 'pin' is pin floating.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	pin	If non-null, the pin.
 *
 * \return	true if pin floating, false if not.
 **************************************************************************************************/

bool is_pin_floating ( IDSIMPIN* pin )
{
	return isfloating ( pin->vtable->istate ( pin, 0 ) );
}

/**********************************************************************************************//**
 * \fn	bool is_pin_steady ( IDSIMPIN* pin )
 *
 * \brief	Query if 'pin' is pin steady.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	pin	If non-null, the pin.
 *
 * \return	true if pin steady, false if not.
 **************************************************************************************************/

bool is_pin_steady ( IDSIMPIN* pin )
{
	return pin->vtable->issteady ( pin, 0 );
}

/**********************************************************************************************//**
 * \fn	bool is_pin_inverted ( IDSIMPIN* pin )
 *
 * \brief	Query if 'pin' is pin inverted.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param [in,out]	pin	If non-null, the pin.
 *
 * \return	true if pin inverted, false if not.
 **************************************************************************************************/

bool is_pin_inverted ( IDSIMPIN* pin )
{
	return pin->vtable->invert ( pin, 0 );
}

/**********************************************************************************************//**
 * \fn	__INLINE__ bool islow ( STATE pinstate )
 *
 * \brief	Query if 'pinstate' islow.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param	pinstate	The pinstate.
 *
 * \return	true if it succeeds, false if it fails.
 **************************************************************************************************/

__INLINE__ bool islow ( STATE pinstate )
{
	return ( pinstate & SP_MASK ) == SP_LOW;
}

/**********************************************************************************************//**
 * \fn	__INLINE__ bool ishigh ( STATE pinstate )
 *
 * \brief	Query if 'pinstate' ishigh.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param	pinstate	The pinstate.
 *
 * \return	true if it succeeds, false if it fails.
 **************************************************************************************************/

__INLINE__ bool
ishigh ( STATE pinstate )
{
	return ( pinstate & SP_MASK ) == SP_HIGH;
}

/**********************************************************************************************//**
 * \fn	__INLINE__ bool isfloating ( STATE pinstate )
 *
 * \brief	Query if 'pinstate' isfloating.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param	pinstate	The pinstate.
 *
 * \return	true if it succeeds, false if it fails.
 **************************************************************************************************/

__INLINE__ bool
isfloating ( STATE pinstate )
{
	return ( pinstate & SP_MASK ) == SP_FLOAT;
}

/**********************************************************************************************//**
 * \fn	__INLINE__ bool iscontention ( STATE pinstate )
 *
 * \brief	Query if 'pinstate' iscontention.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param	pinstate	The pinstate.
 *
 * \return	true if it succeeds, false if it fails.
 **************************************************************************************************/

__INLINE__ bool
iscontention ( STATE pinstate )
{
	return pinstate & SF_CONTENTION;
}

/**********************************************************************************************//**
 * \fn	__INLINE__ bool isdefined ( STATE pinstate )
 *
 * \brief	Query if 'pinstate' isdefined.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param	pinstate	The pinstate.
 *
 * \return	true if it succeeds, false if it fails.
 **************************************************************************************************/

__INLINE__ bool
isdefined ( STATE pinstate )
{
	return pinstate != SP_UNDEFINED;
}

/**********************************************************************************************//**
 * \fn	__INLINE__ bool ishighlow ( STATE pinstate )
 *
 * \brief	Query if 'pinstate' ishighlow.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param	pinstate	The pinstate.
 *
 * \return	true if it succeeds, false if it fails.
 **************************************************************************************************/

__INLINE__ bool
ishighlow ( STATE pinstate )
{
	return pinstate & 1;
}

/**********************************************************************************************//**
 * \fn	__INLINE__ INT polarity ( STATE pinstate )
 *
 * \brief	Polarities the given pinstate.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param	pinstate	The pinstate.
 *
 * \return	An INT.
 **************************************************************************************************/

__INLINE__ INT
polarity ( STATE pinstate )
{
	return pinstate & SP_MASK;
}

/**********************************************************************************************//**
 * \fn	__INLINE__ INT strength ( STATE pinstate )
 *
 * \brief	Strengths the given pinstate.
 *
 * \author	Pugnator
 * \date	11/22/2015
 *
 * \param	pinstate	The pinstate.
 *
 * \return	An INT.
 **************************************************************************************************/

__INLINE__ INT
strength ( STATE pinstate )
{
	return pinstate & SS_MASK;
}
