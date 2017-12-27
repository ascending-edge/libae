#include <ae/buf.h>
#include <string.h>

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


ae_res_t ae_buf_copy(ae_note_t *e,
							ae_buf_t *dest,
							const ae_buf_t *src)
{
	if(dest->len < src->len)
	{
		ae_note_err(e, "buffer overrun detected (dest %zu, src %zu)",
						dest->len, src->len);
		return AE_RES_ERR;
	}

	memcpy(dest->ptr, src->ptr, src->len);
	return AE_RES_OK;
}


ae_res_t ae_buf_copyp(ae_note_t *e,
							 ae_buf_t *dest,
							 size_t src_len,
							 const void *src)
{
	ae_buf_t tmp = {.len = src_len, .ptr = (void*)src};
	AE_RES_TRY(ae_buf_copy(e, dest, &tmp));
	return AE_RES_OK;
}
