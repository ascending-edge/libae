#ifndef _AE_H
#define _AE_H

#include <ae/log.h>
#include <ae/res.h>
#include <ae/str.h>
#include <ae/pool.h>
#include <ae/ptrarray.h>
#include <ae/buf.h>
#include <ae/types.h>
#include <ae/event.h>
#include <ae/try.h>
#include <ae/misc.h>

#ifdef __cplusplus
extern "C" {
#endif

     bool ae_global_init(ae_res_t *e,
                         ae_log_handler_t log_handler,
                         void *log_ctx);
     bool ae_global_uninit(ae_res_t *e);

#ifdef __cplusplus
}
#endif


#endif
