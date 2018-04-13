#include <ae/server.h>

void ae_server_uninit(ae_res_t *e, ae_server_t *self)
{
     if((self->fd != -1)
        && (close(self->fd) != 0))
     {
          ae_res_err(e, "close: %s", strerror(errno));
     }
}


bool ae_server_init(ae_res_t *e, ae_server_t *self)
{
     self->fd = -1;
     return true;
}


bool ae_server_cfg(ae_res_t *e, ae_server_t *self,
                   ae_server_handler_t *handlers,
                   void *ctx)
{
     self->handlers = handlers;
     self->ctx = ctx;
     return true;
}


