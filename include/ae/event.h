#ifndef _AE_EVENT_H
#define _AE_EVENT_H

#include <ae/res.h>

#include <sys/epoll.h>

typedef struct ae_event
{
     size_t count;
     int epoll_fd;
} ae_event_t;


typedef void (*ae_event_cb_t)(ae_event_t *self,
                              const struct epoll_event *event,
                              void *ctx);

typedef struct ae_event_data
{
     void *ctx;
     ae_event_cb_t read;
     ae_event_cb_t write;
     ae_event_cb_t hangup;
     ae_event_cb_t priority;
     ae_event_cb_t error;
} ae_event_data_t;



#ifdef __cplusplus
extern "C" {
#endif

     bool ae_event_uninit(ae_res_t *e, ae_event_t *self);
     bool ae_event_init(ae_res_t *e, ae_event_t *self);

     bool ae_event_add(ae_res_t *e, ae_event_t *self,
                       int fd,
                       const ae_event_data_t *d);

     int ae_event_get_n_registered(ae_event_t *self);

     /* rm */
     /* mod */
     /* get count */
     bool ae_event_wait(ae_res_t *e, ae_event_t *self,
                        struct epoll_event *events,
                        size_t n_events,
                        int timeout_ms,
                        bool *out_was_timeout);

#ifdef __cplusplus
}
#endif


#endif
