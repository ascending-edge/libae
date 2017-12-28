#ifndef _AE_BUF_H
#define _AE_BUF_H

#include <alloca.h>

#include <ae/types.h>
#include <ae/res.h>


typedef struct ae_buf
{
	size_t len;
	void *ptr;
	uint8_t buf[];
} ae_buf_t;

#define AE_BUF_ALLOCA(size)                                           \
	ae_buf_init_from_self(alloca(sizeof(ae_buf_t) + size), size)


#ifdef __cplusplus
extern "C" {
#endif

	ae_buf_t* ae_buf_init_from_self(ae_buf_t *self, size_t len);

	ae_buf_t* ae_buf_init(ae_buf_t *self, void *buf, size_t len);

	bool ae_buf_copy(ae_res_t *e, ae_buf_t *dest, const ae_buf_t *src);
	bool ae_buf_copyp(ae_res_t *e, ae_buf_t *dest,
                       size_t src_len, const void *src);
	
#ifdef __cplusplus
}
#endif


#endif
