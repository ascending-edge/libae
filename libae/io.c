#include <ae/io.h>

#include <unistd.h>

#include <ae/try.h>

bool ae_io_read(ae_res_t *e, int fd,
                void *buf, size_t buf_len,
                size_t *out_read, bool *out_was_eof)
{
     ssize_t res = read(fd, buf, buf_len);
     if(res < 0)
     {
          ae_res_err(e, "read: %s", strerror(errno));
          return false;
     }
     if(out_read)
     {
          *out_read = res;
     }
     
     if(res == 0)
     {
          if(out_was_eof)
          {
               *out_was_eof = true;
          }
          return true;
     }
     return true;
}


bool ae_io_write(ae_res_t *e, int fd,
                 const void *buf, size_t buf_len,
                 size_t *out_written)
{
     errno = 0;
     ssize_t res = write(fd, buf, buf_len);
     if(res < 0)
     {
          goto error_exit;
     }
     if(out_written)
     {
          *out_written = res;
     }
     
     if((res == 0)
        && (errno != 0))
     {
          goto error_exit;
     }
     return true;

error_exit:
     ae_res_err(e, "read: %s", strerror(errno));
     return false;
     
}


bool ae_io_read_all(ae_res_t *e, int fd, void *buf, size_t buf_len,
                    bool *out_was_eof)
{
     uint8_t *iter = buf;
     while(buf_len)
     {
          size_t bytes_read = 0;
          AE_TRY(ae_io_read(e, fd, iter, buf_len, &bytes_read, out_was_eof));
          if(out_was_eof
             && *out_was_eof)
          {
               return true;
          }
          iter += bytes_read;
          buf_len -= bytes_read;
     }
     return true;
}


bool ae_io_write_all(ae_res_t *e, int fd, const void *buf, size_t buf_len)
{
     const uint8_t *iter = buf;
     while(buf_len)
     {
          size_t bytes_written = 0;
          AE_TRY(ae_io_write(e, fd, iter, buf_len, &bytes_written));
          iter += bytes_written;
          buf_len -= bytes_written;
     }
     return true;
}
