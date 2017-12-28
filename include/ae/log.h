#ifndef _AE_LOG_H
#define _AE_LOG_H

#include <stdarg.h>
#include <pthread.h>

#include <ae/res.h>

typedef enum ae_log_level
{
     AE_LOG_LEVEL_TRACE = (1 << 0),
     AE_LOG_LEVEL_DEBUG = (1 << 1),
     AE_LOG_LEVEL_INFO = (1 << 2),
     AE_LOG_LEVEL_WARN = (1 << 3),
     AE_LOG_LEVEL_ERR = (1 << 4),
} ae_log_level_t;

typedef void (*ae_log_handler_t)(void *ctx, ae_log_level_t lvl, const char *msg);

typedef struct ae_log
{
     char buf[4000];
     ae_log_level_t mask;
     pthread_mutex_t lock;
     ae_log_handler_t handler;
     void *ctx;
} ae_log_t;

#ifdef __cplusplus
extern "C" {
#endif
     
     ae_log_t *g_ae_logger;
     ae_log_t g_ae_log;

#    define AE_LN AE_LOG_RES     
#    define AE_LOG_RES(res)                   \
     ae_log_res(g_ae_logger, (res))
     
#    define AE_LOG(lvl, ...)                                            \
     if(g_ae_logger->mask & lvl) ae_log(g_ae_logger, lvl, __VA_ARGS__)

#    define AE_LT AE_LOG_TRACE
#    define AE_LOG_TRACE(...)              \
     AE_LOG(AE_LOG_LEVEL_TRACE, __VA_ARGS__)

#    define AE_LD AE_LOG_DEBUG
#    define AE_LOG_DEBUG(...)                   \
     AE_LOG(AE_LOG_LEVEL_DEBUG, __VA_ARGS__)

#    define AE_LI AE_LOG_INFO     
#    define AE_LOG_INFO(...)                 \
     AE_LOG(AE_LOG_LEVEL_INFO, __VA_ARGS__)

#    define AE_LW AE_LOG_WARN
#    define AE_LOG_WARN(...)                    \
     AE_LOG(AE_LOG_LEVEL_WARN, __VA_ARGS__)

#    define AE_LE AE_LOG_ERR
#    define AE_LOG_ERR(...)                     \
     AE_LOG(AE_LOG_LEVEL_ERR, __VA_ARGS__)

     void ae_log(ae_log_t *self, ae_log_level_t lvl, const char *fmt, ...)
#if __GNUC__
          __attribute__ ((format (printf, 3, 4)))
#endif    
          ;

     void ae_logv(ae_log_t *self,
                  ae_log_level_t lvl,
                  const char *fmt,
                  va_list args);

     void ae_log_res(ae_log_t *self, const ae_res_t *res);

     const char* ae_log_level_to_string(ae_log_level_t lvl);
     int ae_log_level_to_syslog(ae_log_level_t lvl);

#ifdef __cplusplus
}
#endif


#endif
