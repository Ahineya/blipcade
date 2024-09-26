#ifndef QUICKJS_COMPAT_H
#define QUICKJS_COMPAT_H

#ifdef __APPLE__
#include <stdarg.h>
#undef va_list
#define va_list __builtin_va_list
#endif

#endif // QUICKJS_COMPAT_H