/**
 *
 * @file   c_bind.h
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @date   22.09.2014
 * @brief  The main C wrapper of official API
 * @copyright Copyright 2014 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the BSD 2-Clause license.
 *
 */

#pragma once

#include <vsmapi.h>

/* DEBUG */

void console_alloc ( const char* title );

/* DEVICE */

void set_logic_type ( IDSIMMODEL* model, LOGIC_TYPE type );
const char* logic_type_to_string ( LOGIC_TYPE type );
bool get_bool_param ( IDSIMMODEL* model, char* field_name );
bool set_vdm_handler ( IDSIMMODEL* model );
bool vsm_register ( ILICENCESERVER* ils );
char* get_device_id ( IDSIMMODEL* model );
char* get_string_param ( IDSIMMODEL* model, char* field_name );
double get_num_param ( IDSIMMODEL* model, char* field_name );
int32_t get_hex_param ( IDSIMMODEL* model, char* field_name );
int64_t get_init_param ( IDSIMMODEL* model, char* field_name );
void load_image ( IDSIMMODEL* model, char* filename, uint8_t* buffer, size_t buffer_size );
void systime ( IDSIMMODEL* model, ABSTIME* at );

/* LOGGING */

void print_error ( IDSIMMODEL* model, const char* format, ... );
void print_info ( IDSIMMODEL* model, const char* format, ... );
void print_message ( IDSIMMODEL* model, const char* format, ... );
void print_warning ( IDSIMMODEL* model, const char* format, ... );
void set_callback ( IDSIMMODEL* model, RELTIME picotime, EVENTID id );
void set_pin_state ( IDSIMMODEL* model, VSM_PIN pin, STATE state );

/* POPUP */

IDEBUGPOPUP* create_debug_popup ( IDSIMMODEL* model, const char* title, const int32_t id );
IDEBUGPOPUP* create_source_popup ( IDSIMMODEL* model, const char* title, const int32_t id );
IDEBUGPOPUP* create_status_popup ( IDSIMMODEL* model, const char* title, const int32_t id );
IDEBUGPOPUP* create_var_popup ( IDSIMMODEL* model, const char* title, const int32_t id );
IMEMORYPOPUP* create_memory_popup ( IDSIMMODEL* model, const char* title, const int32_t id );
IPOPUP* create_popup ( IDSIMMODEL* model, CREATEPOPUPSTRUCT* cps );
bool add_source_file ( ISOURCEPOPUP* popup, char* filename, bool lowlevel );
void delete_popup ( IDSIMMODEL* model, POPUPID id );
void dump_to_debug_popup ( IDEBUGPOPUP* popup, const uint8_t* buf, uint32_t offset, uint32_t size );
void print_to_debug_popup ( IDEBUGPOPUP* popup, const char* message );
void repaint_memory_popup ( IMEMORYPOPUP* popup );
void set_memory_popup ( IMEMORYPOPUP* popup, size_t offset, void* buffer, size_t size );
void set_pc_address ( ISOURCEPOPUP* popup, size_t address );

/* PINS */

__INLINE__ bool iscontention ( STATE s );
__INLINE__ bool isdefined ( STATE s );
__INLINE__ bool isfloating ( STATE s );
__INLINE__ bool ishigh ( STATE s );
__INLINE__ bool ishighlow ( STATE s );
__INLINE__ bool islow ( STATE s );
__INLINE__ INT  polarity ( STATE s );
__INLINE__ INT  strength ( STATE s );

bool is_pin_active ( IDSIMPIN* pin );
bool is_pin_inactive ( IDSIMPIN* pin );
bool is_pin_inverted ( IDSIMPIN* pin );
bool is_pin_posedge ( IDSIMPIN* pin );
bool is_pin_negedge ( IDSIMPIN* pin );
bool is_pin_edge ( IDSIMPIN* pin );
bool is_pin_floating ( IDSIMPIN* pin );
bool is_pin_high ( IDSIMPIN* pin );
bool is_pin_low ( IDSIMPIN* pin );
bool is_pin_steady ( IDSIMPIN* pin );

IDSIMPIN* get_pin ( IDSIMMODEL* model, char* pin_name );
STATE get_pin_state ( IDSIMPIN* pin );
const char* state_to_string ( STATE s );
int32_t get_pin_bool ( VSM_PIN pin );
void set_pin_bool ( IDSIMMODEL* model, VSM_PIN pin, bool level );
void toggle_pin_state ( IDSIMMODEL* model, VSM_PIN pin );
