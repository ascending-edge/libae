/**
 * @author Greg Rowe <greg.rowe@ascending-edge.com>
 * 
 * Copyright (C) 2018 Ascending Edge, LLC - All Rights Reserved
 */
#include <ae/buf.h>
#include <string.h>

#include <ae/buf.h>
#include <ae/try.h>


ae_buf_t* ae_buf_init_from_self(ae_buf_t *self, size_t len)
{
	self->len = len;
	self->ptr = self->buf;
	return self;
}


ae_buf_t* ae_buf_init(ae_buf_t *self,
							 void *buf,
							 size_t len)
{
	self->len = len;
	self->ptr = buf;
	return self;
}


bool ae_buf_copy(ae_res_t *e, ae_buf_t *dest, const ae_buf_t *src)
{
	if(dest->len < src->len)
	{
		ae_res_err(e, "buffer overrun detected (dest %zu, src %zu)",
                     dest->len, src->len);
		return false;
	}

	memcpy(dest->ptr, src->ptr, src->len);
	return true;
}


bool ae_buf_copyp(ae_res_t *e, ae_buf_t *dest, size_t src_len, const void *src)
{
	ae_buf_t tmp = {.len = src_len, .ptr = (void*)src};
	AE_TRY(ae_buf_copy(e, dest, &tmp));
	return true;
}
