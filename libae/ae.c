#include <ae/ae.h>


ae_res_t ae_global_init(ae_note_t *e,
                        ae_log_handler_t log_handler,
                        void *log_ctx)
{
     g_ae_logger->handler = log_handler;
     g_ae_logger->ctx = log_ctx;
     return 0;
}

ae_res_t ae_global_uninit(ae_note_t *e)
{
     return 0;
}

