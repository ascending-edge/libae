/**
 * @author Greg Rowe <greg.rowe@ascending-edge.com>
 * 
 * Copyright (C) 2018 Ascending Edge, LLC - All Rights Reserved
 */
#ifndef _AE_MISC_H
#define _AE_MISC_H

#include <ae/types.h>

/**
 * This is a convenient macro for obtaining the length of an array
 */
#define AE_ARRAY_LEN(ar) sizeof((ar)) / sizeof((ar)[0])

#define AE_MEM_CLEAR(ptr) memset((ptr), 0, sizeof(*(ptr)));

#define AE_ENUM_TO_STRING(max, state, map) \
     ae_misc_enum_to_string((max), \
                            (state),                        \
                            (map),                          \
                            sizeof((map))/sizeof((map)[0]), \
                            __FILE__, __LINE__)

#ifdef __cplusplus
extern "C" {
#endif

     const char * ae_misc_enum_to_string(int max, int state,
                                         const char **map,
                                         size_t map_len,
                                         const char *file,
                                         int line);


#ifdef __cplusplus
}
#endif


#endif
