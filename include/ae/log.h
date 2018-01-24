/**
 * @author Greg Rowe <greg.rowe@ascending-edge.com>
 * 
 * Copyright (C) 2018 Ascending Edge, LLC - All Rights Reserved
 */
#ifndef _AE_LOG_H
#define _AE_LOG_H

#include <stdarg.h>
#include <pthread.h>

#include <ae/res.h>


/**
 * 5 available logging levels.
 */
typedef enum ae_log_level
{
     AE_LOG_LEVEL_TRACE = (1 << 0), /**< verbose, program flow
                                     * (function enter/exit) */
     AE_LOG_LEVEL_DEBUG = (1 << 1), /**< pertinent to a developer */
     AE_LOG_LEVEL_INFO = (1 << 2), /**< good to see in a log file in
                                    * normal operations. */
     AE_LOG_LEVEL_WARN = (1 << 3), /**< bad but not critically bad */
     AE_LOG_LEVEL_ERR = (1 << 4), /**< critically bad */
} ae_log_level_t;


/** 
 * If a log handler is registered it must look like this.
 *
 * @param ctx context pointer registered with the logger
 *
 * @param lvl the severity of the log @p msg
 *
 * @param msg the logged message
 */
typedef void (*ae_log_handler_t)(void *ctx, ae_log_level_t lvl, const char *msg);


/**
 * Logger class
 */
typedef struct ae_log
{
     char buf[4000];            /**< log messages are written here
                                 * before being output.  This exists
                                 * to avoid having a large stack frame
                                 * for every single log call. */
     ae_log_level_t mask;       /**< dictates which messages are
                                 * output */
     pthread_mutex_t lock;      /**< the logger must have a lock due
                                 * to the 'buf' field in this
                                 * class. */
     ae_log_handler_t handler;  /**< an optional log message
                                 * handler */
     void *ctx;                 /**< arbitrary context to send to
                                 * 'handler' */
} ae_log_t;

#ifdef __cplusplus
extern "C" {
#endif
     /**
      * When the AE_L* macros are used for logging they default to
      * this logger.
      */
     ae_log_t *g_ae_logger;

     
     /**
      * This exists as a matter of convenience.  g_ae_logger is
      * initialized to point to this.  This makes it easy to start
      * logging early.
      */
     ae_log_t g_ae_log;


     /**
      * Log a result object.  A result object can hold multiple info,
      * error, and warning messages.  This convenience macros will log
      * the three message types with the corresponding log level.
      *
      * #AE_LR is short-hand for #AE_LOG_RES
      */
#    define AE_LR AE_LOG_RES     
#    define AE_LOG_RES(res)                   \
     ae_log_res(g_ae_logger, (res))


     /**
      * Log at any severity level.  This expects a printf style
      * argument list.
      */
#    define AE_LOG(lvl, ...)                                            \
     if(g_ae_logger->mask & lvl) ae_log(g_ae_logger, lvl, __VA_ARGS__)


     /**
      * Logs a message at the TRACE level.  This expects a printf
      * style argument list.
      *
      * #AE_LT is short-hand for #AE_LOG_TRACE
      */
#    define AE_LT AE_LOG_TRACE
#    define AE_LOG_TRACE(...)              \
     AE_LOG(AE_LOG_LEVEL_TRACE, __VA_ARGS__)


     /**
      * Logs a message at the DEBUG level.  This expects a printf
      * style argument list.
      *
      * #AE_LD is short-hand for #AE_LOG_DEBUG
      */
#    define AE_LD AE_LOG_DEBUG
#    define AE_LOG_DEBUG(...)                   \
     AE_LOG(AE_LOG_LEVEL_DEBUG, __VA_ARGS__)


     /**
      * Logs a message at the INFO level.  This expects a printf style
      * argument list.
      *
      * #AE_LI is short-hand for #AE_LOG_INFO
      */
#    define AE_LI AE_LOG_INFO     
#    define AE_LOG_INFO(...)                 \
     AE_LOG(AE_LOG_LEVEL_INFO, __VA_ARGS__)


     /**
      * Logs a message at the WARN level.  This expects a printf style
      * argument list.
      *
      * #AE_LW is short-hand for #AE_LOG_WARN
      */
#    define AE_LW AE_LOG_WARN
#    define AE_LOG_WARN(...)                    \
     AE_LOG(AE_LOG_LEVEL_WARN, __VA_ARGS__)


     /**
      * Logs a message at the ERR level.  This expects a printf style
      * argument list.
      *
      * #AE_LE is short-hand for #AE_LOG_ERR
      */
#    define AE_LE AE_LOG_ERR
#    define AE_LOG_ERR(...)                     \
     AE_LOG(AE_LOG_LEVEL_ERR, __VA_ARGS__)


     /** 
      * Logs a message using a variable argument list.  Normally the
      * AE_L* macros are more convenient but, if you require multiple
      * loggers, this will be required.
      *
      * @param lvl severity
      * @param fmt printf style format string
      */
     void ae_log(ae_log_t *self, ae_log_level_t lvl, const char *fmt, ...)
#if __GNUC__
          __attribute__ ((format (printf, 3, 4)))
#endif    
          ;


     /** 
      * Logs a message using a va_list.  Normally the AE_L* macros are
      * more convenient but in some cases this function is required.
      *
      * @param lvl severity
      * @param fmt printf style format string
      * @param args argument list to match @p fmt
      */
     void ae_logv(ae_log_t *self,
                  ae_log_level_t lvl,
                  const char *fmt,
                  va_list args);


     /** 
      * This convenience function logs an ae_res_t result object.
      * Each info, warning, and error message in the @p res is logged
      * at the corresponding severity level.
      *
      * @see #AE_LOG_RES
      * 
      * @see #AE_LR
      *
      * @param res what to log
      */
     void ae_log_res(ae_log_t *self, const ae_res_t *res);


     /** 
      * Converts a log level to a human readable string.
      *
      * @param lvl the level to convert
      *
      * @return @p level as a string
      */
     const char* ae_log_level_to_string(ae_log_level_t lvl);


     /** 
      * This converts an ae_log_level_t type to the corresponding
      * syslog logging level.
      *
      * @param lvl what to convert
      *
      * @return see description
      */
     int ae_log_level_to_syslog(ae_log_level_t lvl);

#ifdef __cplusplus
}
#endif


#endif
