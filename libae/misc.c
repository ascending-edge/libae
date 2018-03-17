#include <ae/misc.h>

#include <stdio.h>
#include <stdlib.h>

const char * ae_misc_enum_to_string(int max, int state,
                                    const char **map,
                                    size_t map_len,
                                    const char *function,
                                    int line)
{
     if(state >= max)
     {
          return "invalid!";
     }
     if(map_len != max)
     {
          fprintf(stderr, "%s:%d update the string map!\n", function, line);
          abort();
     }
     return map[state];
}
