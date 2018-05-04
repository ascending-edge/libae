#ifndef _AE_SERVER_H
#define _AE_SERVER_H

#include <ae/types.h>
#include <ae/res.h>
#include <ae/mux.h>

struct ae_server;
typedef struct ae_server_handler
{
     bool (*on_client_add)(struct ae_server *server, int fd, void **ctx);
     void (*on_client_rm)(struct ae_server *server, void *ctx, int fd);
     void (*on_client_read)(struct ae_server *server, void *ctx, int fd);
     void (*on_client_write)(struct ae_server *server, void *ctx, int fd);
} ae_server_handler_t;

typedef struct ae_server
{
     ae_server_handler_t *handlers;
     void *ctx;
     ae_mux_t *mux;

     ae_mux_handlers_t client_handlers;
     ae_mux_event_t server_event;
     ae_mux_event_t client_event;
} ae_server_t;

#ifdef __cplusplus
extern "C" {
#endif

     void ae_server_uninit(ae_res_t *e, ae_server_t *self);
     bool ae_server_init(ae_res_t *e, ae_server_t *self);
     bool ae_server_cfg(ae_res_t *e, ae_server_t *self,
                        ae_mux_t *mux,
                        ae_server_handler_t *handlers,
                        void *ctx);

     bool ae_server_bind_seqpacket(ae_res_t *e, ae_server_t *self,
                                   const char *path);

     bool ae_server_bind_unix(ae_res_t *e, ae_server_t *self,
                              const char *path);

     bool ae_server_listen(ae_res_t *e, ae_server_t *self);


#ifdef __cplusplus
}
#endif


#endif
