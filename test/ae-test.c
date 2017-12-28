#include <stdio.h>
#include <config.h>
#include <ae/ae.h>


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
     data.cb = ae_test_event_read;
     data.ctx = &foo;
     AE_TRY(ae_event_init(e, &event));

     AE_TRY(ae_event_add(e, &event, fileno(stdin), EPOLLIN, &data));

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


#include <obstack.h>
int main(int argc, char *argv[])
{
     ae_res_t e;
     ae_res_init(&e);
     if(ae_global_init(&e, log_out, NULL))
     {
          /* report the error... */
          fprintf(stderr, "global init error\n");
          return 1;
     }

     
     if(ae_test_main(&e))
     {
          AE_LN(&e);
     }

     if(ae_global_uninit(&e))
     {
          AE_LN(&e);
          return 1;
     }
     return 0;
}
