#include "ebur128.h"
#include "test.h"
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
#include <string.h>
#include "test.h"
#include <math.h>
#include <stdio.h>
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"


float test_global_loudness(const char* file_name){
    float out = -HUGE_VAL;
    drwav wav = {0};
    uint32_t res_file = drwav_init_file(&wav,file_name,NULL);
    if(!res_file){
        goto exit;
    }
    ebur128_state* st = ebur128_init(wav.channels,wav.sampleRate,EBUR128_MODE_I);
    if(!st){
        goto free_file;
    }


    float* buffer = (float*)malloc(wav.sampleRate * wav.channels * sizeof(float));
    if(!buffer){
        goto free_file;
    }
    int res = 0;
    uint64_t frames_read = 0;
    while(frames_read = drwav_read_pcm_frames_f32(&wav,wav.sampleRate,buffer),frames_read == wav.sampleRate){
        res = ebur128_add_frames_float(st,buffer,wav.sampleRate);
        if(EBUR128_SUCCESS != res){
            goto free_buffer;
        }
    }
    res = ebur128_add_frames_float(st,buffer,frames_read);
    if(res != EBUR128_SUCCESS){
        goto free_buffer;
    }
    ebur128_loudness_global(st,&out);

    free_buffer:
        free(buffer);
    free_file:
        if(st){
            ebur128_destroy(&st);
        }
        drwav_uninit(&wav);
    exit:
        return out;
}