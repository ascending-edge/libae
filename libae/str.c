#include <ae/str.h>

#include <stdio.h>


size_t ae_str_printv(char *buf, size_t bufLen, const char *fmt, va_list args)
{
     *buf = '\0';
     int res = vsnprintf(buf, bufLen, fmt, args);
     /* ensure that the string is always terminated.
      * If vsnprintf would overwrite it returns the number of
      * characters that WOULD have been written.  */
     if((res < 0)
        || ((size_t)res >= bufLen))
     {
          buf[bufLen -1] = '\0';
          res = 0;
     }
     return res;
}

size_t ae_str_printf(char *buf, size_t bufLen, const char *fmt, ...)
{
     va_list args;
     va_start(args, fmt);
     int res = ae_str_printv(buf, bufLen, fmt, args);
     va_end(args);
     return res;
}
