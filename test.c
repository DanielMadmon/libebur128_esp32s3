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

float test_global_loudness(const char* filename){
    float out = -HUGE_VAL;
    drwav wav = {0};
    uint32_t res_file = drwav_init_file(&wav,filename,NULL);
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

float test_max_momentary(const char* filename){
    float out = -HUGE_VAL;
    drwav wav = {0};
    uint32_t res_file = drwav_init_file(&wav,filename,NULL);
    if(!res_file){
        goto exit;
    }
    ebur128_state* st = ebur128_init(wav.channels,wav.sampleRate,EBUR128_MODE_M);
    if(!st){
        goto free_file;
    }
    float* buffer = (float*)malloc((wav.sampleRate / 100) * wav.channels * sizeof(float));
    if(!buffer){
        goto free_file;
    }
    int res = 0;
    uint64_t frames_read = 0;
    uint64_t total_frames_read = 0;
    float momentary = 0.0f;
    while(frames_read = drwav_read_pcm_frames_f32(&wav,(wav.sampleRate / 100),buffer),
                                                    frames_read == (wav.sampleRate / 100)){
        total_frames_read += frames_read;
        res = ebur128_add_frames_float(st,buffer,frames_read);
        if(EBUR128_SUCCESS != res){
            goto free_buffer;
        }
        if(total_frames_read >= 4 * wav.sampleRate / 10){
            res = ebur128_loudness_momentary(st,&momentary);
            if(EBUR128_SUCCESS != res){
                goto free_buffer;
            }
            if(momentary > out){
                out = momentary;
            }
        }
    }

    res = ebur128_add_frames_float(st,buffer,frames_read);
    if(EBUR128_SUCCESS != res){
        goto free_buffer;
    }
    if(total_frames_read >= 4 * wav.sampleRate / 10){
        res = ebur128_loudness_momentary(st,&momentary);
        if(EBUR128_SUCCESS != res){
            goto free_buffer;
        }
        if(momentary > out){
            out = momentary;
        }
    }

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

float test_max_shortterm(const char* filename){
    float out = -HUGE_VAL;
    drwav wav = {0};
    uint32_t res_file = drwav_init_file(&wav,filename,NULL);
    if(!res_file){
        goto exit;
    }
    ebur128_state* st = ebur128_init(wav.channels,wav.sampleRate,EBUR128_MODE_S);
    if(!st){
        goto free_file;
    }
    float* buffer = (float*)malloc((wav.sampleRate / 10) * wav.channels * sizeof(float));
    if(!buffer){
        goto free_file;
    }
    int res = 0;
    uint64_t frames_read = 0;
    uint64_t total_frames_read = 0;
    float shortterm = 0.0f;
    while(frames_read = drwav_read_pcm_frames_f32(&wav,(wav.sampleRate / 10),buffer),
                                                    frames_read == (wav.sampleRate / 10)){
        total_frames_read += frames_read;
        res = ebur128_add_frames_float(st,buffer,frames_read);
        if(EBUR128_SUCCESS != res){
            goto free_buffer;
        }
        if(total_frames_read >= 4 * wav.sampleRate / 10){
            res = ebur128_loudness_shortterm(st,&shortterm);
            if(EBUR128_SUCCESS != res){
                goto free_buffer;
            }
            if(shortterm > out){
                out = shortterm;
            }
        }
    }
    res = ebur128_add_frames_float(st,buffer,frames_read);
    if(EBUR128_SUCCESS != res){
        goto free_buffer;
    }
    if(total_frames_read >= 4 * wav.sampleRate / 10){
        res = ebur128_loudness_shortterm(st,&shortterm);
        if(EBUR128_SUCCESS != res){
            goto free_buffer;
        }
        if(shortterm > out){
            out = shortterm;
        }
    }
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


float test_loudness_range(const char* filename){
    float out = -HUGE_VAL;
    drwav wav = {0};
    uint32_t res_file = drwav_init_file(&wav,filename,NULL);
    if(!res_file){
        goto exit;
    }
    ebur128_state* st = ebur128_init(wav.channels,wav.sampleRate,EBUR128_MODE_LRA);
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
    ebur128_loudness_range(st,&out);

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

float test_true_peak(const char* filename){
    float out = -HUGE_VAL;
    drwav wav = {0};
    uint32_t res_file = drwav_init_file(&wav,filename,NULL);
    if(!res_file){
        goto exit;
    }
    ebur128_state* st = ebur128_init(wav.channels,wav.sampleRate,EBUR128_MODE_TRUE_PEAK);
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
    float temp_peak = 0.0f;
    for(uint32_t channel = 0; channel < wav.channels; channel++){
        res = ebur128_true_peak(st,channel,&temp_peak);
        if(EBUR128_SUCCESS != res){
            goto free_buffer;
        }
        if(temp_peak > out){
            out = temp_peak;
        }
    }
    out = 20 * log10f(out);

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

float test_file(const char* filename, float expected_result,ebur128_mode mode){
    float result = -HUGE_VAL;
    switch(mode){
        case EBUR128_MODE_M:
            result = test_max_momentary(filename);
            break;
        case EBUR128_MODE_S:
            result = test_max_shortterm(filename);
            break;
        case EBUR128_MODE_I:
            result = test_global_loudness(filename);
            break;
        case EBUR128_MODE_LRA:
            result = test_loudness_range(filename);
            break;
        case EBUR128_MODE_SAMPLE_PEAK:
            result = test_true_peak(filename);
            break;
        case EBUR128_MODE_TRUE_PEAK:
            result = test_true_peak(filename);
            break;
        default:
            return result;
    }
    if(result == -HUGE_VAL){
        return result;
    }else if(fabsf(expected_result) > fabsf(result)){
        result = fabsf(expected_result) - fabsf(result);
    }else{
        result = fabsf(result) - fabsf(expected_result);
    }
    return result;
}