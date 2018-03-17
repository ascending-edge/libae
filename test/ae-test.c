#include <stdio.h>
#include <ae/ae.h>
#include <syslog.h>
#include <stdlib.h>
#include <inttypes.h>

static void ae_test_read(ae_mux_t *self,
                         const struct epoll_event *event,
                         void *ctx)
{
     uint64_t overflows = 0;
     ae_res_t e;
     ae_res_init(&e);
     if(!ae_timer_read(&e, ctx, &overflows))
     {
          AE_LR(&e);
          return;
     }
     
     AE_LD("sup: %"PRIu64"", overflows);
}


static bool ae_test_main(ae_res_t *e)
{
     ae_mux_t mux;
     memset(&mux, 0, sizeof(mux));
     AE_TRY(ae_mux_init(e, &mux));

     ae_timer_t t;
     memset(&t, 0, sizeof(t));
     AE_TRY(ae_timer_init(e, &t, CLOCK_MONOTONIC));
     struct timespec ts = {.tv_sec = 1, .tv_nsec = 500000000};
     AE_TRY(ae_timer_every(e, &t, &ts, true));

     ae_mux_event_t me;
     memset(&me, 0, sizeof(me));
     me.ctx = &t;
     me.read = ae_test_read;

     AE_TRY(ae_mux_add(e, &mux, t.fd, &me));

     struct epoll_event events[1];
     for(;;)
     {
          bool was_to = false;
          AE_TRY(ae_mux_wait(e, &mux,
                             events, AE_ARRAY_LEN(events),
                             1500, &was_to));
          AE_LD("mux wait exit was_to=%s", was_to ? "true":"false");
     }
     
     ae_mux_uninit(e, &mux);
     return true;
}

void log_out(void *ctx, ae_log_level_t lvl, const char *msg)
{
     fprintf(stderr, "%s\n", msg);
}


static bool opt_callback(ae_res_t *e,
                         struct ae_opt *self,
                         const struct ae_opt_option *option,
                         const char *arg)
{
     AE_LD("callback for option: %s arg=%s", option->name, arg);
     return true;
}

static bool argument_callback(ae_res_t *e,
                              ae_opt_t *self,
                              void *ctx,
                              const char *argument,
                              ae_opt_callback_reason_t reason)
{
     AE_LW("invalid argument: '%s'", argument);
     return true;
}


/* static bool ae_test_opt(ae_res_t *e, int argc, char **argv) */
/* { */
/*      bool nifty = false; */
/*      int foo = 42; */
/*      int int_args[] = {-1, 1, 10}; */
/*      int int_result = 100; */
/*      ae_opt_option_t options[] = { */
/*           { */
/*                .name = "help", */
/*                .help = "print help", */
/*                .type = AE_OPT_TYPE_HELP, */
/*           }, */
/*           { */
/*                .name = "version", */
/*                .help = "print version", */
/*                .type = AE_OPT_TYPE_VERSION, */
/*           }, */
/*           { */
/*                .name = "Group", */
/*                .help = "a section", */
/*                .type = AE_OPT_TYPE_GROUP, */
/*           }, */
/*           { */
/*                .name = "test", */
/*                .help = "testing", */
/*                .type = AE_OPT_TYPE_INT, */
/*           }, */
/*      }; */
/*      ae_opt_t opt; */
/*      AE_TRY(ae_opt_init(e, &opt, "ae-test", "1.0", "help line", */
/*                         argument_callback, */
/*                         NULL, */
/*                         AE_ARRAY_LEN(options), options)); */
/*      AE_TRY(ae_opt_process(e, &opt, argc, argv)); */
/*      AE_LD("bool=%d", nifty); */

/*      AE_LD("remaining unprocessed args: optind=%d", opt.optind); */
/*      for(size_t i=opt.optind; i<argc; ++i) */
/*      { */
/*           AE_LD("[%zu]='%s'", i, argv[i]); */
/*      } */
/*      ae_opt_help_print(&opt, stdout); */
/*      exit(1); */
/* } */

typedef enum st
{
     ST_HEY = 0,
     ST_THERE,
     ST_END_MARKER
} st_t;

const char *hmm(st_t state)
{
     const char *MAP[] = {
          "HEY",
          "THERE"
     };
     return AE_ENUM_TO_STRING(ST_END_MARKER, state, MAP);
}


int main(int argc, char *argv[])
{
     printf("%s\n", hmm(4));
     return 0;
     
             
     ae_res_t e;
     ae_res_init(&e);
     if(!ae_global_init(&e, log_out, NULL))
     {
          /* report the error... */
          fprintf(stderr, "global init error\n");
          return 1;
     }
     openlog("ae-test", LOG_PERROR, LOG_USER);
     g_ae_logger->mask = 0xff;

     
     if(!ae_test_main(&e))
     {
          AE_LR(&e);
     }

     if(!ae_global_uninit(&e))
     {
          AE_LR(&e);
          return 1;
     }
     return 0;
}
