#include <ae/mux.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include <ae/try.h>


bool ae_mux_uninit(ae_res_t *e, ae_mux_t *self)
{
	if(close(self->epoll_fd) != 0)
	{
		ae_res_warn(e, "close: %s", strerror(errno));
	}
	return true;
}


bool ae_mux_init(ae_res_t *e, ae_mux_t *self)
{
     self->count = 0;
	if((self->epoll_fd = epoll_create1(0)) == -1)
	{
		ae_res_err(e, "epoll:%s", strerror(errno));
          return false;
	}
	return true;
}


/* bool st_ep_mod(st_err_t *err, st_ep_t *self, int events, st_ep_entry_t *entry) */
/* { */
/* 	struct epoll_event event; */
/* 	memset(&event, 0, sizeof(event)); */
/* 	event.events = events; */
/* 	event.data.ptr = entry; */

/* 	if(epoll_ctl(self->fd, EPOLL_CTL_MOD, entry->fd, &event) == -1) */
/* 	{ */
/* 		ST_ERR_SET(err, "epoll_ctl(%d): %s", entry->fd, strerror(errno)); */
/* 		return false; */
/* 	} */
/* 	return true; */
/* } */


bool ae_mux_add(ae_res_t *e, ae_mux_t *self, int fd, const ae_mux_event_t *d)
{
     uint32_t events = 0;
     if(d->read)
     {
          events |= EPOLLIN;
     }
     if(d->write)
     {
          events |= EPOLLOUT;
     }
     if(d->write)
     {
          events |= EPOLLOUT;
     }
     if(d->hangup)
     {
          events |= EPOLLRDHUP;
     }
     if(d->priority)
     {
          events |= EPOLLPRI;
     }
     /* EPOLLERR is not needed to be set in the events flags, it is
      * automatic. */
     /* if(d->error) */
     /* { */
     /* } */

	struct epoll_event event;
     event.events = events;
     event.data.ptr = (void*)d;

     /* fprintf(stderr, "cb=%p ctx=%p\n", info->cb, info->ctx); */
     
     
	if(epoll_ctl(self->epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		ae_res_err(e, "epoll: %s", strerror(errno));
		return false;
	}
	++self->count;
	return true;
}


static bool ae_mux_now_get(ae_res_t *e, uint64_t *now_ms)
{
	struct timespec tp;
	memset(&tp, 0, sizeof(tp));
	if(clock_gettime(CLOCK_MONOTONIC, &tp) == -1)
	{
		ae_res_err(e, "clock_gettime: %s", strerror(errno));
          return false;
	}
	/* convert timespec into ms.  1000 ms per second, 1,000,000
	 * nanoseconds per ms. */
	uint64_t ms = 0;
	ms = ((uint64_t)tp.tv_sec * (uint64_t)1000)
		+ ((uint64_t)tp.tv_nsec / (uint64_t)1000000);
     *now_ms = ms;
	return true;
}


bool ae_mux_wait(ae_res_t *e, ae_mux_t *self,
                 struct epoll_event *events,
                 size_t n_events,
                 int timeout_ms,
                 bool *out_was_timeout)
{
	if(out_was_timeout)
	{
		*out_was_timeout = false;
	}

	if(self->count == 0)
	{
          if(timeout_ms)
          {
               usleep(timeout_ms * 1000);
               *out_was_timeout = true;
          }
		return true;
	}

	uint64_t then = 0;
     AE_TRY(ae_mux_now_get(e, &then));
	then += timeout_ms;

	int res = 0;
	for(;;)
	{
		res = epoll_wait(self->epoll_fd, events, self->count, timeout_ms);
		if((res < 0)
		   && (errno == EINTR))
		{
			/* epoll returned before any events occurred and the
                * timeout didn't expire either.  Adjust the timeout
                * accordingly and re-enter the wait */
			uint64_t now = 0;
			AE_TRY(ae_mux_now_get(e, &now));
               if(now < then)
               {
                    timeout_ms = then - now;
               }
		}
		else
		{
			/* All done. */
			break;
		}
	}

	/* If this was a timeout set the flag and return */
	if(res == 0)
	{
		if(out_was_timeout)
		{
			*out_was_timeout = true;
		}
		return true;
	}

	/* error... */
	if(res < 0)
	{
		ae_res_err(e, "epoll: %s", strerror(errno));
		return false;
	}

	/* res is not 0, and it isn't less than zero, so an event
	 * occurred.  Process all handles with events. */
	for(int i=0; i<res; ++i)
	{
          struct epoll_event *event = &events[i];
          uint32_t events = event->events;
		ae_mux_event_t *info = event->data.ptr;
          if((events & EPOLLIN)
             && info->read)
          {
               info->read(self, event, info->ctx);
          }
          if((events & EPOLLOUT)
             && info->write)
          {
               info->write(self, event, info->ctx);
          }
          if(((events & EPOLLRDHUP)
              || events & EPOLLHUP)
             && info->hangup)
          {
               info->hangup(self, event, info->ctx);
          }
          if((events & EPOLLPRI)
             && info->priority)
          {
               info->priority(self, event, info->ctx);
          }
          if((events & EPOLLERR)
             && info->error)
          {
               info->error(self, event, info->ctx);
          }
	}
	return true;
}


bool ae_mux_rm(ae_res_t *e, ae_mux_t *self, int fd)
{
     /* Avoid error for invalid/closed file descriptors */
     if(fcntl(fd, F_GETFD, 0) == -1)
	{
		return true;
	}

	if(epoll_ctl(self->epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1)
	{
		ae_res_err(e, "epoll del fd=%d: %s", fd, strerror(errno));
		return false;
	}

	return true;
}

/* bool st_ep_rm(st_err_t *err, st_ep_t *self, st_ep_entry_t *entry) */
/* { */
/* 	if(!entry) */
/* 	{ */
/* 		ST_ERR_SET(err, "%s", "invalid entry"); */
/* 		return false; */
/* 	} */

/* 	if(fcntl(entry->fd, F_GETFD, 0) == -1) */
/* 	{ */
/* 		return true; */
/* 	} */

/* 	if(epoll_ctl(self->fd, EPOLL_CTL_DEL, entry->fd, NULL) == -1) */
/* 	{ */
/* 		ST_ERR_SET(err, "epoll(%d): %s", entry->fd, strerror(errno)); */
/* 		return false; */
/* 	} */

/* 	return true; */
/* } */


/* size_t st_ep_get_count(const st_ep_t *self) */
/* { */
/* 	return self->count; */
/* } */

