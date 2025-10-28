#ifndef __SNDPIPE_ARGS
#define __SNDPIPE_ARGS


#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include "pcmio.h"


///*** argument parse ***///

struct args_help_s {
    char arg;
    char* help;
};

void usage_exit(const char*, const struct args_help_s*);

inline static int str2ssize(const char* str) {
    static char *endptr;
    ssize_t value = strtol(str, &endptr, 10);
    if(endptr == str || errno == ERANGE)
        return -1;
    return value;
}
inline static double str2double(const char* str) {
    static char *endptr;
    double value = strtod(str, &endptr);
    if(endptr == str || errno == ERANGE)
        return -1.;
    return value;
}

struct args_base_s {
    ssize_t channels;
    pcm_format_t format;
    ssize_t sample_rate;
    ssize_t batch_size;
    double duration;
};
extern struct args_base_s args_base;

char* args_init(char*);
int args_each(char*, int, char*[], const struct args_help_s*);


///*** timer ***///

extern struct timespec time_delta[2];

int timer_init(void);
int timer_check(void);



#endif//__SNDPIPE_ARGS