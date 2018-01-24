/**
 * @author Greg Rowe <greg.rowe@ascending-edge.com>
 * 
 * Copyright (C) 2018 Ascending Edge, LLC - All Rights Reserved
 */
#ifndef _AE_STR_H
#define _AE_STR_H

#include <stdarg.h>
#include <stddef.h>


/**
 * A simple way to copy a string.
 */
#define AE_STR_CP(dest, src)                    \
     strncpy((dest), (src), sizeof(dest))


/** 
 * A simple way to do a formatted print to a string.
 *
 * @param buf where to print to
 * @param fmt printf style format string
 * @param args arguments for @p fmt
 */
#define AE_STR_PRINTV(buf, fmt, args)                 \
     ae_str_printv((buf),(sizeof(buf)), fmt, args)


/** 
 * A simple way to do a formatted print to a string.
 *
 * @param buf where to print to
 * @param fmt printf style format string
 */
#define AE_STR_PRINTF(buf, fmt, ...)                          \
     ae_str_printf((buf),(sizeof(buf)), fmt, __VA_ARGS__)


#ifdef __cplusplus
extern "C" {
#endif

     /** 
      * A simple way to do a formatted print to a string.  If you are
      * using an array for @p buf then consider using #AE_STR_PRINTF
      * as it will automatically take care of the @p buf_len field.
      *
      * @param buf where to print to
      *
      * @param buf_len the length of @p buf
      * 
      * @param fmt printf style format string
      *
      * @return the length of the formatted string
      */
     size_t ae_str_printf(char *buf, size_t buf_len, const char *fmt, ...)
#if __GNUC__
          __attribute__ ((format (printf, 3, 4)))
#endif    
          ;

     /** 
      * A simple way to do a formatted print to a string.  If you are
      * using an array for @p buf then consider using #AE_STR_PRINTV
      * as it will automatically take care of the @p buf_len field.
      *
      * @param buf where to print to
      * 
      * @param buf_len the length of @p
      *
      * @param fmt printf style format string
      *
      * @return the length of the formatted string
      */
     size_t ae_str_printv(char *buf,
                          size_t buf_len,
                          const char *fmt,
                          va_list args);



#ifdef __cplusplus
}
#endif


#endif
