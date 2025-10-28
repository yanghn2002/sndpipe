#include <stdlib.h>
#include <errno.h>
#include "buffer.h"


buffer_t* buffer_init(size_t batch_size, size_t channels) {

    if(!batch_size || !channels) {
        errno = EINVAL;
        return NULL;
    }

    size_t samples = batch_size * channels;

    buffer_t* buffer = (buffer_t*)malloc((sizeof(buffer_t)));
    buffer->data = (double*)malloc(samples * sizeof(double));
    if(!buffer) return NULL;
    if(!buffer->data) return NULL;

    buffer->count = samples;

    return buffer;

}

void buffer_free(buffer_t* buffer) {

    free(buffer->data);
    free(buffer);

}

void buffer_clean(buffer_t* buffer) {

    FOR_BUFFER(index, buffer)
        buffer->data[index] = 0.;

}

double buffer_amplitude(buffer_t* buffer) {
    
    double max = 0.;
    FOR_BUFFER(index, buffer)
        if(buffer->data[index] < 0.) {
            if(-buffer->data[index] > max)
                max = -buffer->data[index];
        }
        else if(buffer->data[index] > max)
            max = buffer->data[index];

    return max;

}