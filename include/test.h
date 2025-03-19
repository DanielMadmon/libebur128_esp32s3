#ifndef ___EBUR128_TEST__
#define ___EBUR128_TEST__
#include <stdio.h>
#include <stdbool.h>
#include "ebur128.h"
#ifdef __cplusplus
extern "C" {
#endif



float test_global_loudness(const char* filename);
float test_loudness_range(const char* filename);
float test_true_peak(const char* filename);
float test_max_momentary(const char* filename);
float test_max_shortterm(const char* filename);

typedef enum mode ebur128_mode;
/**
 * return the differnce between expected result and actual result
 */
float test_file(const char* filename, float expected_result,ebur128_mode mode);

#ifdef __cplusplus
    }
#endif

#endif