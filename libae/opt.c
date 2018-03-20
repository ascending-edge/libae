/**
 * @author Greg Rowe <greg.rowe@ascending-edge.com>
 * 
 * Copyright (C) 2018 Ascending Edge, LLC - All Rights Reserved
 */
#include <ae/opt.h>
#include <ae/try.h>

#include <ae/misc.h>

#include <stdio.h>
#include <stdlib.h>

static const char *s_PARAM_BRACES[][2] = {
     {"", ""},
     {"[", "]"},     
     {"<", ">"},
};


bool ae_opt_init(ae_res_t *e, ae_opt_t *self,
                 const char *name,
                 const char *version,
                 const char *help,
                 ae_opt_callback_t cb, void *ctx,
                 size_t options_len, ae_opt_option_t *options)
{
     self->program_name = name;
     self->version = version;
     self->help = help;
     self->callback = cb;
     self->ctx = ctx;
     self->options_len = options_len;
     self->options = options;

     for(size_t i=0; i<options_len; ++i)
     {
          ae_opt_option_t *option = &options[i];
          if(option->name)
          {
               option->name_len = strlen(option->name);
          }
     }
     return true;
}


static const char* ae_opt_arg_get(const ae_opt_t *self, int argc, char **argv)
{
     if(self->optind >= argc)
     {
          return NULL;
     }
     
     const char *maybe = argv[self->optind];
     /* AE_LD("maybe: %s", maybe); */
     if((maybe[0] == '-')
        && (maybe[1] == '-'))
     {
          return NULL;
     }

     return maybe;
}


static bool ae_opt_parse_flag(ae_res_t *e, ae_opt_t *self,
                              const ae_opt_option_t *option,
                              const char *arg)
{
     if(!option->out)
     {
          ae_res_err(e, "programmer error: '%s' null output argument",
                     option->name);
          return false;

     }
     bool *out = option->out;
     *out = true;
     return true;
}


static bool ae_opt_parse_int(ae_res_t *e, ae_opt_t *self,
                              const ae_opt_option_t *option,
                              const char *arg)
{
     int *params = option->in;
     int def = 0;
     int min = 0;
     int max = 0;
     if(params)
     {
          def = params[0];
          min = params[1];
          max = params[2];
     }
     int *out = option->out;

     if(!out)
     {
          ae_res_err(e, "programmer error: '%s' null output argument",
                     option->name);
          return false;
     }

     if(arg)
     {
          if(sscanf(arg, "%d", out) != 1)
          {
               ae_res_err(e, "'%s': '%s' is not an integer", option->name, arg);
               return false;
          }
     }
     else
     {
          *out = params ? def : -1;
     }
     
     /* do range checking if parameters were specified */
     if(params)
     {
          if((*out < min)
             || (*out > max))
          {
               ae_res_err(e, "'%s': %d is out of range (%d-%d)",
                          option->name, *out, min, max);
               return false;
          }
     }
     return true;
}


static bool ae_opt_parse_double(ae_res_t *e, ae_opt_t *self,
                                const ae_opt_option_t *option,
                                const char *arg)
{
     double *params = option->in;
     double def = 0;
     double min = 0;
     double max = 0;
     if(params)
     {
          def = params[0];
          min = params[1];
          max = params[2];
     }
     double *out = option->out;

     if(!out)
     {
          ae_res_err(e, "programmer error: '%s' null output argument",
                     option->name);
          return false;
     }

     if(arg)
     {
          if(sscanf(arg, "%lg", out) != 1)
          {
               ae_res_err(e, "'%s': '%s' is not a valid value",
                          option->name, arg);
               return false;
          }
     }
     else
     {
          *out = params ? def : -1;
     }
     
     /* do range checking if parameters were specified */
     if(params)
     {
          if((*out < min)
             || (*out > max))
          {
               ae_res_err(e, "'%s': %g is out of range (%g - %g)",
                          option->name, *out, min, max);
               return false;
          }
     }
     return true;
}


static bool ae_opt_parse_bit(ae_res_t *e, ae_opt_t *self,
                             const ae_opt_option_t *option,
                             const char *arg)
{
     uintptr_t which_bits = (uintptr_t)option->in;
     unsigned int *out = option->out;

     if(!out)
     {
          ae_res_err(e, "programmer error: '%s' null output argument",
                     option->name);
          return false;
     }

     *out |=  which_bits;
     return true;
}


static bool ae_opt_parse_counter(ae_res_t *e, ae_opt_t *self,
                                 const ae_opt_option_t *option,
                                 const char *arg)
{
     int *out = option->out;

     if(!out)
     {
          ae_res_err(e, "programmer error: '%s' null output argument",
                     option->name);
          return false;
     }

     if(!arg)
     {
          ++(*out);
          return true;
     }

     int tmp = 0;
     if(sscanf(arg, "%d", &tmp) != 1)
     {
          ae_res_err(e, "'%s': '%s' is not an integer", option->name, arg);
          return true;
     }
     *out += tmp;
     return true;
}


static bool ae_opt_parse_string(ae_res_t *e, ae_opt_t *self,
                                const ae_opt_option_t *option,
                                const char *arg)
{
     char **out = option->out;
     *out = (char*)arg;
     return true;
}


static bool ae_opt_option_process(ae_res_t *e, ae_opt_t *self,
                                  const char *param,
                                  const ae_opt_option_t *option)
{
     /* Deal with argument logic */
     bool arg_ok = (option->is_required == AE_OPT_PARAM_NONE) ? false:true;
     if(arg_ok
        && param)
     {
          ++self->optind;
     }

     if(option->callback)
     {
          AE_TRY(option->callback(e, self, option, param));
     }

     /* Is this something we handle internally? */
     switch(option->type)
     {
     case AE_OPT_TYPE_HELP:
          ae_opt_help_print(self, stdout);
          exit(0);
          break;
     case AE_OPT_TYPE_VERSION:
          ae_opt_version_print(self, stdout);
          exit(0);
          break;
     case AE_OPT_TYPE_FLAG:
          AE_TRY(ae_opt_parse_flag(e, self, option, param));
          break;
     case AE_OPT_TYPE_INT:
          AE_TRY(ae_opt_parse_int(e, self, option, param));
          break;
     case AE_OPT_TYPE_DOUBLE:
          AE_TRY(ae_opt_parse_double(e, self, option, param));
          break;          
     case AE_OPT_TYPE_STRING:
          AE_TRY(ae_opt_parse_string(e, self, option, param));
          break;
     case AE_OPT_TYPE_BIT:
          AE_TRY(ae_opt_parse_bit(e, self, option, param));
          break;
     case AE_OPT_TYPE_COUNTER:
          AE_TRY(ae_opt_parse_counter(e, self, option, param));
          break;
     }

     if(option->callback)
     {
          AE_TRY(option->callback(e, self, option, param));
     }
     return true;
}


static int ae_opt_format_option_field1(const ae_opt_option_t *option,
                                       size_t out_len,
                                       char *out)
{
     char param_help[256];
     param_help[0] = '\0';
     if(option->is_required != AE_OPT_PARAM_NONE)
     {
          sprintf(param_help, "=%s%s%s",
                  s_PARAM_BRACES[option->is_required][0],
                  option->help_param ? option->help_param : "",
                  s_PARAM_BRACES[option->is_required][1]
               );
     }
     return snprintf(out, out_len, "%s%s", option->name, param_help);
}


static size_t ae_opt_field1_width(const ae_opt_option_t *option)
{
     char field1[512];
     ae_opt_format_option_field1(option, sizeof(field1), field1);
     return strlen(field1);
}


static size_t ae_opt_option_field1_width(const ae_opt_option_t *option)
{
     switch(option->type)
     {
     case AE_OPT_TYPE_INT:
     case AE_OPT_TYPE_STRING:
          return ae_opt_field1_width(option);
          break;
     default:
          return option->name_len;
     }
     return 0;
}


static size_t ae_opt_group_get_longest(const ae_opt_t *self, size_t ix)
{
     /* find the longest name until a new group or the end */
     size_t longest = 0;
     /* printf("ix=%zu len=%zu\n", ix, self->options_len); */
     for(size_t i=ix; i<self->options_len; ++i)
     {
          const ae_opt_option_t *option = &self->options[i];
          if(option->type == AE_OPT_TYPE_GROUP)
          {
               return longest;
          }
          /* The option won't be printed so don't process this. */
          if(!option->help)
          {
               continue;
          }
          size_t arg_len = ae_opt_option_field1_width(option);
          if(arg_len > longest)
          {
               longest = arg_len;
          }          
     }
     /* printf("longest: %zu", longest); */
     return longest;
}


static void ae_opt_help_print_option_int(const ae_opt_t *self,
                                         FILE *out,
                                         size_t longest,
                                         const ae_opt_option_t *option)
{
     /* min, max, default */
     char params[256] = {0};
     if(option->in)
     {
          int *in = option->in;
          int def = in[0];
          int min = in[1];
          int max = in[2];
          sprintf(params, " (min: %d, max: %d, default: %d)", min, max, def);
     }

     char field1[512];
     ae_opt_format_option_field1(option, sizeof(field1), field1);
     
     fprintf(out, "  --%-*s - %s%s\n",
             longest,
             field1,
             option->help, params);
}


static void ae_opt_help_print_option_double(const ae_opt_t *self,
                                            FILE *out,
                                            size_t longest,
                                            const ae_opt_option_t *option)
{
     /* min, max, default */
     char params[256] = {0};
     if(option->in)
     {
          double *in = option->in;
          double def = in[0];
          double min = in[1];
          double max = in[2];
          sprintf(params, " (min: %g, max: %g, default: %g)", min, max, def);
     }

     char field1[512];
     ae_opt_format_option_field1(option, sizeof(field1), field1);
     
     fprintf(out, "  --%-*s - %s%s\n",
             longest,
             field1,
             option->help, params);
}


static void ae_opt_help_print_option_string(const ae_opt_t *self,
                                            FILE *out,
                                            size_t longest,
                                            const ae_opt_option_t *option)
{
     char field1[512];
     ae_opt_format_option_field1(option, sizeof(field1), field1);
     
     fprintf(out, "  --%-*s - %s\n",
             longest,
             field1,
             option->help);
}


static void ae_opt_help_print_option_counter(const ae_opt_t *self,
                                            FILE *out,
                                            size_t longest,
                                            const ae_opt_option_t *option)
{
     char field1[512];
     ae_opt_format_option_field1(option, sizeof(field1), field1);
     
     fprintf(out, "  --%-*s - %s\n",
             longest,
             field1,
             option->help);
}


static void ae_opt_help_print_option(const ae_opt_t *self,
                                     FILE *out,
                                     size_t longest,
                                     const ae_opt_option_t *option)
{
     switch(option->type)
     {
     case AE_OPT_TYPE_INT:
          ae_opt_help_print_option_int(self, out, longest, option);
          return;
          break;
     case AE_OPT_TYPE_DOUBLE:
          ae_opt_help_print_option_double(self, out, longest, option);
          return;
          break;          
     case AE_OPT_TYPE_STRING:
          ae_opt_help_print_option_string(self, out, longest, option);
          return;
     case AE_OPT_TYPE_COUNTER:
          ae_opt_help_print_option_counter(self, out, longest, option);
          return;          
          break;
     }
     fprintf(out, "  --%-*s - %s\n", longest, option->name, option->help);
}


void ae_opt_version_print(const ae_opt_t *self, FILE *out)
{
     fprintf(out, "%s %s\n", self->program_name, self->version);
}


static void ae_opt_help_print_group(const ae_opt_option_t *option, FILE *out)
{
     size_t name_len = strlen(option->name);
     int pos = 40 + (name_len / 2);
     fprintf(out, "\n%*s\n", pos, option->name);
     if(option->help)
     {
          fprintf(out, "%s\n", option->help);
     }
}


void ae_opt_help_print(const ae_opt_t *self, FILE *out)
{
     fprintf(out, "Usage %s: %s\n", self->program_name, self->help);

     size_t longest = ae_opt_group_get_longest(self, 0);
     for(size_t i=0; i<self->options_len; ++i)
     {
          const ae_opt_option_t *option = &self->options[i];
          if(option->type == AE_OPT_TYPE_GROUP)
          {
               ae_opt_help_print_group(option, out);
               if(i + 1 < self->options_len)
               {
                    longest = ae_opt_group_get_longest(self, i + 1);
               }
               continue;
          }
          if(!option->help)
          {
               continue;
          }
          ae_opt_help_print_option(self, out, longest, option);
     }
}


static bool ae_opt_argument_is_valid(const char *arg)
{
     /* Require double dash -- */
     size_t arg_len = strlen(arg);
     if(arg_len < 2)
     {
          return false;
     }
     if((arg[0] != '-')
        || (arg[1] != '-'))
     {
          return false;
     }
     return true;
}

static bool ae_opt_default_callback(ae_res_t *e, ae_opt_t *self,
                             const char *argument,
                             ae_opt_callback_reason_t reason)
{
     switch(reason)
     {
     case AE_OPT_CALLBACK_REASON_UNKNOWN_ARGUMENT:
          ae_res_err(e, "unknown option: %s", argument);
          return false;
          break;
     default:
          break;
     }
     return true;
}

                           


bool ae_opt_process(ae_res_t *e, ae_opt_t *self, int argc, char **argv)
{
     /* this is a normal condition, done processing. */
     if(self->optind > argc)
     {
          ae_res_err(e, "option processing is complete (optind: %d, argc: %d)",
                     self->optind, argc);
          return false;
     }

     while(self->optind < argc)
     {
          const char *arg = argv[self->optind];
          ++self->optind;

          /* end of argument processing */
          if(strcmp(arg, "--") == 0)
          {
               return true;
          }

          bool processed = false;
          for(size_t j=0; j<self->options_len; ++j)
          {
               const ae_opt_option_t *option = &self->options[j];
               if(option->type == AE_OPT_TYPE_GROUP)
               {
                    /* AE_LD("skipping group"); */
                    continue;
               }
               /* not an argument so stop processing I guess. */
               if(!ae_opt_argument_is_valid(arg))
               {
                    --self->optind;
                    return true;
               }
               
               const char *stripped = &arg[2];
               char *pos = strchr(stripped, '=');
               size_t limit = pos ? (pos - stripped) : strlen(stripped);

               if(limit != option->name_len)
               {
                    continue;
               }

               if(strncmp(option->name, stripped, limit) != 0)
               {
                    continue;
               }
               const char *param = pos ?
                    (pos+1) : ae_opt_arg_get(self, argc, argv);
               
               AE_TRY(ae_opt_option_process(e, self, param, option));
               processed = true;
          }
          if(!processed)
          {
               if(self->callback)
               {
                    AE_TRY(self->callback(
                                e, self,
                                self->ctx,
                                arg,
                                AE_OPT_CALLBACK_REASON_UNKNOWN_ARGUMENT));
               }
               else
               {
                    AE_TRY(ae_opt_default_callback(
                                e, self,
                                arg,
                                AE_OPT_CALLBACK_REASON_UNKNOWN_ARGUMENT));
               }
          }
     }
     return true;
}
