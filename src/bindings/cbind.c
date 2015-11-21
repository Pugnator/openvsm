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

/**
 * @brief [Convert pin state to string]
 *
 * @param pinstate [pin state]
 * @return [string representation of pin state]
 */
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

/**
 * [Register model to Proteus license server]
 * @param  ils [description]
 * @return     [true on success]
 */
bool
vsm_register ( ILICENCESERVER* ils )
{

	if ( FALSE ==  ils->vtable->authorize ( ils, 0, model_key, VSM_API_VERSION ) )
	{
		return FALSE;
	}
	return TRUE;
}

/**
 * [set_pin_state  description]
 * @param pin   [description]
 * @param state [description]
 */
void set_pin_state ( IDSIMMODEL* model, VSM_PIN pin, STATE state )
{
	ABSTIME curtime = 0;
	systime ( model, &curtime );
	pin.pin->vtable->setstate2 ( pin.pin, 0, curtime, pin.on_time, state );
}

/**
 * [set_pin_bool  description]
 * @param pin   [description]
 * @param level [description]
 */
void set_pin_bool ( IDSIMMODEL* model, VSM_PIN pin, bool level )
{
	ABSTIME curtime = 0;
	systime ( model, &curtime );
	pin.pin->vtable->setstate2 ( pin.pin, 0, curtime, pin.on_time, level ? LOGIC_HI : LOGIC_LO );
}

/**
 * [systime  description]
 * @param at [description]
 */
void systime ( IDSIMMODEL* model, ABSTIME* at )
{
	model->model_dsim->vtable->sysvar ( model->model_dsim, 0, ( DOUBLE* ) at, DSIMTIMENOW );
}


/**
 * [get_string_param  description]
 * @param  field_name [description]
 * @return            [description]
 */
char* get_device_id ( IDSIMMODEL* model )
{
	return model->model_instance->vtable->id ( model->model_instance, 0 );
}

/**
 * [get_string_param  description]
 * @param  field_name [description]
 * @return            [description]
 */
char* get_string_param ( IDSIMMODEL* model, char* field_name )
{
	return model->model_instance->vtable->getstrval ( model->model_instance, 0, field_name, "?" );
}

/**
 * [get_bool_param  description]
 * @param  field_name [description]
 * @return            [description]
 */
bool get_bool_param ( IDSIMMODEL* model, char* field_name )
{
	return model->model_instance->vtable->getboolval ( model->model_instance, 0, field_name, FALSE );
}

/**
 * [get_num_param  description]
 * @param  field_name [description]
 * @return            [description]
 */
double get_num_param ( IDSIMMODEL* model, char* field_name )
{
	double result = 0;
	model->model_instance->vtable->getnumval ( model->model_instance, 0, &result, field_name, 0.0 );
	return result;
}

/**
 * [get_hex_param  description]
 * @param  field_name [description]
 * @return            [description]
 */
int32_t get_hex_param ( IDSIMMODEL* model, char* field_name )
{
	return ( int32_t ) model->model_instance->vtable->gethexval ( model->model_instance, 0, field_name, 0 );
}

/**
 * [get_init_param  description]
 * @param  field_name [description]
 * @return            [description]
 */
int64_t get_init_param ( IDSIMMODEL* model, char* field_name )
{
	return ( int64_t ) model->model_instance->vtable->getinitval ( model->model_instance, 0, field_name, 0 );
}

/**
 * [load_image  description]
 * @param filename    [description]
 * @param buffer      [description]
 * @param buffer_size [description]
 */
void load_image ( IDSIMMODEL* model, char* filename, uint8_t* buffer, size_t buffer_size )
{
	model->model_instance->vtable->loadmemory ( model->model_instance, 0, filename, buffer, buffer_size, 0, 0 );
}

/**
 * [create_popup  description]
 * @param  cps [description]
 * @return     [description]
 */
IPOPUP* create_popup ( IDSIMMODEL* model, CREATEPOPUPSTRUCT* cps )
{
	return ( ( IPOPUP* ) model->model_instance->vtable->createpopup ( model->model_instance, 0, cps ) );
}

/**
 * [create_memory_popup  description]
 * @param  title [description]
 * @param  id    [description]
 * @return       [description]
 */
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

/**
 * [create_debug_popup  description]
 * @param  title [description]
 * @param  id    [description]
 * @return       [description]
 */
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

/**
 * [create_source_popup  description]
 * @param  title [description]
 * @param  id    [description]
 * @return       [description]
 */
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

/**
 * [create_status_popup  description]
 * @param  title [description]
 * @param  id    [description]
 * @return       [description]
 */
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

/**
 * [create_var_popup  description]
 * @param  title [description]
 * @param  id    [description]
 * @return       [description]
 */
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

/**
 * [delete_popup  description]
 * @param id [description]
 */
void delete_popup ( IDSIMMODEL* model, POPUPID id )
{
	model->model_instance->vtable->deletepopup ( model->model_instance, 0, id );
}

/**
 * [set_memory_popup  description]
 * @param popup  [description]
 * @param offset [description]
 * @param buffer [description]
 * @param size   [description]
 */
void set_memory_popup ( IMEMORYPOPUP* popup, size_t offset, void* buffer, size_t size )
{
	popup->vtable->setmemory ( popup, 0, offset, buffer, size );
}

/**
 * [add_source_file  description]
 * @param  popup    [description]
 * @param  filename [description]
 * @param  lowlevel [description]
 * @return          [description]
 */
bool add_source_file ( ISOURCEPOPUP* popup, char* filename, bool lowlevel )
{
	bool result = popup->vtable->addsrcfile ( popup, 0, filename, lowlevel );
	popup->vtable->setpcaddress ( popup, 0, 0 );
	return result;
}

/**
 * [set_vdm_handler  description]
 * @return  [description]
 */
bool set_vdm_handler ( IDSIMMODEL* model )
{
	return model->model_instance->vtable->setvdmhlr ( model->model_instance, 0, &ICPU_DEVICE );
}

/**
 * [set_pc_address  description]
 * @param popup   [description]
 * @param address [description]
 */
void set_pc_address ( ISOURCEPOPUP* popup, size_t address )
{
	popup->vtable->setpcaddress ( popup, 0, address );
}

/**
 * [repaint_memory_popup  description]
 * @param popup [description]
 */
void repaint_memory_popup ( IMEMORYPOPUP* popup )
{
	popup->vtable->repaint ( popup, 0 );
}

/**
 * [print_to_debug_popup  description]
 * @param popup   [description]
 * @param message [description]
 */
void print_to_debug_popup ( IDEBUGPOPUP* popup, const char* message )
{
	popup->vtable->print ( popup, ( char* ) message );
}

/**
 * [dump_to_debug_popup  description]
 * @param popup  [description]
 * @param buf    [description]
 * @param offset [description]
 * @param size   [description]
 */
void dump_to_debug_popup ( IDEBUGPOPUP* popup, const uint8_t* buf, uint32_t offset, uint32_t size )
{
	popup->vtable->dump ( popup, 0, buf + offset, size, 16 );
}


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

/**
 * [toggle_pin_state  description]
 * @param pin [description]
 */
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

/**
 * [get_pin_state  description]
 * @param  pin [description]
 * @return     [description]
 */
STATE get_pin_state ( IDSIMPIN* pin )
{
	return pin->vtable->getstate ( pin, 0 );
}

/**
 * [get_pin_bool  description]
 * @param  pin [description]
 * @return     [description]
 */
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

/**
 * [is_pin_active  description]
 * @param  pin [description]
 * @return     [description]
 */
bool is_pin_active ( IDSIMPIN* pin )
{
	return pin->vtable->isactive ( pin, 0 );
}

/**
 * [is_pin_active  description]
 * @param  pin [description]
 * @return     [description]
 */
bool is_pin_inactive ( IDSIMPIN* pin )
{
	return pin->vtable->isinactive ( pin, 0 );
}

/**
 * [is_pin_posedge  description]
 * @param  pin [description]
 * @return     [description]
 */
bool is_pin_posedge ( IDSIMPIN* pin )
{
	return pin->vtable->isposedge ( pin, 0 );
}

/**
 * [is_pin_negedge  description]
 * @param  pin [description]
 * @return     [description]
 */
bool is_pin_negedge ( IDSIMPIN* pin )
{
	return pin->vtable->isnegedge ( pin, 0 );
}

/**
 * [is_pin_edge  description]
 * @param  pin [description]
 * @return     [description]
 */
bool is_pin_edge ( IDSIMPIN* pin )
{
	return pin->vtable->isedge ( pin, 0 );
}

/**
 * [set_callback  Set the system callback]
 * @param picotime [Time callback should be triggered on]
 * @param id       [unique event id]
 */
void set_callback ( IDSIMMODEL* model, RELTIME picotime, EVENTID id )
{
	/* Add event to the model's hash table in order to keep count of registered events */
	model->model_dsim->vtable->setcallback ( model->model_dsim, 0, picotime, model, id );
}

/**
 * [out_log  description]
 * @param format [description]
 */
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

/**
 * [out_message  description]
 * @param format [description]
 */
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

/**
 * [out_warning  description]
 * @param format [description]
 */
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

/**
 * [out_error  description]
 * @param format [description]
 */
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

/**
 * [get_pin  description]
 * @param  pin_name [description]
 * @return          [description]
 */
IDSIMPIN* get_pin ( IDSIMMODEL* model, char* pin_name )
{
	return model->model_instance->vtable->getdsimpin ( model->model_instance, 0, pin_name, TRUE );
}

/**
 * [is_pin_low  description]
 * @param  pin [description]
 * @return     [description]
 */
bool is_pin_low ( IDSIMPIN* pin )
{
	return islow ( pin->vtable->istate ( pin, 0 ) );
}

/**
 * [is_pin_high  description]
 * @param  pin [description]
 * @return     [description]
 */
bool is_pin_high ( IDSIMPIN* pin )
{
	return ishigh ( pin->vtable->istate ( pin, 0 ) );
}

/**
 * [is_pin_floating  description]
 * @param  pin [description]
 * @return     [description]
 */
bool is_pin_floating ( IDSIMPIN* pin )
{
	return isfloating ( pin->vtable->istate ( pin, 0 ) );
}

/**
 * [is_pin_steady  description]
 * @param  pin [description]
 * @return     [description]
 */
bool is_pin_steady ( IDSIMPIN* pin )
{
	return pin->vtable->issteady ( pin, 0 );
}

/**
 * [is_pin_inverted  description]
 * @param  pin [description]
 * @return     [description]
 */
bool is_pin_inverted ( IDSIMPIN* pin )
{
	return pin->vtable->invert ( pin, 0 );
}

/**
 * [islow  description]
 * @param  pinstate [description]
 * @return   [description]
 */
__INLINE__ bool islow ( STATE pinstate )
{
	return ( pinstate & SP_MASK ) == SP_LOW;
}
__INLINE__ bool
ishigh ( STATE pinstate )
{
	return ( pinstate & SP_MASK ) == SP_HIGH;
}
__INLINE__ bool
isfloating ( STATE pinstate )
{
	return ( pinstate & SP_MASK ) == SP_FLOAT;
}
__INLINE__ bool
iscontention ( STATE pinstate )
{
	return pinstate & SF_CONTENTION;
}
__INLINE__ bool
isdefined ( STATE pinstate )
{
	return pinstate != SP_UNDEFINED;
}
__INLINE__ bool
ishighlow ( STATE pinstate )
{
	return pinstate & 1;
}
__INLINE__ INT
polarity ( STATE pinstate )
{
	return pinstate & SP_MASK;
}
__INLINE__ INT
strength ( STATE pinstate )
{
	return pinstate & SS_MASK;
}
