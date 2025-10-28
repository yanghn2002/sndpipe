#include "buffer.h"
#include "mixing/gain.h"


void buffer_apply_gain(buffer_t* buffer, buffer_t* gain) {

    FOR_BUFFER(index, buffer)
        buffer->data[index] *= buffer_amplitude(gain);

}

void buffer_apply_limite(buffer_t* buffer, buffer_t* gain) {

    double a_gain = buffer_amplitude(gain);
    double a_buffer = buffer_amplitude(buffer);
    double a_dot = a_gain / a_buffer;

    if(a_dot < 1.)
        FOR_BUFFER(index, buffer)
            if(buffer->data[index] > 1. || buffer->data[index] < 1.)
                buffer->data[index] *= a_dot;
    
}