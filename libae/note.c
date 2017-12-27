#include <ae/note.h>

#include <stddef.h>

#include <ae/str.h>

void ae_note_info_addv(ae_note_info_t *self, const char *fmt, va_list args)
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

void ae_note_info_add(ae_note_info_t *self, const char *fmt, ...)
{
     if(!self)
     {
          return;
     }
     
     va_list args;
     va_start(args, fmt);
     ae_note_info_addv(self, fmt, args);
     va_end(args);
}
