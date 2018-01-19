#ifndef _OPT_H
#define _OPT_H

#include <ae/res.h>

#include <stdio.h>

typedef enum ae_opt_callback_reason
{
     AE_OPT_CALLBACK_REASON_UNKNOWN_ARGUMENT = 0,
     AE_OPT_CALLBACK_REASON_ENDMARKER
} ae_opt_callback_reason_t;

typedef enum ae_opt_required
{
     AE_OPT_NONE=0,
     AE_OPT_OPTIONAL,     
     AE_OPT_REQUIRED,
     AE_OPT_BOOL,
     AE_OPT_INT,

     AE_OPT_GROUP,
     AE_OPT_ENDMARKER,
} ae_opt_required_t;


struct ae_opt;
struct ae_opt_option;
typedef bool (*ae_opt_option_callback_t)(ae_res_t *e,
                                         struct ae_opt *self,
                                         const struct ae_opt_option *option,
                                         const char *arg);

typedef bool (*ae_opt_callback_t)(ae_res_t *e,
                                  struct ae_opt *self,
                                  void *ctx,
                                  const char *argument,
                                  ae_opt_callback_reason_t reason);

typedef struct ae_opt_option
{
     const char *name;
     const char *help;     
     ae_opt_required_t required;

     void *in;
     void *out;

     ae_opt_option_callback_t callback;
     void *ctx;
} ae_opt_option_t;


typedef struct ae_opt
{
     const char *program_name;
     const char *help;

     ae_opt_callback_t callback;
     void *ctx;
     
     
     size_t options_len;
     const ae_opt_option_t *options;

     /* processing state */
     int optind;
} ae_opt_t;

#ifdef __cplusplus
extern "C" {
#endif

     /* there is no uninit */
     bool ae_opt_init(ae_res_t *e, ae_opt_t *self,
                      const char *name,
                      const char *help,
                      ae_opt_callback_t cb,
                      void *ctx,
                      size_t options_len,
                      const ae_opt_option_t *options);

     bool ae_opt_process(ae_res_t *e, ae_opt_t *self, int argc, char **argv);

     void ae_opt_help_print(const ae_opt_t *self, FILE *out);

#ifdef __cplusplus
}
#endif


#endif
