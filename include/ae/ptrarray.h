/**
 * @author Greg Rowe <greg.rowe@ascending-edge.com>
 * 
 * Copyright (C) 2018 Ascending Edge, LLC - All Rights Reserved
 */
#ifndef _AE_PTRARRAY_H
#define _AE_PTRARRAY_H

#include <ae/pool.h>


/**
 * The ptrarray class is very useful for storing small collections of
 * objects.  It automatically grows to accommodate more items, if
 * necessary.
 */
typedef struct ae_ptrarray
{
     ae_pool_t *pool;           /**< where to allocate memory from */
     size_t len;                /**< number of pointers in the array */
     char **data;               /**< the pointer array */
     size_t capacity;           /**< total capacity of array */
} ae_ptrarray_t;

#ifdef __cplusplus
extern "C" {
#endif

     /** 
      * Prepare a pointer array for use
      *
      * @param pool where to allocate memory from
      * 
      * @param initial_size the initial capacity of the array
      */
     bool ae_ptrarray_init(ae_res_t *e, ae_ptrarray_t *self,
                           ae_pool_t *pool, size_t initial_size);

     /** 
      * Appends a pointer to the array, growing if necessary.
      *
      * @param item what to append
      */
     bool ae_ptrarray_append(ae_res_t *e, ae_ptrarray_t *self, void *item);


     /** 
      * Removes an item from the ptrarray and moves the last item in
      * the array to fill the missing slot.
      *
      * @param index which item to remove
      */
     void ae_ptrarray_remove(ae_ptrarray_t *self, size_t index);


     /** 
      * This forces the ptrarray to allocate enough memory to hold the
      * current capacity plus @p increase_by items.
      *
      * @param increase_by how many additional slots should be
      * allocated
      */
     bool ae_ptrarray_grow_capacity(ae_res_t *e, ae_ptrarray_t *self, 
                                    size_t increase_by);


     /** 
      * This changes the size of the ptrarray.  This will grow the
      * capacity of the array if necessary.  Unlike
      * #ae_ptrarray_grow_capacity this changes the size of the array.
      * This means that unused slots may be uninitialized.  This can
      * be used to quickly increase the size of the ptrarray prior to
      * adding a number of items directory (perhaps via memcpy).
      *
      * @see #ae_ptrarray_shrink
      *
      * @param increase_by how many more items the ptrarray should hold
      */
     bool ae_ptrarray_grow(ae_res_t *e, ae_ptrarray_t *self, size_t increase_by);


     /** 
      * This is similar to #ae_ptrarray_grow.  This will decrease the
      * length of the array without affecting the capacity.  This is a
      * very fast way to truncate the array.
      *
      * @param new_size the new length of the array
      */
     void ae_ptrarray_shrink(ae_ptrarray_t *self, size_t new_size);


     /** 
      * This duplicates the array.  This is a shallow copy.
      *
      * @param pool where to allocate memory from
      * 
      * @param out where to copy to
      */
     bool ae_ptrarray_dup(ae_res_t *e, const ae_ptrarray_t *self,
                          ae_pool_t *pool,
                          ae_ptrarray_t *out);

     /**
      * This accesses an item by index.
      *
      * @param index which item
      */
#       define ae_ptrarray_at(self, index)      \
     (void*)((self)->data[(index)])


     /**
      * This sets an item by index.
      *
      * @param index which item
      *
      * @param val the new value of the item
      */
#       define ae_ptrarray_set(self, index, val)  \
     (self)->data[(index)] = (void*)(val)


     

#ifdef __cplusplus
}
#endif


#endif
