#ifndef _AE_THREADPOOL_H
#define _AE_THREADPOOL_H

#include <pthread.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>

#include <ae/pool.h>
#include <ae/res.h>
#include <ae/types.h>

typedef struct ae_threadpool_job
{
	void (*func)(void *args);
	void *args;
} ae_threadpool_job_t;


typedef struct ae_threadpool
{
	size_t n_threads;
	pthread_t *ids;

	/* use a posix message queue for ease...some system overhead though */
	/* pthread_mutex_t lock; */
	/* pthread_cond_t cond; */
	mqd_t queue;
} ae_threadpool_t;

#ifdef __cplusplus
extern "C" {
#endif

	void ae_threadpool_uninit(ae_res_t *e, ae_threadpool_t *self);
	bool ae_threadpool_init(ae_res_t *e, ae_threadpool_t *self,
                             ae_pool_t *pool, const char *name,
                             size_t n_threads);
	bool ae_threadpool_start(ae_res_t *e, ae_threadpool_t *self);

	bool ae_threadpool_enqueue(ae_res_t *e, ae_threadpool_t *self,
                                const ae_threadpool_job_t *job);

	bool ae_threadpool_join(ae_res_t *e, ae_threadpool_t *self);	
#ifdef __cplusplus
}
#endif
#endif

