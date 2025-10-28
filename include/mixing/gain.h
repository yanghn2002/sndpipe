#ifndef __SNDPIPE_GAIN
#define __SNDPIPE_GAIN


#include "buffer.h"


void buffer_apply_gain(buffer_t*, buffer_t*); // just signal * gain
void buffer_apply_limite(buffer_t*, buffer_t*); // limite amplitude


#endif//__SNDPIPE_GAIN