#include <ae/pool.h>

#include <ae/log.h>

ae_res_t ae_pool_init(ae_note_t *e, ae_pool_t *self)
{
     obstack_init(self);
     return 0;
}


ae_res_t ae_pool_alloc(ae_note_t *e, ae_pool_t *self, void *_out, size_t len)
{
     void **out = _out;
     *out = obstack_alloc(self, len);
     if(!(*out))
     {
          ae_note_err(e, "error allocating %zu bytes", len);
          return AE_RES_ERR;
     }
     *out = obstack_finish(self);
     return 0;
}

ae_res_t ae_pool_realloc(ae_note_t *e, ae_pool_t *self,
                         void *src, size_t src_len,
                         void *_dest, size_t dest_len)
{
     void **out = _dest;
     
     /* Must reallocate! */
     if(obstack_base(self) != src)
     {
          AE_RES_TRY(ae_pool_alloc(e, self, out, dest_len));
          memcpy(*out, src, src_len);
          return 0;
     }

     if(dest_len < src_len)
     {
          ae_note_err(e, "shrinking is not supported (dest=%zu, src=%zu)",
                      dest_len, src_len);
          return AE_RES_ERR;
     }
     
     *out = src;

     /* Do nothing! */
     if(dest_len == src_len)
     {
          return 0;
     }

     /* Since we're at the top of the memory pool we can grow.    */
     obstack_blank(self, dest_len - src_len);
     return 0;
}


ae_res_t ae_pool_uninit(ae_note_t *e, ae_pool_t *self)
{
     obstack_free(self, NULL);
     return 0;
}
