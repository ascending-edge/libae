#ifndef _POOL_H
#define _POOL_H

#include <ae/res.h>

typedef struct ae_pool
{
     size_t len;
     uint8_t *base;

     size_t bytes_allocated;
     uint8_t *next;
} ae_pool_t;



#ifdef __cplusplus
extern "C" {
#endif

     bool ae_pool_uninit(ae_res_t *e, ae_pool_t *self);     
     bool ae_pool_init(ae_res_t *e, ae_pool_t *self);

     bool ae_pool_alloc(ae_res_t *e, ae_pool_t *self, void *out, size_t len);
     bool ae_pool_realloc(ae_res_t *e, ae_pool_t *self,
                          void *src, size_t src_len,
                          void *dest, size_t dest_len);
     bool ae_pool_strdup(ae_res_t *e, ae_pool_t *self,
                         const char *src, char **out);


#ifdef __cplusplus
}
#endif


#endif
