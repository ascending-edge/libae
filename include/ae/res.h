#ifndef _AE_RES_H
#define _AE_RES_H

#include <ae/types.h>
#include <stdarg.h>

/* this is done as a convenience since strerror(errno) is almost
 * always used with the result objects. */
#include <string.h>
#include <errno.h>


typedef enum ae_res_n
{
	AE_RES_N_OK = 0,
	AE_RES_N_ERR = 1,
	AE_RES_N_WARN = 2,	
	AE_RES_N_INFO = 3,

     AE_RES_N_ENDMARKER
} ae_res_n_t;

typedef struct ae_res_msg
{
     /**< number of messages in msg */
	uint8_t count;
     /**< used internally for where to write */
	char *head;
     /**< holds the messages */     
	char msg[4000];
} ae_res_msg_t;



typedef struct ae_res
{
     uint8_t mask;
	ae_res_msg_t msg[AE_RES_N_ENDMARKER];
} ae_res_t;



#ifdef __cplusplus
extern "C" {
#endif

     
     
     void ae_res_init(ae_res_t *self);

     void ae_res_addv(ae_res_t *self, ae_res_n_t type, va_list args);
     void ae_res_add(ae_res_t *self, ae_res_n_t type, const char *fmt, ...)
#if __GNUC__
          __attribute__ ((format (printf, 3, 4)))
#endif    
          ;


#    define ae_res_info(self, ...) \
     ae_res_add((self), AE_RES_N_INFO, __VA_ARGS__)
#    define ae_res_warn(self, ...) \
     ae_res_add((self), AE_RES_N_WARN, __VA_ARGS__)
#    define ae_res_err(self, ...) \
     ae_res_add((self), AE_RES_N_ERR, __VA_ARGS__)

     
#ifdef __cplusplus
}
#endif


#endif
