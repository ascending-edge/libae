#ifndef _AE_IO_H
#define _AE_IO_H

#include <ae/types.h>
#include <ae/res.h>

#ifdef __cplusplus
extern "C" {
#endif

     bool ae_io_read(ae_res_t *e, int fd,
                     void *buf, size_t buf_len,
                     size_t *out_read, bool *out_was_eof);
     bool ae_io_read_all(ae_res_t *e, int fd,
                         void *buf, size_t buf_len,
                         bool *out_was_eof);

     bool ae_io_write(ae_res_t *e, int fd,
                      const void *buf, size_t buf_len,
                      size_t *out_written);
     bool ae_io_write_all(ae_res_t *e, int fd, const void *buf, size_t buf_len);

#ifdef __cplusplus
}
#endif


#endif
