#include <ae/time.h>


bool ae_time_get(ae_res_t *e, double *out, clockid_t id)
{
     struct timespec ts;
     memset(&ts, 0, sizeof(ts));
     if(clock_gettime(id, &ts) != 0)
     {
          ae_res_err(e, "clock_gettime: %s", strerror(errno));
          return false;
     }
     *out = (double)ts.tv_sec + ((double)ts.tv_nsec/1000000000.0);
     return true;
}


bool ae_time_rtc_get(ae_res_t *e, double *out)
{
     AE_TRY(ae_time_get(e, out, CLOCK_REALTIME));
     return true;
}


bool ae_time_mono_get(ae_res_t *e, double *out)
{
     AE_TRY(ae_time_get(e, out, CLOCK_MONOTONIC));
     return true;
}




