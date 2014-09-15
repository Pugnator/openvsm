#ifndef VSM_TUI_H
#define VSM_TUI_H
#include <vsm_api.h>
void debug_console_alloc(void);
void debug_console_free(void);
void console_log(char *format, ...);
#endif
