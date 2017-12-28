#include <ae/pool.h>

#include <ae/log.h>
#include <ae/try.h>

bool ae_pool_init(ae_res_t *e, ae_pool_t *self)
{
     obstack_init(self);
     return true;
}


bool ae_pool_alloc(ae_res_t *e, ae_pool_t *self, void *_out, size_t len)
{
     void **out = _out;
     *out = obstack_alloc(self, len);
     if(!(*out))
     {
          ae_res_err(e, "error allocating %zu bytes", len);
          return false;
     }
     *out = obstack_finish(self);
     return true;
}


bool ae_pool_realloc(ae_res_t *e, ae_pool_t *self,
                     void *src, size_t src_len,
                     void *_dest, size_t dest_len)
{
     void **out = _dest;
     
     /* Must reallocate! */
     if(obstack_base(self) != src)
     {
          AE_TRY(ae_pool_alloc(e, self, out, dest_len));
          memcpy(*out, src, src_len);
          return true;
     }

     if(dest_len < src_len)
     {
          ae_res_err(e, "shrinking is not supported (dest=%zu, src=%zu)",
                     dest_len, src_len);
          return false;
     }
     
     *out = src;

     /* Do nothing! */
     if(dest_len == src_len)
     {
          return true;
     }

     /* Since we're at the top of the memory pool we can grow.    */
     obstack_blank(self, dest_len - src_len);
     return true;
}


bool ae_pool_uninit(ae_res_t *e, ae_pool_t *self)
{
     obstack_free(self, NULL);
     return true;
}
