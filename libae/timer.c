#include <ae/timer.h>

#include <unistd.h>
#include <sys/timerfd.h>


bool ae_timer_uninit(ae_res_t *e, ae_timer_t *self)
{
     if(self->fd != -1)
     {
          if(close(self->fd) != 0)
          {
               ae_res_warn(e, "timerfd close: %s", strerror(errno));
          }
     }
     return true;
}


bool ae_timer_init(ae_res_t *e, ae_timer_t *self, int clock_id)
{
     self->fd = -1;
     self->fd = timerfd_create(clock_id, TFD_CLOEXEC);
     if(self->fd == -1)
     {
          ae_res_err(e, "timerfd create: %s", strerror(errno));
          return false;
     }
     return true;
}


