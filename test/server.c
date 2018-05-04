#include <syslog.h>
#include <ae/ae.h>


static ae_res_t s_e;
static ae_mux_t s_mux;
static ae_server_t s_server;
static ae_server_handler_t s_handlers;

static bool on_client_add(struct ae_server *server, int fd, void **ctx)
{
     *ctx = (void*)0x1;
     AE_LT(__FUNCTION__);
     return true;
}

static void on_client_rm(struct ae_server *server, void *ctx, int fd)
{
     AE_LT(__FUNCTION__);
     AE_LD("ctx=%p", ctx);
}


static void on_client_read(struct ae_server *server, void *ctx, int fd)
{
     AE_LT(__FUNCTION__);
     AE_LD("ctx=%p", ctx);
     char buf[64];
     ae_res_t e;
     ae_res_init(&e);
     size_t bytes_read = 0;
     bool was_eof = false;
     if(ae_io_read(&e, fd, buf, sizeof(buf), &bytes_read, &was_eof))
     {
          /* potential overflow here */
          buf[bytes_read] = '\0';
          AE_LD("read (%s)", buf);
     }

     AE_LR(&e);
}


/* static void on_client_write(struct ae_server *server, void *ctx, int fd) */
/* { */
/*      AE_LT(__FUNCTION__); */
/*      AE_LD("ctx=%p", ctx); */
/* } */


static bool server_main(ae_res_t *e, int argc, char **argv)
{
     AE_TRY(ae_mux_init(e, &s_mux));
     AE_TRY(ae_server_init(e, &s_server));
     AE_TRY(ae_server_cfg(e, &s_server, &s_mux, &s_handlers, NULL));

     AE_TRY(ae_server_bind_unix(e, &s_server, "/tmp/server"));
     AE_TRY(ae_server_listen(e, &s_server));

     struct epoll_event events[1];
     for(;;)
     {
          AE_TRY(ae_mux_wait(e, &s_mux, NULL,
                             events, AE_ARRAY_LEN(events),
                             -1, NULL));
     }

     ae_server_uninit(e, &s_server);
     ae_mux_uninit(e, &s_mux);
     return true;
}

static void log_out(void *ctx, ae_log_level_t lvl, const char *msg)
{
     fprintf(stderr, "%s\n", msg);
}



int main(int argc, char *argv[])
{
     ae_res_init(&s_e);

     if(!ae_global_init(&s_e, log_out, NULL))
     {
          /* report the error... */
          fprintf(stderr, "global init error\n");
          return 1;
     }
     openlog("server", LOG_PERROR, LOG_USER);
     g_ae_logger->mask = 0xff;

     bool res = server_main(&s_e, argc, argv);

     ae_global_uninit(&s_e);
     AE_LR(&s_e);
     return res ? 0:1;
}


static ae_server_handler_t s_handlers =
{
     .on_client_add = on_client_add,
     .on_client_rm = on_client_rm,
     .on_client_read = on_client_read,
     /* .on_client_write = on_client_write, */
};
