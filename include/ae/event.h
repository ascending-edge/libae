/**
 * @author Greg Rowe <greg.rowe@ascending-edge.com>
 * 
 * Copyright (C) 2018 Ascending Edge, LLC - All Rights Reserved
 */
#ifndef _AE_EVENT_H
#define _AE_EVENT_H

#include <ae/res.h>
#include <sys/epoll.h>


/**
 * This class makes it easy to use epoll (or some other multiplexing
 * mechanism).
 */
typedef struct ae_event
{
     size_t count;              /**< The number of file descriptors
                                 * registered */
     int epoll_fd;              /**< The epoll file descriptor */
} ae_event_t;


/** 
 * Callbacks must look like this.
 *
 * @param self the event object
 * @param event the fd/event that was triggered  (see epoll.h)
 * @param ctx the context pointer that was registered with the fd/event
 */
typedef void (*ae_event_cb_t)(ae_event_t *self,
                              const struct epoll_event *event,
                              void *ctx);


/**
 * This is used for registering a fd/event.  Any field can be NULL.
 */
typedef struct ae_event_data
{
     void *ctx;                 /**< arbitrary context */
     ae_event_cb_t read;        /**< read event callback */
     ae_event_cb_t write;       /**< write event callback */
     ae_event_cb_t hangup;      /**< hangup event callback */
     ae_event_cb_t priority;    /**< priority data event callback */
     ae_event_cb_t error;       /**< error event callback */
} ae_event_data_t;



#ifdef __cplusplus
extern "C" {
#endif

     /** 
      * Cleans up the event wrapper.  This must be called for proper
      * cleanup.
      */
     bool ae_event_uninit(ae_res_t *e, ae_event_t *self);


     /** 
      * This prepares an event wrapper for use.  This must be called
      * prior to any other call.
      */
     bool ae_event_init(ae_res_t *e, ae_event_t *self);


     /** 
      * This adds a file descriptor to the event wrapper.
      *
      * @param fd the file descriptor to monitor
      * 
      * @param d the callbacks.  These must exist, this is not copied.
      */
     bool ae_event_add(ae_res_t *e, ae_event_t *self,
                       int fd,
                       const ae_event_data_t *d);


     /** 
      * Returns the number of currently registered file descriptors.
      * 
      * @return the number of registered file descriptors.
      */
     int ae_event_get_n_registered(ae_event_t *self);


     /** 
      * This will block until one or more events occur on the
      * registered file descriptors or the @p timeout_ms occurs.  You
      * must supply an array, @p events, large enough to accommodate
      * all registered file descriptors.
      *
      * @param events where to hold events
      * 
      * @param n_events the size of @p events in elements
      * 
      * @param timeout_ms how long to wait, in milliseconds, for an
      * event to occur.  -1 means to wait forever.  If @p
      * out_was_timeout is valid and the timeout occurs then it will
      * be set to true.
      * 
      * @param out_was_timeout if timeout_ms expires and this is not
      * NULL then the boolean pointed to will be set to true.
      */
     bool ae_event_wait(ae_res_t *e, ae_event_t *self,
                        struct epoll_event *events,
                        size_t n_events,
                        int timeout_ms,
                        bool *out_was_timeout);

#ifdef __cplusplus
}
#endif


#endif
