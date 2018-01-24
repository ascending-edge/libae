/**
 * @author Greg Rowe <greg.rowe@ascending-edge.com>
 * 
 * Copyright (C) 2018 Ascending Edge, LLC - All Rights Reserved
 */
#ifndef _AE_RES_H
#define _AE_RES_H

#include <ae/types.h>
#include <stdarg.h>

/* this is done as a convenience since strerror(errno) is almost
 * always used with the result objects. */
#include <string.h>
#include <errno.h>

/**
 * The various result types.
 */
typedef enum ae_res_n
{
	AE_RES_N_OK = 0,           /**< a-ok */
	AE_RES_N_ERR = 1,          /**< very bad */
	AE_RES_N_WARN = 2,         /**< probably bad */
	AE_RES_N_INFO = 3,         /**< not a problem */

     AE_RES_N_ENDMARKER
} ae_res_n_t;


/**
 * This is used internally, one for each result type (err, warn,
 * info).
 */
typedef struct ae_res_msg
{
     /**< number of messages in msg */
	uint8_t count;
     /**< used internally for where to write */
	char *head;
     /**< holds the messages */     
	char msg[4000];
} ae_res_msg_t;


/**
 * A result object makes it easy to return meaningful error messages
 * to a caller.  They are used extensively in libae.  Typically one
 * result object is created for a thread or call loop and the same
 * result object is passed through the call stack.
 */
typedef struct ae_res
{
     uint8_t mask;              /**< keeps track of what type of
                                 * messages have been logged.  this is
                                 * a bit mask where bit 0 is err, 1 is
                                 * warn, and 2 is info */
	ae_res_msg_t msg[AE_RES_N_ENDMARKER]; /**< This holds the
                                            * messages */
} ae_res_t;



#ifdef __cplusplus
extern "C" {
#endif

     /** 
      * Prepares a result object for use.  This must be called before
      * anything else.  Use #ae_res_clear if you just want to clear
      * out a result object.
      */
     void ae_res_init(ae_res_t *self);

     /** 
      * This removes all messages from the object.  
      */
     void ae_res_clear(ae_res_t *self);

     
     /** 
      * This adds a message to the result.  Usually it is easier to
      * use #ae_res_info, #ae_res_warn, or #aeres_err macros.
      *
      * @param type the kind of message
      * @param fmt printf style format string
      * @param args arguments matching @p fmt
      */
     void ae_res_addv(ae_res_t *self, ae_res_n_t type,
                      const char *fmt, va_list args);


     /** 
      * This adds a message to the result.  Usually it is easier to
      * use #ae_res_info, #ae_res_warn, or #aeres_err macros.
      *
      * @param type the kind of message
      * @param fmt printf style format string
      */
     void ae_res_add(ae_res_t *self, ae_res_n_t type, const char *fmt, ...)
#if __GNUC__
          __attribute__ ((format (printf, 3, 4)))
#endif    
          ;


     /**
      * A convenience macro for adding an info message to the result.
      * This expects a printf style argument list.
      */
#    define ae_res_info(self, ...) \
     ae_res_add((self), AE_RES_N_INFO, __VA_ARGS__)


     /**
      * A convenience macro for adding a warning message to the
      * result.  This expects a printf style argument list.
      */
#    define ae_res_warn(self, ...)                     \
     ae_res_add((self), AE_RES_N_WARN, __VA_ARGS__)


     /**
      * A convenience macro for adding an error message to the result.
      * This expects a printf style argument list.
      */
#    define ae_res_err(self, ...)                      \
     ae_res_add((self), AE_RES_N_ERR, __VA_ARGS__)

     
#ifdef __cplusplus
}
#endif


#endif
