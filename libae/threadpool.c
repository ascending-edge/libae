#include <ae/threadpool.h>

#include <ae/try.h>
#include <ae/log.h>


typedef enum ae_threadpool_priority
{
	AE_THREADPOOL_PRIORITY_STOP = 10,
	AE_THREADPOOL_PRIORITY_JOB = 1,
} ae_threadpool_priority_t;


typedef enum ae_threadpool_type
{
	AE_THREADPOOL_TYPE_JOB=0,
	AE_THREADPOOL_TYPE_STOP,

	AE_THREADPOOL_TYPE_ENDMARKER	
} ae_threadpool_type_t;


typedef struct ae_threadpool_msg
{
	ae_threadpool_type_t type;
	union
	{
		ae_threadpool_job_t job;
	};
} ae_threadpool_msg_t;


static bool ae_threadpool_send(ae_res_t *e, ae_threadpool_t *self,
                               const ae_threadpool_msg_t *msg,
                               ae_threadpool_priority_t priority)
{
	if(mq_send(self->queue, (const char*)msg, sizeof(*msg), priority) == -1)
     {
          ae_res_err(e, "mq_send: %s", strerror(errno));
          return false;
     }
     return true;
}


static bool ae_threadpool_send_job(ae_res_t *e, ae_threadpool_t *self,
                                   const ae_threadpool_job_t *job)
{
     ae_threadpool_msg_t msg;
     msg.type = AE_THREADPOOL_TYPE_JOB;
     msg.job = *job;
     AE_TRY(ae_threadpool_send(e, self, &msg, AE_THREADPOOL_PRIORITY_JOB));
     return true;
}


static bool ae_threadpool_send_stop(ae_res_t *e, ae_threadpool_t *self)
{
     ae_threadpool_msg_t msg;
     msg.type = AE_THREADPOOL_TYPE_STOP;
     AE_TRY(ae_threadpool_send(e, self, &msg, AE_THREADPOOL_PRIORITY_STOP));
     return true;
}


void ae_threadpool_uninit(ae_res_t *e, ae_threadpool_t *self)
{
     if(self->queue != -1
        && (mq_close(self->queue) == -1))
	{
		ae_res_err(e, "closing queue: %s", strerror(errno));
	}
}


bool ae_threadpool_init(ae_res_t *e, ae_threadpool_t *self,
                        ae_pool_t *pool,
                        const char *name, size_t n_threads)
{
	self->n_threads = n_threads;
     AE_TRY(ae_pool_calloc(e, pool, &self->ids,
                           sizeof(self->ids[0]) * n_threads));

	/* struct mq_attr { */
	/*     long mq_flags;       /\* Flags: 0 or O_NONBLOCK *\/ */
	/*     long mq_maxmsg;      /\* Max. # of messages on queue *\/ */
	/*     long mq_msgsize;     /\* Max. message size (bytes) *\/ */
	/*     long mq_curmsgs;     /\* # of messages currently in queue *\/ */
	/* }; */

	/* A rather arbitrary maximum queue size. */
	struct mq_attr attr;
	memset(&attr, 0, sizeof(attr));
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(ae_threadpool_msg_t);

	self->queue = mq_open(name, O_RDWR|O_CREAT, DEFFILEMODE, &attr);
	if(self->queue == -1)
	{
		ae_res_err(e, "open: %s", strerror(errno));
		return false;
	}
	/* avoid having the queue around after the process exits */
	if(mq_unlink(name) == -1)
	{
		ae_res_err(e, "mq_unlink: %s", strerror(errno));
		return false;
	}
	return true;
}


static void* ae_threadpool_run(void *_self)
{
	ae_threadpool_t *self = _self;

	/* the kernel does all of the synchronization for us. */
	ae_threadpool_msg_t msg;
	for(;;)
	{
		if(mq_receive(self->queue, (char*)&msg, sizeof(msg), NULL) == -1)
		{
			/* @todo handle an error. */
			AE_LW("mq_receive: %s", strerror(errno));
			return NULL;
		}
		switch(msg.type)
		{
		case AE_THREADPOOL_TYPE_JOB:
			msg.job.func(msg.job.args);
			break;
		case AE_THREADPOOL_TYPE_STOP:
			return NULL;
			break;
		default:
			/* impossible condition */
			break;
		}
	}
	return NULL;
}


bool ae_threadpool_start(ae_res_t *e, ae_threadpool_t *self)
{
	for(size_t i=0; i<self->n_threads; ++i)
	{
		int res = pthread_create(&self->ids[i], NULL, ae_threadpool_run, self);
		if(res != 0)
		{
			ae_res_err(e, "thread: %s", strerror(res));
			return false;
		}
	}
	return true;
}


bool ae_threadpool_enque(ae_res_t *e, ae_threadpool_t *self,
                         const ae_threadpool_job_t *job)
{
	ae_threadpool_msg_t msg;
	msg.type = AE_THREADPOOL_TYPE_JOB;
	msg.job = *job;
     AE_TRY(ae_threadpool_send_job(e, self, job));
	return true;
}


bool ae_threadpool_join(ae_res_t *e, ae_threadpool_t *self)
{
	/* for all threads send the stop message, then join */
	ae_threadpool_msg_t msg;
	msg.type = AE_THREADPOOL_TYPE_STOP;
	for(size_t i=0; i<self->n_threads; ++i)
	{
          AE_TRY(ae_threadpool_send_stop(e, self));
	}

	for(size_t i=0; i<self->n_threads; ++i)
	{
		int res = pthread_join(self->ids[i], NULL);
		if(res != 0)
		{
			ae_res_err(e, "join: %s", strerror(errno));
			return false;
		}
	}	
	return true;
}
