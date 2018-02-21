#ifndef _AE_TIME_H
#define _AE_TIME_H

#include <ae/types.h>
#include <ae/res.h>
#include <ae/try.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

     bool ae_time_get(ae_res_t *e, double *out, clockid_t id);
     bool ae_time_mono_get(ae_res_t *e, double *out);
     bool ae_time_rtc_get(ae_res_t *e, double *out);

#ifdef __cplusplus
}
#endif


#endif
