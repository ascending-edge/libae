#ifndef _AE_EVENT_H
#define _AE_EVENT_H

#include <ae/res.h>
#include <ae/note.h>

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
     ae_event_cb_t cb;
     void *ctx;
} ae_event_data_t;



#ifdef __cplusplus
extern "C" {
#endif

     ae_res_t ae_event_uninit(ae_note_t *e, ae_event_t *self);
     ae_res_t ae_event_init(ae_note_t *e, ae_event_t *self);

     ae_res_t ae_event_add(ae_note_t *e, ae_event_t *self,
                           int fd,
                           uint32_t events,
                           ae_event_data_t *d);

     /* rm */
     /* mod */
     /* get count */
     ae_res_t ae_event_wait(ae_note_t *e, ae_event_t *self,
                            struct epoll_event *events,
                            size_t n_events,
                            int timeout_ms,
                            bool *out_was_timeout);

#ifdef __cplusplus
}
#endif


#endif
