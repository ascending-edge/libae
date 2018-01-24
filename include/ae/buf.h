/**
 * @author Greg Rowe <greg.rowe@ascending-edge.com>
 *
 * Copyright (C) 2018 Ascending Edge, LLC - All Rights Reserved
 */
#ifndef _AE_BUF_H
#define _AE_BUF_H

#include <alloca.h>

#include <ae/types.h>
#include <ae/res.h>


/**
 * I'm considering removing this.  This seems more confusing than
 * useful.  In practice it's not very difficult to pass around a
 * size_t alongside each pointer.
 *
 * This is supposed to encapsulate a buffer with a length -- any chunk
 * of RAM.
 */
typedef struct ae_buf
{
	size_t len;                /**< the length of the buffer */
	void *ptr;                 /**< pointer to anything */
	uint8_t buf[];             /**< easy way to have `len` near data
                                 * (for better cache performance)*/
} ae_buf_t;


/** 
 * This allocates and initializes an ae_buf using alloca on the stack.
 *
 * @param size how much RAM to allocate
 *
 * @return the allocated and initialized ae_buf object
 */
#define AE_BUF_ALLOCA(size)                                           \
	ae_buf_init_from_self(alloca(sizeof(ae_buf_t) + size), size)


#ifdef __cplusplus
extern "C" {
#endif

     /** 
      * This initializes the buffer to point to the internal pointer.
      * Use this if you allocate your buffer space immediately
      * following the ae_buf.  This sets the `ptr` field to the `data`
      * field (which is the first byte following the struct).
      *
      * If your buffer exists in another location use #ae_buf_init
      * instead.
      *
      * @see #AE_BUF_ALLOCA
      *
      * @param len the length of the buffer (be sure you've allocated
      * enough space beyond the struct!).
      *
      * @return the initialized buffer (@p self)
      */
	ae_buf_t* ae_buf_init_from_self(ae_buf_t *self, size_t len);


     /** 
      * This initializes the buffer to point to some block of RAM.  If
      * the block of RAM is immediately following this struct use
      * #ae_buf_init_from_self instead.
      *
      * @param buf pointer to wrap
      * 
      * @param len number of bytes available in @p buf
      *
      * @return the initialized buffer (@p self)
      */
	ae_buf_t* ae_buf_init(ae_buf_t *self, void *buf, size_t len);


     /** 
      * Copies one ae_buf object to another @p src to @p dest.  If you
      * have an unwrapped buffer use #ae_buf_copyp.
      *
      * @param dest
      * 
      * @param src 
      */
	bool ae_buf_copy(ae_res_t *e, ae_buf_t *dest, const ae_buf_t *src);


     /** 
      * Copies data into this object.
      *
      * @param dest 
      * @param src_len length, in bytes, of @p src
      * @param src 
      */
	bool ae_buf_copyp(ae_res_t *e, ae_buf_t *dest,
                       size_t src_len, const void *src);
	
#ifdef __cplusplus
}
#endif


#endif
