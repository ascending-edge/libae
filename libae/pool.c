#include <ae/pool.h>
#include <sys/mman.h>
#include <unistd.h>

#include <ae/log.h>
#include <ae/try.h>

#include <stdio.h>

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



bool ae_pool_init(ae_res_t *e, ae_pool_t *self, size_t initial_size)
{
     size_t page_size = getpagesize();     
     size_t pages_needed = (initial_size + page_size - 1) / page_size;
     self->len = pages_needed * page_size;
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
     self->end = self->start + self->len;
     return true;
}


static bool ae_pool_remap(ae_res_t *e, ae_pool_t *self, size_t more)
{
     /* how many pages do we need? */
     /* void *mremap(void *old_address, size_t old_size, */
     /*              size_t new_size, int flags, ... /\* void *new_address *\/); */
     size_t page_size = getpagesize();
     /* printf("page size = %zu\n", page_size); */
     size_t new_total = self->len + more;
     /* printf("new_total = %zu len+more=%zu more=%zu\n", */
     /*        new_total, self->len + more, more); */
     size_t pages_needed = (new_total + page_size - 1) / page_size;
     /* printf("pages_needed = %zu\n", pages_needed); */
     size_t new_len = pages_needed * page_size;
     /* printf("start=%p len=%zu new_len=%zu\n", */
     /*        self->start, self->len, new_len); */

     /* crap...without "may move" flag this doesn't generally
      * work... We can't have the address change or every pointer from
      * the pool will not work. */
     uint8_t *new = mremap(self->start, self->len, new_len, 0);
     if(new == MAP_FAILED)
     {
          ae_res_err(e, "memory remap failure: %s", strerror(errno));
          return false;
     }

     if(new != self->start)
     {
          /* This presents a problem since munmap should happen. */
          ae_res_err(e, "remap did not return expected address!"
                     " (expected=%p, got=%p)",
                     self->start, new);
          return false;
     }

     /* There's no need to reassign self->start */
     self->len = new_len;
     self->end = self->start + self->len;
     return true;
}

bool ae_pool_alloc(ae_res_t *e, ae_pool_t *self, void *_out, size_t len)
{
     /* DO NOT FORGET ABOUT ALIGNMENT! */
     /* Let's make the potentially false assumption that we'll only be
      * on 32 or 64 bit architectures */
#ifdef _LP64 == 64
     int align = 16;
#else
     int align = 8;
#endif

     uint8_t *next = ALIGN_PTR(self->next, align);
     /* printf("next=%p\n", next); */
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
                     void *_new, size_t new_len)
{
     /* We can (maybe) grow in place. */
     if(src == (self->next - src_len))
     {
          ae_res_err(e, "this special case hasn't been implemented");
          return false;
     }

     /* It's not possible to grow in place so allocate a new block of
      * memory. */
     uint8_t *blob = NULL;
     AE_TRY(ae_pool_alloc(e, self, &blob, new_len));
     memcpy(blob, src, src_len);
     uint8_t **out = _new;
     *out = blob;
     return true;
}



bool ae_pool_strdup(ae_res_t *e, ae_pool_t *self,
                    const char *src, ssize_t src_len,
                    char **out)
{
     size_t len = (src_len == -1) ? strlen(src) : src_len;
     ++len;
     printf("src=%s len=%zu\n", src, len);
     AE_TRY(ae_pool_alloc(e, self, out, len));
     printf("out=%p\n", *out);
     strncpy(*out, src, len);
     printf("out=%s\n", *out);
     return true;
}

