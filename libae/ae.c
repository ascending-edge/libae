#include <ae/ae.h>
#include <syslog.h>

bool ae_global_init(ae_res_t *e, ae_log_handler_t log_handler, void *log_ctx)
{
     if(!g_ae_logger)
     {
          g_ae_logger = &g_ae_log;
     }
     g_ae_logger->handler = log_handler;
     g_ae_logger->ctx = log_ctx;
     return true;
}

bool ae_global_uninit(ae_res_t *e)
{
     return true;
}

