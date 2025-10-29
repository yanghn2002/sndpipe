#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "utils.h"


///*** argument parse ***///

struct args_base_s args_base =\
    { 2, PCM_F_S16, 44100, 1024, -1. };

const char keys_base[] = "C:F:S:B:D:";

const struct args_help_s args_help_base[] = {
    { 'C', "Channele (>0)" },
    { 'F', "Format" },
    { 'S', "Sample Rate (each channel)" },
    { 'B', "Batch Size (each channel)" },
    { 'D', "Duration" },
    { '\0', "" },
};

void usage_exit(const char* argv0,\
const struct args_help_s* args_help_extra) {
    
    fprintf(stderr, "Usage: %s [args] ", argv0);
    size_t index;
    for(index = 0; args_help_base[index].arg; index++);
        fprintf(stderr, "%s", args_help_base[index].help);
    for(index = 0; args_help_extra[index].arg; index++);
        fprintf(stderr, "%s", args_help_extra[index].help);
    putc('\n', stderr);
    for(index = 0; args_help_base[index].arg; index++)
        fprintf(stderr, "    -%c: %s\n",
            args_help_base[index].arg, args_help_base[index].help);
    for(index = 0; args_help_extra[index].arg; index++)
        fprintf(stderr, "    -%c: %s\n",
            args_help_extra[index].arg, args_help_extra[index].help);
    
    exit(EXIT_FAILURE);

}

char* args_init(char* keys_extra) {

    size_t args_base_size = strlen(keys_base);
    size_t args_extra_size = strlen(keys_extra);
    size_t args_total_size = args_base_size + args_extra_size;
    char* opts = (char*)malloc(args_total_size + 1);
    if(!opts) return NULL;
    opts[args_total_size] = '\0';
    memcpy(opts, keys_base, args_base_size);
    memcpy(opts + args_base_size, keys_extra, args_extra_size);

    return opts;

}

int key;
pcm_format_t format;
ssize_t channels, sample_rate, batch_size;
double duration;
int args_each(char* arg_opts, int argc, char*argv[],\
const struct args_help_s* args_help) {

    while((key = getopt(argc, argv, "C:F:S:D:B:w:a:f:p:")) != -1) {
        switch(key) {
            case 'C':
                channels = str2ssize(optarg);
                if(channels == -1) usage_exit(argv[0], args_help);
                else if(channels == 0.) usage_exit(argv[0], args_help);
                else args_base.channels = channels;
                break;
            case 'F':
                if(!strcmp(optarg, "S16"))
                    args_base.format = PCM_F_S16;
                else usage_exit(argv[0], args_help);
                break;
            case 'S':
                sample_rate = str2ssize(optarg);
                if(sample_rate == -1) usage_exit(argv[0], args_help);
                else args_base.sample_rate = sample_rate;
                break;
            case 'B':
                batch_size = str2ssize(optarg);
                if(batch_size == -1) usage_exit(argv[0], args_help);
                else args_base.batch_size = batch_size;
                break;
            case 'D':
                duration = str2double(optarg);
                if(duration == -1) usage_exit(argv[0], args_help);
                else args_base.duration = duration;
                break;
            case '?': usage_exit(argv[0], args_help);
            default: return key;
        }
    }
    
    free(arg_opts);

    if(args_base.batch_size % args_base.channels != 0) {
        fputs("batch_size % channels not 0", stderr);
        exit(EXIT_FAILURE);
    }

    return -1;

}


///*** timer ***///

struct timespec time_delta[2];

int timer_init(void) {

    if(clock_gettime(CLOCK_MONOTONIC, &time_delta[0]) != 0)
        return 1;

}

int timer_check(void) {

    if(args_base.duration > 0) {
        if(clock_gettime(CLOCK_MONOTONIC, &time_delta[1])) {
            perror("clock_gettime");
            return -1;
        }
        double elapsed = (time_delta[1].tv_sec - time_delta[0].tv_sec) + 
                         (time_delta[1].tv_nsec - time_delta[0].tv_nsec) / 1e9;
        if(elapsed >= args_base.duration) return 1;
    }

    return 0;

}