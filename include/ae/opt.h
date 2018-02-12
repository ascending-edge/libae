/**
 * @author Greg Rowe <greg.rowe@ascending-edge.com>
 * 
 * Copyright (C) 2018 Ascending Edge, LLC - All Rights Reserved
 */
#ifndef _OPT_H
#define _OPT_H

#include <ae/res.h>
#include <stdint.h>
#include <stdio.h>

/**
 * These are the possible reasons for a global level callback.
 *
 * @todo add one for processing an argument
 */
typedef enum ae_opt_callback_reason
{
     AE_OPT_CALLBACK_REASON_UNKNOWN_ARGUMENT = 0, /**< a command line
                                                   * argument was
                                                   * specified that
                                                   * doesn't match any
                                                   * configured
                                                   * options. */
     AE_OPT_CALLBACK_REASON_ENDMARKER
} ae_opt_callback_reason_t;


/**
 * These are the possible entry types in the options table.
 */
typedef enum ae_opt_type
{
     AE_OPT_TYPE_GROUP,         /**< Not a real argument, just a
                                 * visual separator. */
     AE_OPT_TYPE_HELP,          /**< The argument is to print the help
                                 * output. */
     AE_OPT_TYPE_VERSION,       /**< The argument is to print the
                                 * version information. */
     AE_OPT_TYPE_FLAG,          /**< The argument is a "presence"
                                 * flag */
     AE_OPT_TYPE_INT,           /**< The argument is an integer */
     AE_OPT_TYPE_DOUBLE,        /**< The argument is a double  */
     AE_OPT_TYPE_STRING,        /**< The argument is a string */
     AE_OPT_TYPE_BIT,           /**< The argument is stored in a bit flag */
     AE_OPT_TYPE_COUNTER,       /**< The argument controls a counter */
     
     AE_OPT_ENDMARKER,
} ae_opt_type_t;


/**
 * The possible parameter options
 */
typedef enum ae_opt_param
{
     AE_OPT_PARAM_NONE=0,       /**< You had better NOT provide an
                                 * argument. */
     AE_OPT_PARAM_OPTIONAL,     /**< Fine, do whatever you want.  See
                                 * if I care. */
     AE_OPT_PARAM_REQUIRED,     /**< You had better provide an
                                 * argument.  I'm not kidding
                                 * around. */

     AE_OPT_PARAM_ENDMARKER,
} ae_opt_required_t;


struct ae_opt;
struct ae_opt_option;

/** 
 * Callbacks for individual options must look like this.
 *
 * @param option the option table entry that was matched.
 *
 * @param arg if the option has an argument it is here.  If the
 * argument is optional this may be NULL.
 *
 * @return true to keep processing, false otherwise (and you should
 * add a message to @p e)
 */
typedef bool (*ae_opt_option_callback_t)(ae_res_t *e,
                                         struct ae_opt *self,
                                         const struct ae_opt_option *option,
                                         const char *arg);


/** 
 * Global callback functions must look like this.  They are called for
 * global level events.  See #ae_opt_option_callback_t for callbacks
 * when specific options are matched.
 *
 * @param ctx an arbitrary context pointer specified when the option
 * parser, @p self, was initialized. See #ae_opt_init.
 *
 * @param argument the name of the argument that caused the callback
 *
 * @param reason this specifies the reason for the callback.
 *
 * @return true to keep processing, false otherwise (and you should
 * add a message to @p e)
 */
typedef bool (*ae_opt_callback_t)(ae_res_t *e,
                                  struct ae_opt *self,
                                  void *ctx,
                                  const char *argument,
                                  ae_opt_callback_reason_t reason);


/**
 * This is a single entry into the option table.  This holds all of
 * the information about a single option.
 */
typedef struct ae_opt_option
{
     const char *name;          /**< command line argument name */
     size_t name_len;           /**< callers should ignore this field */
     const char *help;          /**< short help text */
     const char *help_param;    /**< very short help text for a parameter */
     ae_opt_type_t type;        /**< how to process this table entry */
     ae_opt_required_t is_required; /**< none, required, or optional parameter */

     void *in;                  /**< Some types take input arguments */
     void *out;                 /**< Some types take output arguments */

     ae_opt_option_callback_t callback; /**< Called when the option is
                                         * matched.  This usually
                                         * isn't required. */
     void *ctx;                 /**< Optional context to provide to
                                 * the callback */
} ae_opt_option_t;


/**
 * The option parser class
 */
typedef struct ae_opt
{
     const char *program_name;  /**< name of the program, used in help
                                 * and version output */
     const char *help;          /**< short help text for help output */
     const char *version;       /**< the program version used in
                                 * version output */

     ae_opt_callback_t callback; /**< global callback called for each
                                  * command line argument. */
     void *ctx;                 /**< supplied to `callback` */
     
     
     size_t options_len;        /**< the number of elements in `options` */
     const ae_opt_option_t *options; /**< option processing table */

     int optind;                /**< holds the number of the last
                                 * processed argument.  This is used
                                 * to process trailing parameters. */
} ae_opt_t;

#ifdef __cplusplus
extern "C" {
#endif

     /** 
      * Initialize an option processor.  There is no need to
      * uninitialized the processor.
      *
      * @param name the name of the program, used for help and version
      * output
      * 
      * @param version the version of the program, used for version output
      * 
      * @param help short text used for help output
      * 
      * @param cb a global callback function 
      * 
      * @param ctx optional context pointer that is passed to @p cb
      * 
      * @param options_len the number of elements in @p options
      * 
      * @param options the option processing table
      */
     bool ae_opt_init(ae_res_t *e, ae_opt_t *self,
                      const char *name,
                      const char *version,
                      const char *help,
                      ae_opt_callback_t cb, void *ctx,
                      size_t options_len, ae_opt_option_t *options);

     /** 
      * Process arguments
      *
      * @param argc number of elements in @p argv
      * 
      * @param argv command line arguments
      */
     bool ae_opt_process(ae_res_t *e, ae_opt_t *self, int argc, char **argv);


     /** 
      * Prints the program usage.  It is usually easier to use
      * AE_OPT_TYPE_HELP to have the option processor automatically
      * provide help.
      *
      * @param out where to print to
      */
     void ae_opt_help_print(const ae_opt_t *self, FILE *out);


     /** 
      * Prints the version.  It is usually easier to use
      * AE_OPT_TYPE_VERSION to have the option processor automatically
      * provide version output.
      *
      * @param out where to print to
      */
     void ae_opt_version_print(const ae_opt_t *self, FILE *out);

#ifdef __cplusplus
}
#endif


#endif
