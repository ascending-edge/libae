#ifndef _AE_TIMER_H
#define _AE_TIMER_H

#include <ae/ae.h>

typedef struct ae_timer
{
     int fd;
     clockid_t clock_id;
} ae_timer_t;

#ifdef __cplusplus
extern "C" {
#endif

     bool ae_timer_uninit(ae_res_t *e, ae_timer_t *self);
     bool ae_timer_init(ae_res_t *e, ae_timer_t *self, int clock_id);

     bool ae_timer_stop(ae_res_t *e, ae_timer_t *self);

     bool ae_timer_every(ae_res_t *e, ae_timer_t *self,
                         const struct timespec *interval,
                         bool trigger_now);
     
     bool ae_timer_single_shot(ae_res_t *e, ae_timer_t *self,
                               const struct timespec *from_now);

     bool ae_timer_read(ae_res_t *e, ae_timer_t *self, uint64_t *out);
     
#ifdef __cplusplus
}
#endif


#endif
