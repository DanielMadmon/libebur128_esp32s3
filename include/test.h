#ifndef ___EBUR128_TEST__
#define ___EBUR128_TEST__
#include <stdio.h>
#include "dr_wav.h"
/***/
float test_global_loudness(const char* filename);
float test_loudness_range(const char* filename);
float test_true_peak(const char* filename);
float test_max_momentary(const char* filename);
float test_max_shortterm(const char* filename);

#endif