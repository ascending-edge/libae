#include <stdio.h>
#include <ae/ae.h>

#include <stdlib.h>


static bool cool(ae_res_t *e)
{
     ae_res_err(e, "hello from cool %u", LIBAE_MAJOR);
     return false;
}


static void ae_test_event_read(ae_event_t *ae_event,
                               const struct epoll_event *event,
                               void *ctx)
{
     uint32_t *val = ctx;
     fprintf(stderr, "context %u!!!\n", *val);
}

static bool ae_test_event(ae_res_t *e)
{
     fprintf(stderr, "test event\n");
     uint32_t foo = 68;
     ae_event_t event;
     ae_event_data_t data;
     data.read = ae_test_event_read;
     data.ctx = &foo;
     AE_TRY(ae_event_init(e, &event));

     AE_TRY(ae_event_add(e, &event, fileno(stdin), &data));

     struct epoll_event events[1];
     bool was_timeout = false;
     AE_TRY(ae_event_wait(e, &event,
                          events,
                          sizeof(events)/sizeof(events[0]),
                          2000,
                          &was_timeout));
     fprintf(stderr, "was timeout: %s\n", was_timeout ? "true":"false");
     return true;
}


static bool ae_test_main(ae_res_t *e)
{
     ae_pool_t pool;
     AE_TRY(ae_pool_init(e, &pool));

     ae_ptrarray_t ar;
     AE_TRY(ae_ptrarray_init(e, &ar, &pool, 5));
     char *hmm[] = {
          "one",
          "two",
          "three",
          "four",
          "five",
          "six",
          "seven",
          "eight",
          "nine",
     };
     for(size_t i=0; i<sizeof(hmm)/sizeof(hmm[0]); ++i)
     {
          AE_TRY(ae_ptrarray_append(e, &ar, hmm[i]));
     }
     for(size_t i=0; i<ar.len; ++i)
     {
          char *foo = ae_ptrarray_at(&ar, i);
          AE_LD("foo[%zu]=\"%s\"", i, foo);
     }

     AE_TRY(ae_test_event(e));
     
     AE_TRY(ae_pool_uninit(e, &pool));
     AE_TRY(cool(e));
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
/*                .name = "argument1", */
/*                .help = "argument 1 help text", */
/*                .is_required = AE_OPT_PARAM_REQUIRED, */
/*                .callback = opt_callback, */
/*           }, */
/*           { */
/*                .name = "Group Name", */
/*                .help = "group help text", */
/*                .required = AE_OPT_GROUP, */
/*           }, */
/*           { */
/*                .name = "short", */
/*                .help = "argument 1 help text", */
/*                .required = AE_OPT_REQUIRED, */
/*                .callback = opt_callback, */
/*                .ctx = &foo, */
/*           }, */
/*           { */
/*                .name = "test", */
/*                .help = "nifty", */
/*                .required = AE_OPT_BOOL, */
/*                .callback = opt_callback, */
/*                .ctx = &foo, */
/*                .out = &nifty, */
/*           }, */
/*           { */
/*                .name = "int", */
/*                .help = "sweet", */
/*                .required = AE_OPT_INT, */
/*                .callback = opt_callback, */
/*                .ctx = &foo, */
/*                .in = &int_args, */
/*                .out = &int_result, */
/*           }, */
/*      }; */
/*      ae_opt_t opt; */
/*      AE_TRY(ae_opt_init(e, &opt, "ae-test", "help line", */
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

int main(int argc, char *argv[])
{
     ae_res_t e;
     ae_res_init(&e);
     if(!ae_global_init(&e, log_out, NULL))
     {
          /* report the error... */
          fprintf(stderr, "global init error\n");
          return 1;
     }

     /* if(!ae_test_opt(&e, argc -1, argv+1)) */
     /* { */
     /*      AE_LR(&e); */
     /* } */
     
     /* if(!ae_test_main(&e)) */
     /* { */
     /*      AE_LR(&e); */
     /* } */

     if(!ae_global_uninit(&e))
     {
          AE_LR(&e);
          return 1;
     }
     return 0;
}
