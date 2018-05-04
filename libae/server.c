#include <ae/server.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <linux/limits.h>
#include <unistd.h>
#include <malloc.h>

#include <ae/log.h>
#include <ae/try.h>
#include <ae/misc.h>


static ae_mux_handlers_t s_server_handlers;



void ae_server_uninit(ae_res_t *e, ae_server_t *self)
{
     if((self->server_event.fd != -1)
        && (close(self->server_event.fd) != 0))
     {
          ae_res_err(e, "close: %s", strerror(errno));
     }
}


static void ae_server_on_server_read(ae_mux_t *mux, int fd, void *ctx)
{
     AE_LT(__FUNCTION__);

     ae_server_t *self = ctx;
     struct sockaddr addr;
     socklen_t len;
     int client_fd = -1;
     if((client_fd = accept4(fd, (struct sockaddr*)&addr, &len,
                             SOCK_CLOEXEC)) == -1)
     {
          AE_LW("accept: %s", strerror(errno));
          return;
     }

     void *client_ctx = NULL;
     if(self->handlers->on_client_add)
     {
          if(!self->handlers->on_client_add(ctx, client_fd, &client_ctx))
          {
               /* The caller doesn't want this client so don't add
                * it.  Instead close the client socket. */
               AE_LD("client connection rejected");
               if(close(client_fd) != 0)
               {
                    AE_LW("close: %s", strerror(errno));
               }
               return;
          }
     }

     /* we want to handle this client. */
     /* allocate space for two pointers and then add it to the mux */
     void **pair = malloc(sizeof(*pair) * 2);
     AE_LD("allocated client context: %p", pair);
     pair[0] = ctx;
     pair[1] = client_ctx;
     ae_mux_event_t *client_event = malloc(sizeof(*client_event));
     AE_LD("client event ptr: %p", client_event);
     client_event->fd = client_fd;
     client_event->ctx = pair;
     client_event->handlers = &self->client_handlers;
     ae_res_t e;
     ae_res_init(&e);
     if(!ae_mux_add(&e, self->mux, client_event))
     {
          AE_LR(&e);
     }
}


static void ae_server_on_server_write(ae_mux_t *mux, int fd, void *ctx)
{
     AE_LT(__FUNCTION__);
}


static void ae_server_on_server_hangup(ae_mux_t *mux, int fd, void *ctx)
{
     AE_LT(__FUNCTION__);
}


static void ae_server_on_server_priority(ae_mux_t *mux, int fd, void *ctx)
{
     AE_LT(__FUNCTION__);
}


static void ae_server_on_server_error(ae_mux_t *mux, int fd, void *ctx)
{
     AE_LT(__FUNCTION__);
}


static void ae_server_on_client_read(ae_mux_t *mux, int fd, void *ctx)
{
     AE_LT(__FUNCTION__);
     void **x = ctx;
     ae_server_t *self = x[0];
     void *client_ctx = x[1];
     if(!self->handlers->on_client_read)
     {
          return;
     }
     self->handlers->on_client_read(self, client_ctx, fd);
}


static void ae_server_on_client_write(ae_mux_t *mux, int fd, void *ctx)
{
     AE_LT(__FUNCTION__);
     void **x = ctx;
     ae_server_t *self = x[0];
     void *client_ctx = x[1];
     if(!self->handlers->on_client_write)
     {
          return;
     }
     self->handlers->on_client_write(self, client_ctx, fd);
}




/* static void ae_server_on_client_priority(ae_mux_t *mux, int fd, void *ctx) */
/* { */
/*      AE_LT(__FUNCTION__); */
/* } */

static void ae_server_client_rm(ae_mux_t *mux, int fd, void *ctx)
{
     void **x = ctx;
     ae_server_t *self = x[0];
     void *client_ctx = x[1];
     if(!self->handlers->on_client_rm)
     {
          return;
     }
     self->handlers->on_client_rm(self, client_ctx, fd);
     ae_res_t e;
     ae_res_init(&e);
     ae_mux_rm(&e, mux, fd);
     if(close(fd) == -1)
     {
          ae_res_err(&e, "close: %s", strerror(errno));
     }
     AE_LR(&e);
     free(ctx);
}

static void ae_server_on_client_hangup(ae_mux_t *mux, int fd, void *ctx)
{
     AE_LT(__FUNCTION__);
     ae_server_client_rm(mux, fd, ctx);
}


static void ae_server_on_client_error(ae_mux_t *mux, int fd, void *ctx)
{
     AE_LT(__FUNCTION__);
     ae_server_client_rm(mux, fd, ctx);
}



bool ae_server_init(ae_res_t *e, ae_server_t *self)
{
     self->server_event.fd = -1;
     self->server_event.handlers = &s_server_handlers;
     self->server_event.ctx = self;
     self->client_event.ctx = self;     
     return true;
}


bool ae_server_cfg(ae_res_t *e, ae_server_t *self,
                   ae_mux_t *mux,
                   ae_server_handler_t *handlers,
                   void *ctx)
{
     self->mux = mux;
     self->handlers = handlers;
     self->ctx = ctx;
     if(handlers->on_client_read)
     {
          self->client_handlers.read = ae_server_on_client_read;
     }
     if(handlers->on_client_write)
     {
          self->client_handlers.write = ae_server_on_client_write;
     }
     self->client_handlers.hangup = ae_server_on_client_hangup;
     self->client_handlers.error = ae_server_on_client_error;
     return true;
}

static bool ae_server_open(ae_res_t *e, ae_server_t *self, int flags)
{
     if(self->server_event.fd != -1)
     {
          ae_res_err(e, "socket already open!");
          return false;
     }

     self->server_event.fd = socket(AF_UNIX,
                                    flags | SOCK_CLOEXEC | SOCK_STREAM,
                                    0);
     if(self->server_event.fd == -1)
     {
          ae_res_err(e, "socket: %s", strerror(errno));
          return false;
     }
     return true;
}


bool ae_server_bind_seqpacket(ae_res_t *e, ae_server_t *self,
                              const char *path)
{
     AE_TRY(ae_server_open(e, self, SOCK_SEQPACKET|SOCK_CLOEXEC));

     /* struct sockaddr_un { */
     /*     sa_family_t sun_family;               /\* AF_UNIX *\/ */
     /*     char        sun_path[UNIX_PATH_MAX];  /\* pathname *\/ */
     /* }; */

     /* Setup the addressing */
     struct sockaddr_un addr;
     memset(&addr, 0, sizeof(addr));
     addr.sun_family = AF_UNIX;
     strncpy(addr.sun_path, path, sizeof(addr.sun_path));

     if(bind(self->server_event.fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
     {
          ae_res_err(e, "bind %s: %s", path, strerror(errno));
          return false;
     }

     return true;

}




bool ae_server_bind_unix(ae_res_t *e, ae_server_t *self,
                         const char *path)
{

     /* UGLY GROSS COPY PASTE */
     AE_TRY(ae_server_open(e, self, 0));

     /* struct sockaddr_un { */
     /*     sa_family_t sun_family;               /\* AF_UNIX *\/ */
     /*     char        sun_path[UNIX_PATH_MAX];  /\* pathname *\/ */
     /* }; */

     /* Setup the addressing */
     struct sockaddr_un addr;
     memset(&addr, 0, sizeof(addr));
     addr.sun_family = AF_UNIX;
     strncpy(addr.sun_path, path, sizeof(addr.sun_path));

     if(bind(self->server_event.fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
     {
          ae_res_err(e, "bind %s: %s", path, strerror(errno));
          return false;
     }

     return true;
}


bool ae_server_listen(ae_res_t *e, ae_server_t *self)
{
     if(listen(self->server_event.fd, 1024) == -1)
     {
          ae_res_err(e, "listen: %s", strerror(errno));
          return false;
     }
     AE_TRY(ae_mux_add(e, self->mux, &self->server_event));
     return true;
}




static ae_mux_handlers_t s_server_handlers =
{
     .read = ae_server_on_server_read,
     .write = ae_server_on_server_write,
     .hangup = ae_server_on_server_hangup,
     .priority = ae_server_on_server_priority,
     .error = ae_server_on_server_error,
};
