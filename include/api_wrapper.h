#ifndef API_WRAPPER_H
#define API_WRAPPER_H
#include <vsm_api.h>

#define print_popup(popup,fmt,args ...) popup->vtable->print(popup,fmt, ## args)

void set_pin_state (VSM_PIN pin, STATE state);
STATE get_pin_state (IDSIMPIN *pin);
IDSIMPIN *get_pin (char *pin_name);
BOOL is_pin_low (IDSIMPIN *pin);
BOOL is_pin_high (IDSIMPIN *pin);
BOOL is_pin_steady (IDSIMPIN *pin);
BOOL is_pin_floating (IDSIMPIN *pin);
BOOL is_pin_active (IDSIMPIN *pin);
void set_callback (ABSTIME picotime, EVENTID id);
BOOL vsm_register (ILICENCESERVER *ils);
CHAR *get_image_filename (CHAR* field_name);
void load_image (CHAR* filename, uint8_t *buffer, size_t buffer_size);
void out_log (char *text);
void out_message (char *text);
void out_warning (char *text);
void out_error (char *text);
void delete_popup (POPUPID id);
void set_popup_memory (IMEMORYPOPUP *popup, size_t offset, void *buffer, size_t size);
IPOPUP *create_popup (CREATEPOPUPSTRUCT *cps);

inline BOOL islow (STATE s);
inline BOOL ishigh (STATE s);
inline BOOL isfloating (STATE s);
inline BOOL iscontention (STATE s);
inline BOOL isdefined (STATE s);
inline BOOL ishighlow (STATE s);
inline INT  polarity (STATE s);
inline INT  strength (STATE s);
#endif
