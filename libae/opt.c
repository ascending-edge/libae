#include <ae/opt.h>
#include <ae/try.h>

#include <ae/misc.h>

#include <stdio.h>


bool ae_opt_init(ae_res_t *e, ae_opt_t *self,
                 const char *name,
                 const char *help,
                 ae_opt_callback_t cb,
                 void *ctx,
                 size_t options_len,
                 const ae_opt_option_t *options)
{
     self->program_name = name;
     self->help = help;
     self->callback = cb;
     self->ctx = ctx;
     self->options_len = options_len;
     self->options = options;
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


static bool ae_opt_parse_bool(ae_res_t *e, ae_opt_t *self,
                              const ae_opt_option_t *option,
                              const char *arg)
{
     const char *AFFIRMATIVE[] = {
          "true",
          "yes",
          "1",
     };
     const char *NEGATIVE[] = {
          "false",
          "no",
          "0"
     };

     bool *out = option->out;
     if(!out)
     {
          ae_res_err(e, "programmer error: null output argument");
          return false;
     }

     if(!arg)
     {
          *out = true;
          return true;
     }
     
     for(size_t i=0; i<AE_ARRAY_LEN(AFFIRMATIVE); ++i)
     {
          if(strcmp(arg, AFFIRMATIVE[i]) == 0)
          {
               *out = true;
               return true;
          }
     }

     for(size_t i=0; i<AE_ARRAY_LEN(NEGATIVE); ++i)
     {
          if(strcmp(arg, NEGATIVE[i]) == 0)
          {
               *out = false;
               return true;
          }
     }

     ae_res_err(e, "'%s' must be a boolean value", option->name);
     return false;
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


static bool ae_opt_option_process(ae_res_t *e, ae_opt_t *self,
                                  int argc, char **argv,
                                  const ae_opt_option_t *option)
{
     const char *arg = ae_opt_arg_get(self, argc, argv);

     /* Deal with argument logic */
     bool arg_ok = false;
     switch(option->required)
     {
     case AE_OPT_NONE:
          if(arg)
          {
               ae_res_err(e, "'%s' must not have an argument", option->name);
               return false;
          }
          break;
     case AE_OPT_REQUIRED:
          arg_ok = true;
          if(!arg)
          {
               ae_res_err(e, "'%s' option requires an argument", option->name);
               return false;
          }
          break;
     case AE_OPT_OPTIONAL:
     case AE_OPT_INT:
     case AE_OPT_BOOL:
          arg_ok = true;
          break;
     case AE_OPT_GROUP:
     case AE_OPT_ENDMARKER:
     default:
          ae_res_err(e, "programmer error! %s:%d", __FILE__, __LINE__);
          return false;
          break;
     }
     
     if(arg_ok
        && arg)
     {
          ++self->optind;
     }

     /* Is this something we handle internally? */
     switch(option->required)
     {
     case AE_OPT_BOOL:
          AE_TRY(ae_opt_parse_bool(e, self, option, arg));
          break;
     case AE_OPT_INT:
          AE_TRY(ae_opt_parse_int(e, self, option, arg));
          break;
     }

     if(option->callback)
     {
          AE_TRY(option->callback(e, self, option, arg));
     }
     return true;
}

static size_t ae_opt_group_get_longest(const ae_opt_t *self, size_t ix)
{
     /* find the longest name until a new group or the end */
     size_t longest = 0;
     for(size_t i=ix; i<self->options_len; ++i)
     {
          const ae_opt_option_t *option = &self->options[i];
          if(option->required == AE_OPT_GROUP)
          {
               return longest;
          }
          /* The option won't be printed so don't process this. */
          if(!option->help)
          {
               continue;
          }
          size_t arg_len = strlen(option->name);
          if(arg_len > longest)
          {
               longest = arg_len;
          }          
     }
     return longest;
}


static void ae_opt_help_print_option_int(const ae_opt_t *self,
                                         FILE *out,
                                         size_t longest,
                                         const ae_opt_option_t *option)
{
     const char *type = "[int]";
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

     fprintf(out, "  --%-*s - %+10s - %s%s\n",
             longest, option->name, type, option->help, params);
     
}


static void ae_opt_help_print_option(const ae_opt_t *self,
                                     FILE *out,
                                     size_t longest,
                                     const ae_opt_option_t *option)
{
     const char *type = "";
     switch(option->required)
     {
     case AE_OPT_OPTIONAL:
          type = "[string]";
          break;
     case AE_OPT_REQUIRED:
          type = "<string>";          
          break;
     case AE_OPT_BOOL:
          type = "[boolean]";
          break;
     case AE_OPT_INT:
          ae_opt_help_print_option_int(self, out, longest, option);
          return;
          break;
     }
     fprintf(out, "  --%-*s - %+10s - %s\n",
             longest, option->name, type, option->help);
}

void ae_opt_help_print(const ae_opt_t *self, FILE *out)
{
     fprintf(out, "Usage %s: %s\n", self->program_name, self->help);

     size_t longest = ae_opt_group_get_longest(self, 0);
     for(size_t i=0; i<self->options_len; ++i)
     {
          const ae_opt_option_t *option = &self->options[i];
          if(option->required == AE_OPT_GROUP)
          {
               size_t name_len = strlen(option->name);
               int pos = 40 + (name_len / 2);
               fprintf(out, "\n%*s\n", pos, option->name);
               if(self->help)
               {
                    fprintf(out, "%s\n", option->help);
               }
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
               if(option->required == AE_OPT_GROUP)
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
               /* AE_LD("comparing %s to %s", stripped, option->name); */
               if(strcmp(stripped, option->name) != 0)
               {
                    continue;
               }
               
               AE_TRY(ae_opt_option_process(e, self, argc, argv, option));
               processed = true;
          }
          if(!processed
             && self->callback)
          {
               AE_TRY(self->callback(e, self, self->ctx,
                                     arg,
                                     AE_OPT_CALLBACK_REASON_UNKNOWN_ARGUMENT));
          }
     }
     return true;
}
