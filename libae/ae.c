/**
 * @author Greg Rowe <greg.rowe@ascending-edge.com>
 * 
 * Global libae code belongs here.
 */
#include <ae/ae.h>


bool ae_global_init(ae_res_t *e, ae_log_handler_t log_handler, void *log_ctx)
{
     /* On ARM I save that g_ae_logger wasn't initialized.  This
      * probably has to do with a compiler flag I should set but, as a
      * workaround, I did this. */
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
     /* Just a placeholder for now */
     return true;
}

