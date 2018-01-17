#include <ae/log.h>

#include <time.h>
#include <syslog.h>
#include <stdio.h>
          

ae_log_t g_ae_log =
{
     .mask = 0xff,
     .lock = PTHREAD_MUTEX_INITIALIZER,
     .handler = NULL,
     .ctx = NULL
};

ae_log_t *g_ae_logger = &g_ae_log;


void ae_logv(ae_log_t *self,
             ae_log_level_t lvl,
             const char *fmt,
             va_list args)
{
     if(!(self->mask & lvl))
     {
          return;
     }

     pthread_mutex_lock(&self->lock);
     char *out = self->buf;
     size_t len = sizeof(self->buf) -1;
     out[len] = '\0';

     /* turn the level into a string and put in the message */
     int vres = snprintf(out, len, "%s ", ae_log_level_to_string(lvl));
     if((vres > 0)
        && ((size_t)vres <= len))
     {
          len -= vres;
          out += vres;
     }
    
     /* put the actual message into the buffer */
     vres = vsnprintf(out, len, fmt, args);
     if((vres > 0)
        && ((size_t)vres <= len))
     {
          len -= vres;
          out += vres;
     }

     
     if(self->handler)
     {
          self->handler(self->ctx, lvl, self->buf);
          pthread_mutex_unlock(&self->lock);
          return;
     }

     /* No handler, so use syslog */
     syslog(ae_log_level_to_syslog(lvl), "%s", self->buf);
     pthread_mutex_unlock(&self->lock);
}


void ae_log(ae_log_t *self, ae_log_level_t lvl, const char *fmt, ...)
{
     if(!(self->mask & lvl))
     {
          return;
     }

     va_list args;
     va_start(args, fmt);
     ae_logv(self, lvl, fmt, args);
     va_end(args);
}



const char* ae_log_level_to_string(ae_log_level_t lvl)
{
     switch(lvl)
     {
     case AE_LOG_LEVEL_TRACE:
          return "TRACE";
          break;
     case AE_LOG_LEVEL_DEBUG:
          return "DEBUG";          
          break;
     case AE_LOG_LEVEL_INFO:
          return "INFO ";
          break;
     case AE_LOG_LEVEL_WARN:
          return "WARN ";
          break;
     case AE_LOG_LEVEL_ERR:
          return "ERROR";
          break;
     default:
          break;
     }
     return "***invalid***";
}

int ae_log_level_to_syslog(ae_log_level_t lvl)
{
     switch(lvl)
     {
     case AE_LOG_LEVEL_TRACE:
          return LOG_WARNING;
          break;
     case AE_LOG_LEVEL_DEBUG:
          return LOG_DEBUG;
          break;
     case AE_LOG_LEVEL_INFO:
          return LOG_INFO;
          break;
     case AE_LOG_LEVEL_WARN:
          return LOG_WARNING;
          break;
     case AE_LOG_LEVEL_ERR:
          return LOG_ERR;
          break;
     default:
          break;
     }
     return LOG_CRIT;
}


void ae_log_res(ae_log_t *self, const ae_res_t *res)
{
     if(res->msg[AE_RES_N_INFO].count)
     {
          ae_log(self, AE_LOG_LEVEL_INFO, "{%s}",
                 res->msg[AE_RES_N_INFO].msg);
     }
     if(res->msg[AE_RES_N_WARN].count)
     {
          ae_log(self, AE_LOG_LEVEL_WARN, "{%s}",
                 res->msg[AE_RES_N_WARN].msg);
     }
     if(res->msg[AE_RES_N_ERR].count)
     {
          ae_log(self, AE_LOG_LEVEL_ERR, "{%s}",
                 res->msg[AE_RES_N_ERR].msg);
     }
}
