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
#include "mixing/gain.h"


struct {
    double amplitude;
} args_extra;

const struct args_help_s args_help[] = {
    { 'a', "Amplitude" },
    { '\0', "<input-1-fifo> [input-x-fifo] < limite-fifo > mixed-fifo" },
};

int parsearg(int argc, char* argv[]) {

    char* args = args_init("a:");

    args_extra.amplitude = 1.;

    int key;
    double amplitude; 
    while((key = args_each(args, argc, argv, args_help)) != -1) {
        switch(key) {
            case 'a':
                amplitude = str2double(optarg);
                if(amplitude == -1.) goto ARGS_ERROR;
                else args_extra.amplitude = amplitude;
                break;
        }
    }

    return optind;
    
    ARGS_ERROR: usage_exit(argv[0], args_help);

}


int main(int argc, char* argv[]) {
    
    int curr_argc = parsearg(argc, argv);
    if(argc - curr_argc < 2) usage_exit(argv[0], args_help);
    
    pcmbuf_t* pcmbuf_result = pcmbuf_init_from_stdio(
        args_base.batch_size, args_base.channels, stdout, args_base.format);
    if(!pcmbuf_result) {
        perror("pcmbuf_init_from_stdio");
        return EXIT_FAILURE;
    }
    pcmbuf_t* pcmbuf_limite = pcmbuf_init_from_stdio(
        args_base.batch_size, args_base.channels, stdin, args_base.format);
    if(!pcmbuf_limite) {
        perror("pcmbuf_init_from_stdio");
        return EXIT_FAILURE;
    }
    size_t inpnum = argc - curr_argc;
    pcmbuf_t** pcmbuf_inputs = (pcmbuf_t**)malloc(inpnum * sizeof(pcmbuf_t*));
    if(!pcmbuf_inputs) {
        perror("malloc");
        return EXIT_FAILURE;
    }
    size_t index_i = 0;
    for(int index = curr_argc; index < argc; index++) {
        pcmbuf_inputs[index_i++] = pcmbuf_init_from_rfifo(
            args_base.batch_size, args_base.channels, argv[index], args_base.format);
        if(!pcmbuf_inputs) {
            perror("pcmbuf_init_from_rfifo");
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
        
        buffer_clean(pcmbuf_result->buffer);
        
        ret = pcmbuf_input(pcmbuf_limite);
        if(ret) {
            perror("pcmbuf_input");
            break;
        }
        for(size_t index_i = 0; index_i < inpnum; index_i++) {
            ret = pcmbuf_input(pcmbuf_inputs[index_i]);
            if(ret) {
                perror("pcmbuf_input");
                break;
            }
            FOR_BUFFER(index_b, pcmbuf_result->buffer) { // mix in double
                pcmbuf_result->buffer->data[index_b] +=\
                    pcmbuf_inputs[index_i]->buffer->data[index_b];
            }
        }
        FOR_BUFFER(index, pcmbuf_result->buffer)         
            pcmbuf_result->buffer->data[index] *= args_extra.amplitude;

        buffer_apply_limite(pcmbuf_result->buffer, pcmbuf_limite->buffer);
        
        ret = pcmbuf_output(pcmbuf_result);
        if(ret) {
            perror("pcmbuf_output");
            break;
        }

    }

    pcmbuf_free(pcmbuf_result);
    pcmbuf_free(pcmbuf_limite);
    for(size_t index = 0; index < inpnum; index++)
        pcmbuf_free(pcmbuf_inputs[index]);
    free(pcmbuf_inputs);

    return EXIT_SUCCESS;

}