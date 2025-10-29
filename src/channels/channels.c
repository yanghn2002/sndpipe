#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "buffer.h"
#include "lambda.h"
#include "pcmio.h"
#include "utils.h"


#define split 1
#define combine 2
#ifdef CHANNELS_TASK
    #if CHANNELS_TASK == split
        #pragma message("CHANNELS_TASK == split")
    #elif CHANNELS_TASK == combine
        #pragma message("CHANNELS_TASK == combine")
    #endif
#else
    #error "CHANNELS_TASK undefined"
#endif


struct {
} args_extra;

const struct args_help_s args_help[] = {
#if CHANNELS_TASK == split
    { '\0', "<output-1-fifo> [output-x-fifo .. ] < input-fifo" },
#elif CHANNELS_TASK == combine
    { '\0', "<input-1-fifo> <input-x-fifo ...> > output-fifo" },
#endif
};

int parsearg(int argc, char* argv[]) {

    char* args = args_init("");

    int key;
    while((key = args_each(args, argc, argv, args_help)) != -1) {
    }

    return optind;
    
    ARGS_ERROR: usage_exit(argv[0], args_help);

}

#if CHANNELS_TASK == split
    #define PCMBUF_INIT_FROM_STDIO stdin
#endif
#if CHANNELS_TASK == combine
    #define PCMBUF_INIT_FROM_STDIO stdout
#endif
int main(int argc, char* argv[]) {
    
    int curr_argc = parsearg(argc, argv);
    int channels = argc - curr_argc;

#if CHANNELS_TASK == split
    if(channels < 1) usage_exit(argv[0], args_help);
#endif
#if CHANNELS_TASK == combine
    if(channels < 2) usage_exit(argv[0], args_help);
#endif
    pcmbuf_t* pcmbuf_stdio = pcmbuf_init_from_stdio(
        args_base.batch_size, args_base.channels, PCMBUF_INIT_FROM_STDIO, args_base.format);
    if(!pcmbuf_stdio) {
        perror("pcmbuf_init_from_fifo");
        return EXIT_FAILURE;
    }
    if(channels != args_base.channels) {
        fprintf(stderr, "Channels not match (%d/%ld).\n",
            channels, args_base.channels);
        return EXIT_FAILURE;
    }
    pcmbuf_t** pcmbuf_fifos = (pcmbuf_t**)malloc(args_base.channels * sizeof(pcmbuf_t*));
    if(!pcmbuf_fifos) {
        perror("malloc");
        return EXIT_FAILURE;
    }
    size_t index_i = 0;
    for(int index = curr_argc; index < argc; index++) {
#if CHANNELS_TASK == split
    #define PCMBUF_INIT_FROM_FIFO pcmbuf_init_from_wfifo
#endif
#if CHANNELS_TASK == combine
    #define PCMBUF_INIT_FROM_FIFO pcmbuf_init_from_rfifo
#endif
        pcmbuf_fifos[index_i++] = PCMBUF_INIT_FROM_FIFO(
            args_base.batch_size, 1, argv[index], args_base.format);
        if(!pcmbuf_fifos) {
#if CHANNELS_TASK == split
    perror("pcmbuf_init_from_wfifo");
#endif
#if CHANNELS_TASK == combine
    perror("pcmbuf_init_from_rfifo");
#endif
            return EXIT_FAILURE;
        }
    }


    if(timer_init()) {
        perror("timer_init");
        return EXIT_FAILURE;
    }
    
    int ret = 0;
    while(!ret) {

        ret = timer_check();
        if(ret < 0) {
            perror("timer_check");
            break;
        } else if(ret) {
            ret = 0;
            break;
        }

#if CHANNELS_TASK == split
        ret = pcmbuf_input(pcmbuf_stdio);
        if(ret) {
            perror("pcmbuf_input");
            break;
        }
#endif
#if CHANNELS_TASK == combine
        for(size_t index = 0; index < args_base.channels; index++) {
            ret = pcmbuf_input(pcmbuf_fifos[index]);
            if(ret) {
                perror("pcmbuf_input");
                break;
            }
        }
#endif

        for(size_t index_b = 0; index_b < args_base.batch_size; index_b++)
            for(size_t index_c = 0; index_c < args_base.channels; index_c++) {
#if CHANNELS_TASK == split
                pcmbuf_fifos[index_c]->buffer->data[index_b] =\
                    pcmbuf_stdio->buffer->data[index_c+index_b*args_base.channels];
#endif
#if CHANNELS_TASK == combine
                pcmbuf_stdio->buffer->data[index_c+index_b*args_base.channels] =\
                    pcmbuf_fifos[index_c]->buffer->data[index_b];
#endif
            }

#if CHANNELS_TASK == split
        for(size_t index = 0; index < args_base.channels; index++) {
            ret = pcmbuf_output(pcmbuf_fifos[index]);
            if(ret) {
                perror("pcmbuf_output");
                break;
            }
        }
#endif
#if CHANNELS_TASK == combine
        ret = pcmbuf_output(pcmbuf_stdio);
        if(ret) {
            perror("pcmbuf_output");
            break;
        }
#endif

    }

    pcmbuf_free(pcmbuf_stdio);
    for(size_t index = 0; index < args_base.channels; index++)
        pcmbuf_free(pcmbuf_fifos[index]);
    free(pcmbuf_fifos);

    return EXIT_SUCCESS;

}