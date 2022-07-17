#include <stdarg.h>

wchar_t *utf8_to_wchar(const char *str);

int vasprintf(char **strp, const char *fmt, va_list ap);
int asprintf(char **strp, const char *fmt, ...);
char *get_module_dir(void);
char *get_abspath_from_modpath(const char *fname);