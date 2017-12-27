#ifndef _POOL_H
#define _POOL_H

#include <ae/res.h>
#include <ae/note.h>

#include <malloc.h>
#include <obstack.h>

#define obstack_chunk_alloc malloc
#define obstack_chunk_free free

typedef struct obstack ae_pool_t;

#ifdef __cplusplus
extern "C" {
#endif

     ae_res_t ae_pool_init(ae_note_t *e, ae_pool_t *self);
     ae_res_t ae_pool_alloc(ae_note_t *e, ae_pool_t *self,
                            void *out, size_t len);
     ae_res_t ae_pool_realloc(ae_note_t *e, ae_pool_t *self,
                              void *src, size_t src_len,
                              void *dest, size_t dest_len);
     ae_res_t ae_pool_uninit(ae_note_t *e, ae_pool_t *self);

#ifdef __cplusplus
}
#endif


#endif
