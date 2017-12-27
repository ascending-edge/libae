#ifndef _AE_STR_H
#define _AE_STR_H

#include <stdarg.h>
#include <stddef.h>

#define AE_STR_CP(dest, src)                    \
     strncpy((dest), (src), sizeof(dest))

#define STR_PRINTV(buf, fmt, args)                 \
     ae_str_printv((buf),(sizeof(buf)), fmt, args)


#define STR_PRINTF(buf, fmt, ...)                          \
     ae_str_printf((buf),(sizeof(buf)), fmt, __VA_ARGS__)


#ifdef __cplusplus
extern "C" {
#endif


     size_t ae_str_printf(char *buf, size_t bufLen, const char *fmt, ...)
#if __GNUC__
          __attribute__ ((format (printf, 3, 4)))
#endif    
          ;

     size_t ae_str_printv(char *buf,
                          size_t bufLen,
                          const char *fmt,
                          va_list args);



#ifdef __cplusplus
}
#endif


#endif
