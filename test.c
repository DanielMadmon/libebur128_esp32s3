#include "ebur128.h"
#include "test.h"
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"
#include <string.h>
#include "test.h"
#include <math.h>

static const char* TAG = "ebur128_test";
static const char test_dir[] = CONFIG_TEST_FILES_DIR;
static const size_t test_dir_len = sizeof(test_dir) - 1;

static int32_t conv_24bit_int32(uint8_t* buffer){
    return (buffer[2] << 16) | (buffer[1] << 8) | (buffer[0]); 
}

float test_global_loudness(const char* file_name,ebur128_state** out_state){
    size_t file_name_len = strlen(file_name);
    ebur128_state* st = NULL;
    float gated_loudness = 0.0f;
    FILE* wav_file = NULL;
    wavfile_header_t wav_info = {0};
    char * file_path = calloc(1,test_dir_len + file_name_len + 1);
    if(!file_path){
        ESP_LOGD(TAG,"failed to allocate file path buffer");
        goto __cln_exit;
    }
    memcpy(file_path,test_dir,test_dir_len);
    memcpy(&file_path[test_dir_len],file_name,file_name_len);
    ESP_LOGI(TAG,"%s",file_path);
    wav_file = wavfile_read_open(file_path,&wav_info.pcmSize);
    if(!wav_file){
        ESP_LOGD(TAG,"failed to open wav file:%s",file_path);
        goto __cln_exit;
    }
    int res_get_inf = wavfile_info(wav_file,&wav_info);
    if(-1 == res_get_inf){
        ESP_LOGD(TAG,"failed to get wav_info file:%s",file_name);
        goto __cln_exit;
    }
    st = ebur128_init((uint32_t)wav_info.channels,(uint64_t)wav_info.sampleRate,EBUR128_MODE_I);
    if(!st){
        ESP_LOGD(TAG,"failed to init ebur128");
        goto __cln_exit;
    }
    if(wav_info.channels == 5){
        ebur128_set_channel(st, 0, EBUR128_LEFT);
        ebur128_set_channel(st, 1, EBUR128_RIGHT);
        ebur128_set_channel(st, 2, EBUR128_CENTER);
        ebur128_set_channel(st, 3, EBUR128_LEFT_SURROUND);
        ebur128_set_channel(st, 4, EBUR128_RIGHT_SURROUND);
    }
    if(wav_info.bitsPerSample == 16){
        size_t buffer_size = wav_info.sampleRate * wav_info.channels * sizeof(int16_t);
        int16_t* buffer = (int16_t*)malloc(buffer_size);
        if(!buffer){
            ESP_LOGD(TAG,"failed to allocate buffer for:%s",file_name);
            goto __cln_exit;
        }
        const size_t bytes_size = wav_info.pcmSize;
        size_t bytes_read = 0;
        int32_t res = -1;
        while(bytes_read < bytes_size){
            res = wavfile_read(wav_file,buffer,wav_info.sampleRate);
            if(res <= 0){
                ESP_LOGD(TAG,"failed to read wav_file:%s",file_name);
                free(buffer);
                goto __cln_exit;
            }
            bytes_read += res;
            res = ebur128_add_frames_int16_t(st,buffer,res / 2);
            if(EBUR128_SUCCESS != res){
                ESP_LOGD(TAG,"failed to add frames. out of mem:%s",file_name);
                free(buffer);
                goto __cln_exit;
            }
        }


    }else if(wav_info.bitsPerSample == 24){
        size_t buffer_size = wav_info.sampleRate * wav_info.channels * sizeof(int32_t);
        uint8_t* buffer = (uint8_t*)malloc(buffer_size);
        if(!buffer){
            ESP_LOGD(TAG,"failed to allocate buffer for:%s",file_name);
            goto __cln_exit;
        }
        const size_t bytes_size = wav_info.pcmSize;
        size_t bytes_read = 0;
        int32_t res = -1;
        int32_t res_addf = 0;
        int32_t conv_24 = 0;
        while(bytes_read < bytes_size){
            res = wavfile_read(wav_file,buffer,wav_info.sampleRate);
            if(res <= 0){
                ESP_LOGD(TAG,"failed to read wav_file:%s",file_name);
                free(buffer);
                goto __cln_exit;
            }
            bytes_read += res;
            for(size_t i = 0; i < res; i += 3){
                conv_24 = conv_24bit_int32(&buffer[i]);
                res_addf = ebur128_add_frames_int32_t(st,&conv_24,1);
                if(EBUR128_SUCCESS != res_addf){
                ESP_LOGD(TAG,"failed to add frames. out of mem:%s",file_name);
                free(buffer);
                goto __cln_exit;
                }
            }
            
        }
        free(buffer);
    }else{
        ESP_LOGD(TAG,"unssuported bits per sample in wav file:%s",file_name);
        goto __cln_exit;
    }
    ebur128_loudness_global(st,&gated_loudness);
    *out_state = st;
    goto __cln_exit;

    __cln_exit:
        if(wav_file){
            wavfile_read_close(wav_file);
        }
        if(st){
            ebur128_destroy(&st);
        }
        if(file_path){
            free(file_path);
        }
        return gated_loudness;
}