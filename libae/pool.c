#include <ae/pool.h>
#include <sys/mman.h>
#include <unistd.h>

#include <ae/log.h>
#include <ae/try.h>

bool ae_pool_init(ae_res_t *e, ae_pool_t *self)
{
/* |MAP_UNINITIALIZED      */
     self->len = getpagesize();
     self->base = mmap(NULL, self->len,
                       PROT_READ|PROT_WRITE,
                       MAP_PRIVATE|MAP_ANONYMOUS,
                       -1, 0);
     if(self->base == MAP_FAILED)
     {
          ae_res_err(e, "mmap: %s", strerror(errno));
          return false;
     }
     return true;
}


bool ae_pool_alloc(ae_res_t *e, ae_pool_t *self, void *_out, size_t len)
{
     /* figure out the address is the address insize of the allocated
      * range?  If not compute how much to grow.*/
     return true;
}


bool ae_pool_realloc(ae_res_t *e, ae_pool_t *self,
                     void *src, size_t src_len,
                     void *_dest, size_t dest_len)
{
     ae_res_err(e, "%s: not yet implemented", __FUNCTION__);
     return false;
}


bool ae_pool_uninit(ae_res_t *e, ae_pool_t *self)
{
     if(munmap(self->base, self->len) == -1)
     {
          ae_res_warn(e, "munmap: %s", strerror(errno));
          return true;
     }
     return true;
}


bool ae_pool_strdup(ae_res_t *e, ae_pool_t *self,
                    const char *src, char **out)
{
     ae_res_err(e, "not yet implemented");
     return false;
}

