#include <vsm_api.h>

int32_t popup_id = 0;

BOOL vsm_register (ILICENCESERVER *ils)
{
	CONSOLE_ALLOCATED = FALSE;

	#ifdef _CURSES_ENABLED
	CONSOLE_ALLOCATED = TRUE;
	//debug_console_alloc();
	#endif

	if(FALSE ==  ils->vtable->authorize( ils, 0, model_key, VSM_API_VERSION ) )
	{
		out_log("Device failed to authorize");
		return FALSE;
	}
	return TRUE;
}

void set_pin_state (VSM_PIN pin, STATE state)
{
	pin.pin->vtable->setstate2(pin.pin, 0, pin.on_time, pin.off_time, state);
}

CHAR *get_image_filename (CHAR* field_name)
{
	return model_instance->vtable->getstrval(model_instance, 0, field_name, "?");
}

void load_image (CHAR* filename, uint8_t *buffer, size_t buffer_size)
{
	model_instance->vtable->loadmemory(model_instance, 0, filename, buffer, buffer_size, 0, 0);	
}

IPOPUP *create_popup (CREATEPOPUPSTRUCT *cps)
{
	return ((IPOPUP *)model_instance->vtable->createpopup(model_instance, 0, cps));
}

IMEMORYPOPUP *create_memory_popup (const char *title, const int32_t id)
{
	CREATEPOPUPSTRUCT *cps = malloc(sizeof *cps);
	cps->caption = (char *)title;
	cps->flags = PWF_VISIBLE;
	cps->type = PWT_MEMORY;
	cps->height = 32;
	cps->width = 16;
	cps->id = id;	
	IMEMORYPOPUP *popup = (IMEMORYPOPUP *)create_popup(cps);
	free(cps);
	return popup;
}

IDEBUGPOPUP *create_debug_popup (const char *title, const int32_t id)
{
	CREATEPOPUPSTRUCT *cps = malloc(sizeof *cps);
	cps->caption = (char *)title;
	cps->flags = PWF_VISIBLE;
	cps->type = PWT_DEBUG;
	cps->height = 200;
	cps->width = 200;
	cps->id = id;	
	IDEBUGPOPUP *popup = create_popup(cps);	
	free(cps);	
	return popup;
}

IDEBUGPOPUP *create_source_popup (const char *title, const int32_t id)
{
	CREATEPOPUPSTRUCT *cps = malloc(sizeof *cps);
	cps->caption = (char *)title;
	cps->flags = PWF_VISIBLE;
	cps->type = PWT_SOURCE;
	cps->height = 200;
	cps->width = 200;
	cps->id = id;
	IDEBUGPOPUP *popup = create_popup(cps);
	free(cps);
	return popup;
}

IDEBUGPOPUP *create_status_popup (const char *title, const int32_t id)
{
	CREATEPOPUPSTRUCT *cps = malloc(sizeof *cps);
	cps->caption = (char *)title;
	cps->flags = PWF_VISIBLE;
	cps->type = PWT_STATUS;
	cps->height = 200;
	cps->width = 200;
	cps->id = id;
	IDEBUGPOPUP *popup = create_popup(cps);
	free(cps);
	return popup;
}

IDEBUGPOPUP *create_var_popup (const char *title, const int32_t id)
{
	CREATEPOPUPSTRUCT *cps = malloc(sizeof *cps);
	cps->caption = (char *)title;
	cps->flags = PWF_VISIBLE;
	cps->type = PWT_VAR;
	cps->height = 200;
	cps->width = 200;
	cps->id = id;	
	IDEBUGPOPUP *popup = create_popup(cps);
	free(cps);
	return popup;
}

void delete_popup (POPUPID id)
{
	model_instance->vtable->deletepopup(model_instance, 0, id);
}

void set_popup_memory (IMEMORYPOPUP *popup, size_t offset, void *buffer, size_t size)
{
	popup->vtable->setmemory(popup, 0, offset, buffer, size);
}

void print_to_debug_popup (IDEBUGPOPUP *popup, const char *message)
{
	popup->vtable->print(popup, (char *)message);
}

void dump_to_debug_popup (IDEBUGPOPUP *popup, const uint8_t *buf, uint32_t size, int32_t base)
{
	popup->vtable->dump(popup, 0, buf, size, base);
}

void toggle_pin_state (VSM_PIN pin)
{
	STATE s = get_pin_state(pin.pin);
	if(SHI == s)
	{
		set_pin_state(pin, SLO);
	}
	else if (SLO == s)
	{
		set_pin_state(pin, SHI);
	}
}

STATE get_pin_state (IDSIMPIN *pin)
{
	return pin->vtable->istate(pin, 0);
}

BOOL is_pin_active (IDSIMPIN *pin)
{
	return pin->vtable->isactive(pin, 0);
}

void set_callback (ABSTIME picotime, EVENTID id)
{
	model_dsim->vtable->setclockcallback(model_dsim, 0, picotime, 1000000000000, &VSM_DEVICE, vsm_callback, id);
}

void out_log (const char *format, ...)
{
	char* string;
	va_list args;	
	va_start ( args, format );
	if ( 0 > vasprintf ( &string, (char *)format, args ) ) string = NULL;
	va_end ( args );
	model_instance->vtable->log(model_instance, string);
	free ( string );
}

void out_message (const char *format, ...)
{
	char* string;
	va_list args;	
	va_start ( args, format );
	if ( 0 > vasprintf ( &string, (char *)format, args ) ) string = NULL;
	va_end ( args );
	model_instance->vtable->message(model_instance, string);
	free ( string );
}

void out_warning (const char *format, ...)
{
	char* string;
	va_list args;	
	va_start ( args, format );
	if ( 0 > vasprintf ( &string, (char *)format, args ) ) string = NULL;
	va_end ( args );
	model_instance->vtable->warning(model_instance, string);
	free ( string );
}

void out_error (const char *format, ...)
{
	char* string;
	va_list args;	
	va_start ( args, format );
	if ( 0 > vasprintf ( &string, (char *)format, args ) ) string = NULL;
	va_end ( args );
	model_instance->vtable->error(model_instance, string);
	free ( string );	
}

IDSIMPIN *get_pin (char *pin_name)
{
	return model_instance->vtable->getdsimpin(model_instance, 0, pin_name, TRUE);
}

BOOL is_pin_low (IDSIMPIN *pin)
{
	return islow(pin->vtable->istate(pin, 0));
}

BOOL is_pin_high (IDSIMPIN *pin)
{
	return ishigh(pin->vtable->istate(pin, 0));
}

BOOL is_pin_floating (IDSIMPIN *pin)
{
	return isfloating(pin->vtable->istate(pin, 0));
}

BOOL is_pin_steady (IDSIMPIN *pin)
{
	return pin->vtable->issteady(pin, 0);
}

inline BOOL islow (STATE s)
{
	return (s & SP_MASK) == SP_LOW;
}
inline BOOL ishigh (STATE s)
{
	return (s & SP_MASK) == SP_HIGH;
}
inline BOOL isfloating (STATE s)
{
	return (s & SP_MASK) == SP_FLOAT;
}
inline BOOL iscontention (STATE s)
{
	return s & SF_CONTENTION;
}
inline BOOL isdefined (STATE s)
{
	return s != SP_UNDEFINED;
}
inline BOOL ishighlow (STATE s)
{
	return s & 1;
}
inline INT  polarity (STATE s)
{
	return s & SP_MASK;
}
inline INT  strength (STATE s)
{
	return s & SS_MASK;
}
