#ifndef __SNDPIPE_BUFFER
#define __SNDPIPE_BUFFER


#include <stddef.h>


typedef struct {
    double* data;
    size_t count;
} buffer_t;

buffer_t* buffer_init(size_t, size_t);
void buffer_free(buffer_t*);
void buffer_clean(buffer_t*);
double buffer_amplitude(buffer_t*);

#define FOR_BUFFER(I, B) for(size_t I = 0; I < B->count; I++)  


#endif//__SNDPIPE_BUFFER