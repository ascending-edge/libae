/**
 * @author Greg Rowe <greg.rowe@ascending-edge.com>
 * 
 * Copyright (C) 2018 Ascending Edge, LLC - All Rights Reserved
 */
#ifndef _AE_MISC_H
#define _AE_MISC_H

/**
 * This is a convenient macro for obtaining the length of an array
 */
#define AE_ARRAY_LEN(ar) sizeof((ar)) / sizeof((ar)[0])

#define AE_MEM_CLEAR(ptr) memset((ptr), 0, sizeof(*(ptr)));

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif


#endif
