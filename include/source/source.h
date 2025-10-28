#ifndef __SNDPIPE_SOURCE
#define __SNDPIPE_SOURCE


#include "lambda.h"
#include "buffer.h"


typedef struct {
    wave_lambda_t* wave_lambda;
    size_t channels;
    size_t sample_rate;
    double amplitude;
    double frequency;
    double _phi_curr;
} source_t;

source_t* source_init
(wave_lambda_t*, size_t, size_t, double, double, double);
void source_free(source_t*);

int source_to_buffer(source_t*, buffer_t*);


#endif//__SNDPIPE_SOURCE