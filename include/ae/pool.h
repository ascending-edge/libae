#ifndef _POOL_H
#define _POOL_H

#include <ae/res.h>

#include <malloc.h>

#if HAVE_OBSTACK_H
#    include <obstack.h>
#    define obstack_chunk_alloc malloc
#    define obstack_chunk_free free
typedef struct obstack ae_pool_t;
#else
/* just a placeholder... */
typedef int ae_pool_t;
#endif  /* HAVE_OBSTACK_H */




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
