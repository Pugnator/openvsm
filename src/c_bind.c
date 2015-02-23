/**
 *
 * @file   c_bind.c
 * @Author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @brief  A set of C-wrappers functions to make life easier
 *
 * This file is part of OpenVSM.
 * OpenVSM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenVSM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenVSM.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <vsm_api.h>

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

int popup_id = 0; //!< Global pop identificator. Should be unique

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
	//print_info(model, "set_pin_bool: %S", level ? "1" : "0");
	ABSTIME curtime = 0;
	systime ( model, &curtime );
	pin.pin->vtable->setstate2 ( pin.pin, 0, curtime, pin.on_time, level ? SHI : SLO );
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

/**
 * [toggle_pin_state  description]
 * @param pin [description]
 */
void toggle_pin_state ( IDSIMMODEL* model, VSM_PIN pin )
{
	STATE pinstate = get_pin_state ( pin.pin );
	if ( SHI == pinstate )
	{
		set_pin_state ( model, pin, SLO );
	}
	else if ( SLO == pinstate )
	{
		set_pin_state ( model,pin, SHI );
	}
	else
	{
		set_pin_state ( model,pin, FLT );
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
inline int32_t get_pin_bool ( VSM_PIN pin )
{
	STATE pinstate = get_pin_state ( pin.pin );
	if ( SLO == pinstate || WLO == pinstate || ILO == pinstate || PLO == pinstate )
	{
		return 0;
	}
	else if ( SHI == pinstate || WHI == pinstate || IHI == pinstate || PHI == pinstate )
	{
		return 1;
	}
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
	callback_events* new = NULL;
	HASH_FIND_INT ( model->events, &id, new );
	if ( !new )
	{
		new = malloc ( sizeof *new );
		new->id = id;
		HASH_ADD_INT ( model->events, id, new );
	}
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
 * [islow  description]
 * @param  pinstate [description]
 * @return   [description]
 */
inline bool islow ( STATE pinstate )
{
	return ( pinstate & SP_MASK ) == SP_LOW;
}
inline bool
ishigh ( STATE pinstate )
{
	return ( pinstate & SP_MASK ) == SP_HIGH;
}
inline bool
isfloating ( STATE pinstate )
{
	return ( pinstate & SP_MASK ) == SP_FLOAT;
}
inline bool
iscontention ( STATE pinstate )
{
	return pinstate & SF_CONTENTION;
}
inline bool
isdefined ( STATE pinstate )
{
	return pinstate != SP_UNDEFINED;
}
inline bool
ishighlow ( STATE pinstate )
{
	return pinstate & 1;
}
inline INT
polarity ( STATE pinstate )
{
	return pinstate & SP_MASK;
}
inline INT
strength ( STATE pinstate )
{
	return pinstate & SS_MASK;
}
