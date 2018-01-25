#include <ae/pool.h>
#include <sys/mman.h>
#include <unistd.h>

#include <ae/log.h>
#include <ae/try.h>

#define ALIGN_PTR(p, a)                                                 \
     (uint8_t *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))


bool ae_pool_uninit(ae_res_t *e, ae_pool_t *self)
{
     if(munmap(self->start, self->len) == -1)
     {
          ae_res_warn(e, "munmap: %s", strerror(errno));
          return true;
     }
     return true;
}



bool ae_pool_init(ae_res_t *e, ae_pool_t *self)
{
     self->len = getpagesize();
     self->start = mmap(NULL, self->len,
                        PROT_READ|PROT_WRITE,
                        MAP_PRIVATE|MAP_ANONYMOUS,
                        -1, 0);
     if(self->start == MAP_FAILED)
     {
          ae_res_err(e, "mmap: %s", strerror(errno));
          return false;
     }
     self->next = self->start;
     /* cache this value. */
     self->end = self->start + self->len;
     return true;
}


bool ae_pool_remap(ae_res_t *e, ae_pool_t *self, size_t more)
{
     ae_res_err(e, "%s is not yet implemented", __FUNCTION__);
     return false;
}

bool ae_pool_alloc(ae_res_t *e, ae_pool_t *self, void *_out, size_t len)
{
     /* DO NOT FORGET ABOUT ALIGNMENT! */
     /* Let's make the potentially false assumption that we'll only be
      * on 32 or 64 bit architectures */
#if __WORDSIZE == 64
     int align = 16;
#elif __WORDSIZE == 32
     int align = 8;
#else
#error    "you must update for this word size"     
#endif

     uint8_t *next = ALIGN_PTR(self->next, align);
     printf("next=%p", next);
     /* 
        base
        prev
        next
        end
      */
     if((next + len) > self->end)
     {
          AE_TRY(ae_pool_remap(e, self, len));
     }

     /*  */
     uint8_t **out = _out;
     *out = next;
     self->next = next + len;
     return true;
}


bool ae_pool_realloc(ae_res_t *e, ae_pool_t *self,
                     void *src, size_t src_len,
                     void *_dest, size_t dest_len)
{
     ae_res_err(e, "%s: not yet implemented", __FUNCTION__);
     return false;
}



bool ae_pool_strdup(ae_res_t *e, ae_pool_t *self,
                    const char *src, char **out)
{
     ae_res_err(e, "not yet implemented");
     return false;
}

