/**
 * @author Greg Rowe <greg.rowe@ascending-edge.com>
 *
 * Copyright (C) 2018 Ascending Edge, LLC - All Rights Reserved
 */
#ifndef _AE_MUX_H
#define _AE_MUX_H

#include <sys/epoll.h>

#include <ae/res.h>
#include <ae/threadpool.h>

/**
 * This class makes it easy to use epoll (or some other multiplexing
 * mechanism).
 */
typedef struct ae_mux
{
     size_t count;              /**< The number of file descriptors
                                 * registered */
     int epoll_fd;              /**< The epoll file descriptor */
} ae_mux_t;


/**
 * Callbacks must look like this.
 *
 * @param self the event object
 * @param event the fd/event that was triggered  (see epoll.h)
 * @param ctx the context pointer that was registered with the fd/event
 */
typedef void (*ae_mux_cb_t)(ae_mux_t *self, int fd, void *ctx);

typedef struct ae_mux_handlers
{
     ae_mux_cb_t read;        /**< read event callback */
     ae_mux_cb_t write;       /**< write event callback */
     ae_mux_cb_t hangup;      /**< hangup event callback */
     ae_mux_cb_t priority;    /**< priority data event callback */
     ae_mux_cb_t error;       /**< error event callback */
} ae_mux_handlers_t;

/**
 * This is used for registering a fd/event.  Any field can be NULL.
 */
typedef struct ae_mux_event
{
     int fd;
     void *ctx;               /**< arbitrary context */
     ae_mux_handlers_t *handlers;
} ae_mux_event_t;



#ifdef __cplusplus
extern "C" {
#endif

     /**
      * Cleans up the event wrapper.  This must be called for proper
      * cleanup.
      */
     bool ae_mux_uninit(ae_res_t *e, ae_mux_t *self);


     /**
      * This prepares an event wrapper for use.  This must be called
      * prior to any other call.
      */
     bool ae_mux_init(ae_res_t *e, ae_mux_t *self);


     /**
      * Adds *or modifies* a file in the mux set.
      *
      * @param d event information
      */
     bool ae_mux_add(ae_res_t *e, ae_mux_t *self,
                     const ae_mux_event_t *d);


     /**
      * This removes a file descriptor from the event wrapper.
      *
      * @param fd the file descriptor to remove
      */
     bool ae_mux_rm(ae_res_t *e, ae_mux_t *self, int fd);



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
     bool ae_mux_wait(ae_res_t *e, ae_mux_t *self,
                      ae_threadpool_t *threadpool,
                      struct epoll_event *events,
                      size_t n_events,
                      int timeout_ms,
                      bool *out_was_timeout);

     /**
      * Waits for an event and executes the handler in the current
      * thread.
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
     bool ae_mux_wait_st(ae_res_t *e, ae_mux_t *self,
                         struct epoll_event *events,
                         size_t n_events,
                         int timeout_ms,
                         bool *out_was_timeout);


#ifdef __cplusplus
}
#endif


#endif
