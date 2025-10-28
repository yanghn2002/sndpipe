#ifndef __SNDPIPE_PCMIO
#define __SNDPIPE_PCMIO


#include <stdio.h>
#include "buffer.h"


typedef enum {
    PCM_F_S16 = -2
} pcm_format_t;

size_t pcm_format_size(pcm_format_t);

typedef struct {
    FILE* file;
    pcm_format_t format;
} pcmio_t;

pcmio_t* pcmio_init_from_stdio(FILE*, pcm_format_t);
pcmio_t* pcmio_init_from_rfifo(const char*, pcm_format_t);
pcmio_t* pcmio_init_from_wfifo(const char*, pcm_format_t);
pcmio_t* _pcmio_init_from_fifo(const char*, pcm_format_t, const char*);
void pcmio_free(pcmio_t*);


int buffer_input(buffer_t*, pcmio_t*);
int _buffer_input_s16(buffer_t*, pcmio_t*);
int buffer_output(buffer_t*, pcmio_t*);
int _buffer_output_s16(buffer_t*, pcmio_t*);


typedef struct {
    pcmio_t* pcmio;
    buffer_t* buffer;
} pcmbuf_t;

pcmbuf_t* pcmbuf_init_from_stdio(size_t, size_t, FILE*, pcm_format_t);
pcmbuf_t* pcmbuf_init_from_rfifo(size_t, size_t, const char*, pcm_format_t);
pcmbuf_t* pcmbuf_init_from_wfifo(size_t, size_t, const char*, pcm_format_t);
pcmbuf_t* _pcmbuf_init_from_fifo(size_t, size_t, const char*, pcm_format_t, 
    const char*);
void pcmbuf_free(pcmbuf_t*);
int pcmbuf_input(pcmbuf_t*);
int pcmbuf_output(pcmbuf_t*);


#endif//__SNDPIPE_PCMIO