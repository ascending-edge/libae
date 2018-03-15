#ifndef _AE_EVENT_H
#define _AE_EVENT_H

#include <ae/types.h>
#include <ae/res.h>

typedef struct ae_event
{
     int fd;
} ae_event_t;

#ifdef __cplusplus
extern "C" {
#endif

     bool ae_event_uninit(ae_res_t *e, ae_event_t *self);
     bool ae_event_init(ae_res_t *e, ae_event_t *self);

     
     bool ae_event_read(ae_res_t *e, ae_event_t *self, uint64_t *out);
     bool ae_event_write(ae_res_t *e, ae_event_t *self, uint64_t val);
     


#ifdef __cplusplus
}
#endif


#endif
