/**
 * @author Greg Rowe <greg.rowe@ascending-edge.com>
 * 
 * Copyright (C) 2018 Ascending Edge, LLC - All Rights Reserved
 */
#ifndef _POOL_H
#define _POOL_H

#include <ae/res.h>

/**
 * A memory pool class.  (This is not complete!).  This uses mmap and
 * mremap to allocate multiple chunks of memory that can be freed in
 * one operation.
 */
typedef struct ae_pool
{
     size_t len;                /**< total size of memory chunk */
     uint8_t *base;             /**< base memory address */

     size_t bytes_allocated;    /**< number of bytes allocated */
     uint8_t *next;             /**< where to get next pointer from */
} ae_pool_t;



#ifdef __cplusplus
extern "C" {
#endif

     /** 
      * This destroys the memory pool
      */
     bool ae_pool_uninit(ae_res_t *e, ae_pool_t *self);


     /** 
      * This prepares the memory pool for use.
      */
     bool ae_pool_init(ae_res_t *e, ae_pool_t *self);


     /** 
      * This allocates a block of memory from the pool.
      *
      * @param out where to store the pointer
      * 
      * @param len how many bytes to allocate
      */
     bool ae_pool_alloc(ae_res_t *e, ae_pool_t *self, void *out, size_t len);


     /** 
      * Much like realloc() in the standard library this will attempt
      * to grow the @p src pointer, if possible.  If it can't it will
      * allocate a new block of memory and copy @src to @p dest.
      *
      * I'd like to remove the requirement of having the source_len
      * argument.  That would require a bit more book-keeping.
      *
      * @param src source memory block
      * @param src_len how many bytes are in @p src
      * @param dest destination memory block (really a void**)
      * @param dest_len how many bytes should be allocated
      */
     bool ae_pool_realloc(ae_res_t *e, ae_pool_t *self,
                          void *src, size_t src_len,
                          void *dest, size_t dest_len);


     /** 
      * Duplicates a string allocating memory from the pool to do so.
      *
      * @param src source string
      * @param out output string
      */
     bool ae_pool_strdup(ae_res_t *e, ae_pool_t *self,
                         const char *src, char **out);


#ifdef __cplusplus
}
#endif


#endif
