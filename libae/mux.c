#include <ae/mux.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include <ae/try.h>
#include <ae/log.h>


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


static uint32_t ae_mux_event_to_flags(const ae_mux_event_t *d)
{
     uint32_t events = 0;
     if(!d->handlers)
     {
          return events;
     }

     if(d->handlers->read)
     {
          events |= EPOLLIN;
     }
     if(d->handlers->write)
     {
          events |= EPOLLOUT;
     }
     if(d->handlers->write)
     {
          events |= EPOLLOUT;
     }
     if(d->handlers->hangup)
     {
          events |= EPOLLRDHUP;
     }
     if(d->handlers->priority)
     {
          events |= EPOLLPRI;
     }
     /* EPOLLERR is not needed to be set in the events flags, it is
      * automatic. */
     /* if(d->error) */
     /* { */
     /* } */
     
     return events;
}


static bool ae_mux_ctl(ae_res_t *e, ae_mux_t *self,
                       const ae_mux_event_t *d,
                       int op)
{
     uint32_t events = ae_mux_event_to_flags(d);

	struct epoll_event event;
     event.events = events;
     event.data.ptr = (void*)d;

     /* fprintf(stderr, "cb=%p ctx=%p\n", info->cb, info->ctx); */
     
	if(epoll_ctl(self->epoll_fd, op, d->fd, &event) == -1)
	{
		ae_res_err(e, "epoll: %s", strerror(errno));
		return false;
	}
	++self->count;
	return true;
}
                              

bool ae_mux_add(ae_res_t *e, ae_mux_t *self, const ae_mux_event_t *d)
{
     AE_LD("adding %p, fd=%d", d, d->fd);
     AE_TRY(ae_mux_ctl(e, self, d, EPOLL_CTL_ADD));
     return true;
}


bool ae_mux_mod(ae_res_t *e, ae_mux_t *self, int fd, const ae_mux_event_t *d)
{
     AE_TRY(ae_mux_ctl(e, self, d, EPOLL_CTL_MOD));
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


static void ae_mux_singlethread_handler(ae_mux_t *self, 
                                        struct epoll_event *event)
{
     uint32_t events = event->events;
     ae_mux_event_t *info = event->data.ptr;
     AE_LD("info: %p", info);
     if(!info->handlers)
     {
          AE_LW("mux event with no handlers...what's the point?");
          return;
     }
     if((events & EPOLLIN)
        && info->handlers->read)
     {
          info->handlers->read(self, info->fd, info->ctx);
     }
     if((events & EPOLLOUT)
        && info->handlers->write)
     {
          info->handlers->write(self, info->fd, info->ctx);
     }
     if(((events & EPOLLRDHUP)
         || events & EPOLLHUP)
        && info->handlers->hangup)
     {
          info->handlers->hangup(self, info->fd, info->ctx);
     }
     if((events & EPOLLPRI)
        && info->handlers->priority)
     {
          info->handlers->priority(self, info->fd, info->ctx);
     }
     if((events & EPOLLERR)
        && info->handlers->error)
     {
          info->handlers->error(self, info->fd, info->ctx);
     }
}


static bool ae_mux_barrier_wait(ae_res_t *e, pthread_barrier_t *barrier)
{
     int res = pthread_barrier_wait(barrier);
/* RETURN VALUE */
/*        Upon successful completion, the pthread_barrier_wait()
 *        function shall */
/*        return PTHREAD_BARRIER_SERIAL_THREAD for a single
 *        (arbitrary) thread */
/*        synchronized at the barrier and zero for each of the other
 *        threads. */
/*        Otherwise, an error number shall be returned to indicate the
 *        error. */
     
     /* PTHREAD_BARRIER_SERIAL_THREAD      */
     if(res != 0)
     {
          AE_LW("pthread barrier wait: %s", strerror(res));
     }
     return true;
}


static void ae_mux_multithread_handler(void *_args)
{
     void **args = _args;
     ae_mux_t *self = args[0];
     struct epoll_event *event = args[1];
     pthread_barrier_t *barrier = args[2];
     ae_mux_singlethread_handler(self, event);

     ae_res_t e;
     ae_res_init(&e);
     ae_mux_barrier_wait(&e, barrier);
     AE_LR(&e);
}


static void ae_mux_threadpool_add(ae_mux_t *self,
                                  ae_pool_t *mempool,
                                  struct epoll_event *event,
                                  ae_threadpool_t *thread_pool,
                                  pthread_barrier_t *barrier)
{
     /** @todo allocate the job and the arguments from mempool */
     ae_threadpool_job_t job;
     job.func = ae_mux_multithread_handler;
     void *args[] = {self, event, barrier};
     job.args = args;

     ae_res_t e;
     ae_res_init(&e);
     /* ae_threadpool_enqueue(&e, thread_pool, &job); */
     AE_LR(&e);
}


static bool ae_mux_barrier_init(ae_res_t *e,
                                pthread_barrier_t *barrier,
                                unsigned int count)
{
     int res = pthread_barrier_init(barrier, NULL, count);
     if(res != 0)
     {
          ae_res_err(e, "pthread barrier init: %s", strerror(res));
          return false;
     }
     return true;
}


static bool ae_mux_barrier_uninit(ae_res_t *e, pthread_barrier_t *barrier)
{
     int res = pthread_barrier_destroy(barrier);
     if(res != 0)
     {
          ae_res_err(e, "pthread barrier destroy: %s", strerror(res));
          return false;
     }     
     return true;
}


bool ae_mux_wait(ae_res_t *e, ae_mux_t *self,
                 ae_threadpool_t *threadpool,
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

     pthread_barrier_t barrier;
     ae_pool_t mempool;
     memset(&mempool, 0, sizeof(mempool));
     if(threadpool)
     {
          /* create a barrier.  Each FD must check in. */
          /* AE_TRY(ae_pool_init(e, &mempool, res * ?)); */
          AE_TRY(ae_mux_barrier_init(e, &barrier, res));
     }
	/* res is not 0, and it isn't less than zero, so an event
	 * occurred.  Submit each FD to a threadpool or handle in this
	 * thread. */
	for(int i=0; i<res; ++i)
	{
          struct epoll_event *event = &events[i];
          if(threadpool)
          {
               /* ae_mux_threadpool_add(self, event, threadpool, &barrier); */
          }
          else
          {
               ae_mux_singlethread_handler(self, event);
          }
	}

     /* wait for everything to finish */
     if(threadpool)
     {
          ae_mux_barrier_wait(e, &barrier);
          ae_mux_barrier_uninit(e, &barrier);
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
     --self->count;
	return true;
}
