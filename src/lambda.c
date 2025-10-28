#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include "buffer.h"
#include "lambda.h"


const double _4pi = 4. * M_PI;
const double _2pi = 2. * M_PI;
const double _pi  = 1. * M_PI;


int generate_wave
(wave_lambda_t* wave_lambda, buffer_t* buffer,
size_t channels, double amplitude, double phi_a, double phi_b) {
    
    double delta_phi = phi_b - phi_a;
    if(delta_phi < 0.) {
        errno = EINVAL;
        return 1;
    }

    double delta_phi_d = delta_phi / buffer->count;  // differential delta phi
    
    while(phi_a > _2pi) phi_a -= _2pi;
    FOR_BUFFER(index_b, buffer) {
        double phi = phi_a + delta_phi_d * index_b;
        while(phi > _2pi) phi -= _2pi;
        double value = amplitude * wave_lambda(phi);
        for(size_t index_c = 0; index_c < channels; index_c++)
            buffer->data[index_b+index_c] = value; // just repeat
    }

    return 0;

}


double wave_lambda_sine(double phi) {

    return sin(phi);

}

double wave_lambda_square(double phi) {

    if(phi < _pi) return 1.;
    else return -1.;

}

double wave_lambda_saw(double phi) {

    if(phi < _pi) return phi / _pi;
    else {
        phi -= _pi;
        return phi / _pi - 1.;
    }

}