#ifndef _AE_PTRARRAY_H
#define _AE_PTRARRAY_H

#include <ae/pool.h>

typedef struct ae_ptrarray
{
     ae_pool_t *pool;
     size_t len;
     char **data;
     size_t capacity;
} ae_ptrarray_t;

#ifdef __cplusplus
extern "C" {
#endif

     bool ae_ptrarray_init(ae_res_t *e, ae_ptrarray_t *self,
                                    ae_pool_t *pool, size_t initial_size);
     bool ae_ptrarray_append(ae_res_t *e, ae_ptrarray_t *self,
                                      void *item);
     void ae_ptrarray_remove(ae_ptrarray_t *self, size_t index);
     bool ae_ptrarray_grow_capacity(ae_res_t *e, ae_ptrarray_t *self, 
                                             size_t increase_by);
     bool ae_ptrarray_grow(ae_res_t *e, ae_ptrarray_t *self, size_t increase_by);
     void ae_ptrarray_shrink(ae_ptrarray_t *self, size_t new_size);
     bool ae_ptrarray_dup(ae_res_t *e, const ae_ptrarray_t *self,
                          ae_pool_t *pool,
                          ae_ptrarray_t *out);

#       define ae_ptrarray_at(self, index)      \
     (void*)((self)->data[(index)])

#       define ae_ptrarray_set(self, index, val)  \
     (self)->data[(index)] = (void*)(val)


     

#ifdef __cplusplus
}
#endif


#endif
