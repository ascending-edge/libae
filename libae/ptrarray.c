#include <ae/ptrarray.h>
#include <ae/try.h>


bool ae_ptrarray_grow_capacity(ae_res_t *e, ae_ptrarray_t *self,
                               size_t increase_by)
{
     size_t new_capacity = self->capacity + increase_by;
     void *new = NULL;
     AE_TRY(ae_pool_realloc(e, self->pool,
                            self->data, self->len * sizeof(self->data[0]),
                            &new, new_capacity * sizeof(self->data[0])));
     self->capacity = new_capacity;
     self->data = new;
     return true;
}


bool ps_ptrarray_grow(ae_res_t *e, ae_ptrarray_t *self, size_t increase_by)
{
     size_t new_len = self->len + increase_by;

     /* nothing more to do. */
     if(new_len < self->capacity)
     {
          ae_res_err(e, "could not grow array to %zu", new_len);
          return false;
     }

     if(new_len > self->capacity)
     {
          /* Make the capacity equal to the new length of the array. */
          AE_TRY(ae_ptrarray_grow_capacity(e, self,
                                               new_len - self->capacity));
     }

     self->len = new_len;
     return true;
}


bool ae_ptrarray_init(ae_res_t *e, ae_ptrarray_t *self,
                      ae_pool_t *pool, size_t initial_size)
{
     AE_TRY(ae_pool_alloc(e, pool, &self->data,
                              sizeof(self->data[0]) * initial_size));
     self->pool = pool;
     self->len = 0;
     self->capacity = initial_size;
     return true;
}


bool ae_ptrarray_append(ae_res_t *e, ae_ptrarray_t *self, void *item)
{
     if(self->len == self->capacity)
     {
          AE_TRY(ae_ptrarray_grow_capacity(e, self, 16));
     }

     self->data[self->len++] = item;
     return true;
}


void ae_ptrarray_remove(ae_ptrarray_t *self, size_t index)
{
     if(self->len == 0)
     {
          return;
     }

     if(self->len > 1)
     {
          self->data[index] = self->data[self->len - 1];
     }

     --self->len;
}


void ae_ptrarray_shrink(ae_ptrarray_t *self, size_t new_size)
{
     if(new_size >= self->len)
     {
          return;
     }
     self->len = new_size;
}


bool ae_ptrarray_dup(ae_res_t *e, const ae_ptrarray_t *self,
                     ae_pool_t *pool,
                     ae_ptrarray_t *out)
{
     out->pool = pool;
     out->len = self->len;
     out->capacity = self->capacity;
     size_t n_bytes = out->capacity * sizeof(out->data[0]);
     AE_TRY(ae_pool_alloc(e, pool, &out->data, n_bytes));
     memcpy(out->data, self->data, n_bytes);
     return true;
}
