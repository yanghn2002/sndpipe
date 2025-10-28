#ifndef __SNDPIPE_LAMBDA
#define __SNDPIPE_LAMBDA


#include <math.h>
#include "buffer.h"


#define PI_ALIAS\
    extern const double _4pi;\
    extern const double _2pi;\
    extern const double _pi;\


typedef double(wave_lambda_t)(double);

int generate_wave(wave_lambda_t*, buffer_t*, size_t, double, double, double);
double wave_lambda_sine(double);
double wave_lambda_square(double);
double wave_lambda_saw(double);


#endif//__SNDPIPE_LAMBDA