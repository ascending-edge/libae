#include <ae/res.h>
#include <ae/misc.h>
#include <ae/str.h>
#include <ae/try.h>

#include <stdlib.h>


/** 
 * Initializes an ae_res_msg_t
 */
static void ae_res_msg_init(ae_res_msg_t *self)
{
     self->count = 0;
     self->head = self->msg;
}


/** 
 * Adds to a result message using a va_list
 */
static void ae_res_msg_addv(ae_res_msg_t *self, const char *fmt, va_list args)
{
     size_t remain = sizeof(self->msg) - (self->head - self->msg);
     if(!remain)
     {
          return;
     }

     if(self->count)
     {
          *self->head = '\n';
          ++self->head;
          --remain;
     }
     self->head += ae_str_printv(self->head, remain, fmt, args);
     ++self->count;
}


void ae_res_clear(ae_res_t *self)
{
     for(size_t i=0; i<AE_ARRAY_LEN(self->msg); ++i)
     {
          ae_res_msg_init(&self->msg[i]);
     }
}


void ae_res_init(ae_res_t *self)
{
     self->mask = 0;
     ae_res_clear(self);
}


void ae_res_addv(ae_res_t *self, ae_res_n_t type, const char *fmt, va_list args)
{
     self->mask |= (1 << (type-1));
     ae_res_msg_addv(&self->msg[type], fmt, args);
}


void ae_res_add(ae_res_t *self, ae_res_n_t type, const char *fmt, ...)
{
     va_list args;
     va_start(args, fmt);
     ae_res_addv(self, type, fmt, args);
     va_end(args);
}


