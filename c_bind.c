/**
 *  
 * @file   c_bind.c
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @brief  A set of C-wrappers functions to make life easier
 * @copyright GNU Public License.
 */

#include <vsm_api.h>

int32_t popup_id = 0; ///< global popup-id counter. Functions use it as unique ID


/*! 
 *  @brief     VSM model registration
 *  @details   This function registers VSM model to license server
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

/*! 
 *  @brief Set VSM_PIN to desired state
 *  @see set_pin_bool
 *  @param [in] pin pin itself
 *  @param [in] state the state needed to be set 
 */
void
set_pin_state ( VSM_PIN pin, STATE state )
{
	ABSTIME curtime = 0;
	systime ( &curtime );
	pin.pin->vtable->setstate2 ( pin.pin, 0, curtime, pin.on_time, state );
}

/*! 
 *  @brief Set VSM_PIN using boolean value
 *  @see set_pin_bool
 *  @param [in] pin pin itself
 *  @param [in] level the state needed to be set 
 */ 
void
set_pin_bool ( VSM_PIN pin, int32_t level )
{
	ABSTIME curtime = 0;
	systime ( &curtime );
	pin.pin->vtable->setstate2 ( pin.pin, 0, curtime, pin.on_time, level ? SHI : SLO );
}

/*! 
 *  @brief Get current system time
 *  @see set_pin_bool
 *  @param [out] absolute simulation time 
 */
void systime ( ABSTIME* at )
{
	model_dsim->vtable->sysvar ( model_dsim, 0, ( DOUBLE* ) at, DSIMTIMENOW );
}


/**
* Get model string parameter (case insensitive)
*/
char*
get_string_param ( char* field_name )
{
	return model_instance->vtable->getstrval ( model_instance, 0, field_name, "?" );
}

bool
get_bool_param ( char* field_name )
{
	return model_instance->vtable->getboolval ( model_instance, 0, field_name, FALSE );
}

double
get_num_param ( char* field_name )
{
	double result = 0;
	model_instance->vtable->getnumval ( model_instance, 0, &result, field_name, 0.0 );
	return result;
}

int32_t
get_hex_param ( char* field_name )
{
	return ( int32_t ) model_instance->vtable->gethexval ( model_instance, 0, field_name, 0 );
}

int64_t
get_init_param ( char* field_name )
{
	return ( int64_t ) model_instance->vtable->getinitval ( model_instance, 0, field_name, 0 );
}

void
load_image ( char* filename, uint8_t* buffer, size_t buffer_size )
{
	model_instance->vtable->loadmemory ( model_instance, 0, filename, buffer, buffer_size, 0, 0 );
}

IPOPUP*
create_popup ( CREATEPOPUPSTRUCT* cps )
{
	return ( ( IPOPUP* ) model_instance->vtable->createpopup ( model_instance, 0, cps ) );
}

IMEMORYPOPUP*
create_memory_popup ( const char* title, const int32_t id )
{
	CREATEPOPUPSTRUCT* cps = malloc ( sizeof *cps );
	cps->caption = ( char* ) title;
	cps->flags = PWF_VISIBLE | PWF_HIDEONANIMATE | PWF_SIZEABLE| PWF_AUTOREFRESH ;
	cps->type = PWT_MEMORY;
	cps->height = 32;
	cps->width = 16;
	cps->id = id;
	IMEMORYPOPUP* popup = ( IMEMORYPOPUP* ) create_popup ( cps );
	free ( cps );
	return popup;
}

IDEBUGPOPUP*
create_debug_popup ( const char* title, const int32_t id )
{
	CREATEPOPUPSTRUCT* cps = malloc ( sizeof *cps );
	cps->caption = ( char* ) title;
	cps->flags = PWF_VISIBLE | PWF_HIDEONANIMATE | PWF_SIZEABLE| PWF_AUTOREFRESH ;
	cps->type = PWT_DEBUG;
	cps->height = 200;
	cps->width = 640;
	cps->id = id;
	IDEBUGPOPUP* popup = create_popup ( cps );
	free ( cps );
	return popup;
}

IDEBUGPOPUP*
create_source_popup ( const char* title, const int32_t id )
{
	CREATEPOPUPSTRUCT* cps = malloc ( sizeof *cps );
	cps->caption = ( char* ) title;
	cps->flags = PWF_VISIBLE | PWF_HIDEONANIMATE | PWF_SIZEABLE| PWF_AUTOREFRESH ;
	cps->type = PWT_SOURCE;
	cps->height = 200;
	cps->width = 640;
	cps->id = id;
	IDEBUGPOPUP* popup = create_popup ( cps );
	free ( cps );
	return popup;
}

IDEBUGPOPUP*
create_status_popup ( const char* title, const int32_t id )
{
	CREATEPOPUPSTRUCT* cps = malloc ( sizeof *cps );
	cps->caption = ( char* ) title;
	cps->flags = PWF_VISIBLE | PWF_HIDEONANIMATE | PWF_SIZEABLE | PWF_AUTOREFRESH ;
	cps->type = PWT_STATUS;
	cps->height = 200;
	cps->width = 200;
	cps->id = id;
	IDEBUGPOPUP* popup = create_popup ( cps );
	free ( cps );
	return popup;
}

IDEBUGPOPUP*
create_var_popup ( const char* title, const int32_t id )
{
	CREATEPOPUPSTRUCT* cps = malloc ( sizeof *cps );
	cps->caption = ( char* ) title;
	cps->flags = PWF_VISIBLE | PWF_HIDEONANIMATE | PWF_SIZEABLE| PWF_AUTOREFRESH ;
	cps->type = PWT_VAR;
	cps->height = 200;
	cps->width = 200;
	cps->id = id;
	IDEBUGPOPUP* popup = create_popup ( cps );
	free ( cps );
	return popup;
}

void
delete_popup ( POPUPID id )
{
	model_instance->vtable->deletepopup ( model_instance, 0, id );
}

void
set_memory_popup ( IMEMORYPOPUP* popup, size_t offset, void* buffer, size_t size )
{
	popup->vtable->setmemory ( popup, 0, offset, buffer, size );
}

bool
add_source_file ( ISOURCEPOPUP* popup, char* filename, bool lowlevel )
{
	bool result = popup->vtable->addsrcfile ( popup, 0, filename, lowlevel );
	popup->vtable->setpcaddress ( popup, 0, 0 );
	return result;
}

bool
set_vdm_handler ( void )
{
	return model_instance->vtable->setvdmhlr ( model_instance, 0, &ICPU_DEVICE );
}

void
set_pc_address ( ISOURCEPOPUP* popup, size_t address )
{
	popup->vtable->setpcaddress ( popup, 0, address );
}

void
repaint_memory_popup ( IMEMORYPOPUP* popup )
{
	popup->vtable->repaint ( popup, 0 );
}

void
print_to_debug_popup ( IDEBUGPOPUP* popup, const char* message )
{
	popup->vtable->print ( popup, ( char* ) message );
}

void
dump_to_debug_popup ( IDEBUGPOPUP* popup, const uint8_t* buf, uint32_t offset, uint32_t size )
{
	popup->vtable->dump ( popup, 0, buf + offset, size, 16 );
}

void
toggle_pin_state ( VSM_PIN pin )
{
	STATE s = get_pin_state ( pin.pin );
	if ( SHI == s )
	{
		set_pin_state ( pin, SLO );
	}
	else if ( SLO == s )
	{
		set_pin_state ( pin, SHI );
	}
}

STATE
get_pin_state ( IDSIMPIN* pin )
{
	return pin->vtable->istate ( pin, 0 );
}

int32_t
get_pin_bool ( VSM_PIN pin )
{
	if ( SLO == pin.pin->vtable->istate ( pin.pin, 0 ) )
		return 0;
	else if ( SHI == pin.pin->vtable->istate ( pin.pin, 0 ) )
		return 1;
	srand ( time ( NULL ) );
	return rand() % 2;
}

bool
is_pin_active ( IDSIMPIN* pin )
{
	return pin->vtable->isactive ( pin, 0 );
}

bool
is_pin_posedge ( IDSIMPIN* pin )
{
	return pin->vtable->isposedge ( pin, 0 );
}

bool
is_pin_negedge ( IDSIMPIN* pin )
{
	return pin->vtable->isnegedge ( pin, 0 );
}

bool
is_pin_edge ( IDSIMPIN* pin )
{
	return pin->vtable->isedge ( pin, 0 );
}

void
set_callback ( RELTIME picotime, EVENTID id )
{
	model_dsim->vtable->setcallback ( model_dsim, 0, picotime, &VSM_DEVICE, id );
}

void
out_log ( const char* format, ... )
{
	char* string;
	va_list args;
	va_start ( args, format );
	if ( 0 > vasprintf ( &string, ( char* ) format, args ) ) string = NULL;
	va_end ( args );
	model_instance->vtable->log ( model_instance, string );
	free ( string );
}

void
out_message ( const char* format, ... )
{
	char* string;
	va_list args;
	va_start ( args, format );
	if ( 0 > vasprintf ( &string, ( char* ) format, args ) ) string = NULL;
	va_end ( args );
	model_instance->vtable->message ( model_instance, string );
	free ( string );
}

void
out_warning ( const char* format, ... )
{
	char* string;
	va_list args;
	va_start ( args, format );
	if ( 0 > vasprintf ( &string, ( char* ) format, args ) ) string = NULL;
	va_end ( args );
	model_instance->vtable->warning ( model_instance, string );
	free ( string );
}

void
out_error ( const char* format, ... )
{
	char* string;
	va_list args;
	va_start ( args, format );
	if ( 0 > vasprintf ( &string, ( char* ) format, args ) ) string = NULL;
	va_end ( args );
	model_instance->vtable->error ( model_instance, string );
	free ( string );
}

IDSIMPIN*
get_pin ( char* pin_name )
{
	return model_instance->vtable->getdsimpin ( model_instance, 0, pin_name, TRUE );
}

bool
is_pin_low ( IDSIMPIN* pin )
{
	return islow ( pin->vtable->istate ( pin, 0 ) );
}

bool
is_pin_high ( IDSIMPIN* pin )
{
	return ishigh ( pin->vtable->istate ( pin, 0 ) );
}

bool
is_pin_floating ( IDSIMPIN* pin )
{
	return isfloating ( pin->vtable->istate ( pin, 0 ) );
}

bool
is_pin_steady ( IDSIMPIN* pin )
{
	return pin->vtable->issteady ( pin, 0 );
}

inline bool
islow ( STATE s )
{
	return ( s & SP_MASK ) == SP_LOW;
}
inline bool
ishigh ( STATE s )
{
	return ( s & SP_MASK ) == SP_HIGH;
}
inline bool
isfloating ( STATE s )
{
	return ( s & SP_MASK ) == SP_FLOAT;
}
inline bool
iscontention ( STATE s )
{
	return s & SF_CONTENTION;
}
inline bool
isdefined ( STATE s )
{
	return s != SP_UNDEFINED;
}
inline bool
ishighlow ( STATE s )
{
	return s & 1;
}
inline INT
polarity ( STATE s )
{
	return s & SP_MASK;
}
inline INT
strength ( STATE s )
{
	return s & SS_MASK;
}

