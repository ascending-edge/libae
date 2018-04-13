#ifndef _AE_SERVER_H
#define _AE_SERVER_H

typedef struct ae_server_handler
{
     bool (*on_client_new)(ae_res_t *e, void *ctx,
                           void *cookie, ps_tcp_t *client);
     bool (*on_client_close)(ae_res_t *e, void *ctx,
                             void *clientCtx, void *cookie, ps_tcp_t *client);
     bool (*on_client_read)(void *ctx,
                            void *clientCtx,
                            void *cookie, ps_tcp_t *client);
     bool (*on_client_write)(void *ctx,
                             void *clientCtx,
                             void *cookie, ps_tcp_t *client);
} ae_server_handler_t;

typedef struct ae_server
{
     int fd;
     ae_server_handler_t *handlers;
     void *ctx;
} ae_server_t;

#ifdef __cplusplus
extern "C" {
#endif

     void ae_server_uninit(ae_res_t *e, ae_server_t *self);
     bool ae_server_init(ae_res_t *e, ae_server_t *self);
     bool ae_server_cfg(ae_res_t *e, ae_server_t *self,
                        ae_server_handler_t *handlers,
                        void *ctx);

     bool ae_server_bind_unix(ae_res_t *e, ae_server_t *self,
                              const char *path);
     /* bool ae_server_bind_tcp(ae_res_t *e, ae_server_t *self, */
     /*                         const char *intf, uin16_t port); */

     bool ae_server_listen(ae_res_t *e, ae_server_t *self);
#ifdef __cplusplus
}
#endif


#endif
