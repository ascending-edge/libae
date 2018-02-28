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

static bool ae_timer_set(ae_res_t *e, ae_timer_t *self,
                         int flags,
                         const struct timespec *interval,
                         const struct timespec *value)
{
     struct itimerspec new_value;
     new_value.it_interval = *interval;
     new_value.it_value = *value;
     if(timerfd_settime(self->fd, flags, &new_value, NULL) != 0)
     {
          ae_res_err(e, "timer: %s", strerror(errno));
          return false;
     }
     return true;
          
}


bool ae_timer_stop(ae_res_t *e, ae_timer_t *self)
{
     struct itimerspec new_value;
     memset(&new_value, 0, sizeof(new_value));
     AE_TRY(ae_timer_set(e, self, 0,
                         &new_value.it_interval,
                         &new_value.it_value));
     return true;
}


bool ae_timer_every(ae_res_t *e, ae_timer_t *self,
                    const struct timespec *interval)
{
     AE_TRY(ae_timer_set(e, self, 0, interval, interval));
     return true;
}


bool ae_timer_single_shot(ae_res_t *e, ae_timer_t *self,
                          const struct timespec *from_now)
{
     struct timespec nada;
     memset(&nada, 0, sizeof(nada));
     AE_TRY(ae_timer_set(e, self, 0, &nada, from_now));
     return true;
}


bool ae_timer_read(ae_res_t *e, ae_timer_t *self, uint64_t *out)
{
     ssize_t res = read(self->fd, out, sizeof(*out));
     if(res == sizeof(*out))
     {
          return true;
     }
     if(res == -1)
     {
          ae_res_err(e, "timer read: %s", strerror(errno));
          return false;
     }
     ae_res_err(e, "timer read: unexpected read return");
     return false;
}


