#include <ae/event.h>

#include <unistd.h>
#include <sys/eventfd.h>


bool ae_event_uninit(ae_res_t *e, ae_event_t *self)
{
     if(self->fd != -1)
     {
          if(close(self->fd) != 0)
          {
               ae_res_warn(e, "eventfd close: %s", strerror(errno));
          }
     }
     return true;
}


bool ae_event_init(ae_res_t *e, ae_event_t *self)
{
     self->fd = -1;
     self->fd = eventfd(0, EFD_CLOEXEC);
     if(self->fd == -1)
     {
          ae_res_err(e, "eventfd create: %s", strerror(errno));
          return false;
     }
     return true;
}


bool ae_event_read(ae_res_t *e, ae_event_t *self, uint64_t *out)
{
     if(eventfd_read(self->fd, out) != 0)
     {
          ae_res_err(e, "eventfd_read: %s", strerror(errno));
          return false;
     }
     return true;
}


bool ae_event_write(ae_res_t *e, ae_event_t *self, uint64_t val)
{
     if(eventfd_write(self->fd, val) != 0)
     {
          ae_res_err(e, "eventfd_write: %s", strerror(errno));
          return false;
     }
     return true;
}


