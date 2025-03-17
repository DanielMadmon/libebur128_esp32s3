#ifndef ___EBUR128_TEST__
#define ___EBUR128_TEST__
#include "sdkconfig.h"
#if CONFIG_ENABLE_TESTS
#include "wav.h"
#include <stdio.h>

float test_global_loudness(const char* file_name,ebur128_state** out_state);



#endif
#endif