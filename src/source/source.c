#include <stdlib.h>
#include "source/source.h"


PI_ALIAS


source_t* source_init(
wave_lambda_t* wave_lambda,
size_t channels, size_t sample_rate, double amplitude, double frequency, double phi) {

    source_t* source  = (source_t*)malloc(sizeof(source_t));
    if(!source) return NULL;

    source->channels = channels;
    source->wave_lambda = wave_lambda;
    source->sample_rate = sample_rate;
    source->amplitude = amplitude;
    source->frequency = frequency;
    source->_phi_curr = phi;

    return source;

}

void source_free(source_t* source) {

    free(source);

}


int source_to_buffer(source_t* source, buffer_t* buffer) {

    while(source->_phi_curr > _4pi) source->_phi_curr -= _2pi;
    double phi_b = source->_phi_curr\
        + _2pi * source->frequency\
        * buffer->count / source->sample_rate\
        / source->channels;
    // here: phi_a + 2pi * f * (buffer_size / sample_rate) / channels

    int ret = generate_wave(
        source->wave_lambda, buffer, source->channels,
        source->amplitude, source->_phi_curr, phi_b);
    if(ret) return ret;

    source->_phi_curr = phi_b;

    return 0;

}