#ifndef API_WRAPPER_H
#define API_WRAPPER_H
#include <vsm_api.h>

#define print_popup(popup,fmt,args ...) popup->vtable->print(popup,fmt, ## args)

extern int32_t popup_id;

BOOL is_pin_active ( IDSIMPIN* pin );
BOOL is_pin_floating ( IDSIMPIN* pin );
BOOL is_pin_high ( IDSIMPIN* pin );
BOOL is_pin_low ( IDSIMPIN* pin );
BOOL is_pin_steady ( IDSIMPIN* pin );
BOOL vsm_register ( ILICENCESERVER* ils );
CHAR* get_image_filename ( CHAR* field_name );
IDSIMPIN* get_pin ( char* pin_name );
STATE get_pin_state ( IDSIMPIN* pin );
void delete_popup ( POPUPID id );
void load_image ( CHAR* filename, uint8_t* buffer, size_t buffer_size );
void out_error ( const char* format, ... );
void out_log ( const char* format, ... );
void out_message ( const char* format, ... );
void out_warning ( const char* format, ... );
void set_callback ( ABSTIME picotime, EVENTID id );
void set_pin_state ( VSM_PIN pin, STATE state );
void set_popup_memory ( IMEMORYPOPUP* popup, size_t offset, void* buffer, size_t size );
void print_to_debug_popup (IDEBUGPOPUP *popup, const char *message);
void dump_to_debug_popup (IDEBUGPOPUP *popup, const uint8_t *buf, uint32_t size, int32_t base);
void toggle_pin_state ( VSM_PIN pin );
IDEBUGPOPUP* create_debug_popup ( const char* title, const int32_t id );
IDEBUGPOPUP* create_source_popup ( const char* title, const int32_t id );
IDEBUGPOPUP* create_status_popup ( const char* title, const int32_t id );
IDEBUGPOPUP* create_var_popup ( const char* title, const int32_t id );
IMEMORYPOPUP* create_memory_popup ( const char* title, const int32_t id );
IPOPUP* create_popup ( CREATEPOPUPSTRUCT* cps );

inline BOOL iscontention ( STATE s );
inline BOOL isdefined ( STATE s );
inline BOOL isfloating ( STATE s );
inline BOOL ishigh ( STATE s );
inline BOOL ishighlow ( STATE s );
inline BOOL islow ( STATE s );
inline INT  polarity ( STATE s );
inline INT  strength ( STATE s );
#endif
