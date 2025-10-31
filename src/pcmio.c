#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include "pcmio.h"


size_t pcm_format_size(pcm_format_t format) {

    return format < 0 ? -format : format;

}


pcmio_t* pcmio_init_from_stdio(FILE* stdio, pcm_format_t format) {

    if(!(stdio == stdin || stdio == stdout)) {
        errno = EINVAL;
        return NULL;
    }
    
    pcmio_t* pcmio = (pcmio_t*)malloc(sizeof(pcmio));
    if(!pcmio) return NULL;

    pcmio->file = stdio;
    pcmio->format = format;

    return pcmio;

}
pcmio_t* pcmio_init_from_rfifo(const char* fifo, pcm_format_t format) {

    return _pcmio_init_from_fifo(fifo, format, "rb");

}
pcmio_t* pcmio_init_from_wfifo(const char* fifo, pcm_format_t format) {

    return _pcmio_init_from_fifo(fifo, format, "wb");

}
pcmio_t* _pcmio_init_from_fifo(const char* fifo, pcm_format_t format, const char* flags) {

    pcmio_t* pcmio = (pcmio_t*)malloc(sizeof(pcmio));
    if(!pcmio) return NULL;

    pcmio->file = fopen(fifo, flags);
    if(!pcmio->file) return NULL;
    pcmio->format = format;

    return pcmio;

}

void pcmio_free(pcmio_t* pcmio) {

    fclose(pcmio->file);
    
    free(pcmio);

}


int buffer_input(buffer_t* buffer, pcmio_t* pcmio) {

    switch(pcmio->format) {
        case PCM_F_S16:
            return _buffer_input_s16(buffer, pcmio);
        default:
            errno = EINVAL;
            return -1;
    }

    return 0;

}

int _buffer_input_s16(buffer_t* buffer, pcmio_t* pcmio) {

    FOR_BUFFER(index, buffer) {
        static int16_t sample;
        int read = fread(&sample, -PCM_F_S16, 1, pcmio->file);
        if(read != 1) 
            if(read) return -1;
            else return 1;
        buffer->data[index] = ((double)sample) / INT16_MAX;
    }

    return 0;

}


int buffer_output(buffer_t* buffer, pcmio_t* pcmio) {

    switch(pcmio->format) {
        case PCM_F_S16:
            return _buffer_output_s16(buffer, pcmio);
        default:
            errno = EINVAL;
            return -1;
    }

    return 0;

}

int _buffer_output_s16(buffer_t* buffer, pcmio_t* pcmio) {
    
    FOR_BUFFER(index, buffer) {
        static uint16_t sample;
        if(buffer->data[index] > 1.) buffer->data[index] = 1.; // overdrive, cut
        else if(buffer->data[index] < -1.) buffer->data[index] = -1.;
        sample = (int16_t)(buffer->data[index] * INT16_MAX);
        int write = fwrite(&sample, -PCM_F_S16, 1, pcmio->file);
        if(write != 1)
            if(write) return -1;
            else return 1;
    }
    fflush(stdout);

    return 0;

}


pcmbuf_t* pcmbuf_init_from_stdio
(size_t batch_size, size_t channels, FILE* stdio, pcm_format_t format) {

    pcmbuf_t* pcmbuf = (pcmbuf_t*)malloc(sizeof(pcmbuf_t));
    if(!pcmbuf) return NULL;

    pcmbuf->pcmio = pcmio_init_from_stdio(stdio, format);
    if(!pcmbuf->pcmio) return NULL;
    pcmbuf->buffer = buffer_init(batch_size, channels);
    if(!pcmbuf->buffer) return NULL;
    return pcmbuf;

}

pcmbuf_t* pcmbuf_init_from_rfifo
(size_t batch_size, size_t channels, const char* fifo, pcm_format_t format) {

    return _pcmbuf_init_from_fifo(batch_size, channels, fifo, format, "rb");

}
pcmbuf_t* pcmbuf_init_from_wfifo
(size_t batch_size, size_t channels, const char* fifo, pcm_format_t format) {

    return _pcmbuf_init_from_fifo(batch_size, channels, fifo, format, "wb");

}
pcmbuf_t* _pcmbuf_init_from_fifo
(size_t batch_size, size_t channels, const char* fifo, pcm_format_t format, const char* flags) {

    pcmbuf_t* pcmbuf = (pcmbuf_t*)malloc(sizeof(pcmbuf_t));
    if(!pcmbuf) return NULL;
    pcmbuf->pcmio = _pcmio_init_from_fifo(fifo, format, flags);
    if(!pcmbuf->pcmio) return NULL;
    pcmbuf->buffer = buffer_init(batch_size, channels);
    if(!pcmbuf->buffer) return NULL;
    return pcmbuf;

}

void pcmbuf_free(pcmbuf_t* pcmbuf) {

    pcmio_free(pcmbuf->pcmio);
    buffer_free(pcmbuf->buffer);
    free(pcmbuf);

}

int pcmbuf_input(pcmbuf_t* pcmbuf) {

    return buffer_input(pcmbuf->buffer, pcmbuf->pcmio);

}

int pcmbuf_output(pcmbuf_t* pcmbuf) {

    return buffer_output(pcmbuf->buffer, pcmbuf->pcmio);

}