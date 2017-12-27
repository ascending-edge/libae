#ifndef _AE_NOTE_H
#define _AE_NOTE_H

#include <ae/types.h>
#include <stdarg.h>

/* this is done as a convenience since strerror(errno) is almost
 * always used with note objects. */
#include <string.h>
#include <errno.h>


typedef struct ae_note_info
{
     /**< number of messages in msg */
	uint8_t count;
     /**< used internally for where to write */
	char *head;
     /**< holds the messages */     
	char msg[4000];
} ae_note_info_t;



typedef struct ae_note
{
	ae_note_info_t info;
	ae_note_info_t warn;	
	ae_note_info_t err;
} ae_note_t;

#ifdef __cplusplus
extern "C" {
#endif

#	define AE_NOTE_INFO_INIT(info)             \
     (info)->count = 0;                         \
     (info)->head = (info)->msg

#    define AE_NOTE(self)                          \
     ae_note_t self;                               \
     AE_NOTE_INFO_INIT(&(self).info);                 \
     AE_NOTE_INFO_INIT(&(self).warn);                 \
     AE_NOTE_INFO_INIT(&(self).err)                   \

     
	void ae_note_info_add(ae_note_info_t *self, const char *fmt, ...)
#if __GNUC__
          __attribute__ ((format (printf, 2, 3)))
#endif    
          ;

     void ae_note_info_addv(ae_note_info_t *self, const char *fmt, va_list args);


#     define ae_note_info(self, ...)              \
     ae_note_info_add(&(self)->info, __VA_ARGS__)
#     define ae_note_infov(self, ...)                  \
     ae_note_info_addv(&(self)->info, __VA_ARGS__)

#     define ae_note_warn(self, ...)                   \
     ae_note_info_add(&(self)->warn,  __VA_ARGS__)
#     define ae_note_warnv(self, ...)                  \
     ae_note_info_addv(&(self)->warn,  __VA_ARGS__)

#     define ae_note_err(self, ...)               \
     ae_note_info_add(&(self)->err,  __VA_ARGS__)
#     define ae_note_errv(self,  ...)             \
     ae_n_info_addv(&(self)->err,  __VA_ARGS__)


#ifdef __cplusplus
}
#endif


#endif
