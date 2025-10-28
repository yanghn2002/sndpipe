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
#include "source/source.h"


struct {
    double amplitude;
} args_extra;

const struct args_help_s args_help[] = {
    { 'a', "Amplitude" },
    { '\0', "> constand-fifo" },
};

int parsearg(int argc, char* argv[]) {

    char* args = args_init("a:");

    args_extra.amplitude = 1.;

    int key;
    double amplitude, frequency, phi; 
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


double wave_lambda_constand(double phi);

int main(int argc, char* argv[]) {

    int curr_argc = parsearg(argc, argv);
    if(argv[curr_argc]) {
        for(int index = curr_argc; index < argc; index++)
            fprintf(stderr, "Unknow argument: '%s'\n", argv[index]);
        usage_exit(argv[0], args_help);
    }

    pcmbuf_t* pcmbuf = pcmbuf_init_from_stdio(
        args_base.batch_size, args_base.channels, stdout, PCM_F_S16);
    if(!pcmbuf) {
        perror("pcmbuf_init_from_stdio");
        return EXIT_FAILURE;
    }
    source_t* source = source_init(wave_lambda_constand,
        args_base.channels, args_base.sample_rate, args_extra.amplitude, 0., 0.);
    if(!source) {
        perror("source_init");
        return EXIT_FAILURE;
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

        ret = source_to_buffer(source, pcmbuf->buffer);
        if(ret) {
            perror("source_to_buffer");
            break;
        }
        ret = pcmbuf_output(pcmbuf);
        if(ret) {
            perror("pcmbuf_output");
            break;
        }

    }


    source_free(source);
    pcmbuf_free(pcmbuf);

    return ret ? EXIT_FAILURE : EXIT_SUCCESS;

}

double wave_lambda_constand(double phi) {

    return 1.;

}