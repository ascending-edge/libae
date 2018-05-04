/**
 * @author Greg Rowe <greg.rowe@ascending-edge.com>
 *
 * Copyright (C) 2018 Ascending Edge, LLC - All Rights Reserved
 * 
 * Users should be able to include just this header to get all libae
 * definitions.
 */
#ifndef _AE_H
#define _AE_H

#include <ae/event.h>
#include <ae/io.h>
#include <ae/log.h>
#include <ae/misc.h>
#include <ae/mux.h>
#include <ae/opt.h>
#include <ae/pool.h>
#include <ae/ptrarray.h>
#include <ae/res.h>
#include <ae/server.h>
#include <ae/str.h>
#include <ae/time.h>
#include <ae/timer.h>
#include <ae/try.h>
#include <ae/types.h>

#ifdef __GNUC__
#define AE_PRINTF_ARGS(a,b) __attribute__ ((format (printf, a, b)))
#else
#define AE_PRINTF_ARGS(a,b) 
#endif


#ifdef __cplusplus
extern "C" {
#endif

     /** 
      * This performs all required global initialization.
      *
      * @param log_handler function to call for log messages
      * 
      * @param log_ctx context for @p log_handler
      */
     bool ae_global_init(ae_res_t *e,
                         ae_log_handler_t log_handler,
                         void *log_ctx);


     /** 
      * This performs all cleanup.  This is currently an empty
      * implementation.
      */
     bool ae_global_uninit(ae_res_t *e);

#ifdef __cplusplus
}
#endif


#endif
