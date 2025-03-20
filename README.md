libebur128 fork with single precision floats support for esp32(s3).
==========

libebur128 is a library that implements the EBU R 128 standard for loudness
normalisation.

Features
--------

* Portable ANSI C code.
* Implements Momentary, Shortterm and Integrated modes.
* Implements loudness range measurement (EBU - TECH 3342).
* True peak scanning.
* Supports all samplerates by recalculation of the filter coefficients.
* support for conversion of LUFS to discrete levels.


Requirements
--------
* enable external psram support. 
* may need to disable watchdog timers based on usage.

Installation
------------
```bash
idf.py add-dependency "danielmadmon/libebur128_esp32s3^0.0.1"
```
* or clone the repository into components directory in esp-idf project.

Basic Usage
-----
```C
#include "ebur128.h"

int16_t * data = malloc(48000*sizeof(int16_t)); //data. can be float/int32_t/int16_t
ebur128_state* state = ebur128_init(1,48000,EBUR128_MODE_M);
ebur128_add_frames_int16_t(state,data,48000);

ebur128_loudness_momentary(state,&result);
ESP_LOGI(TAG,"result:%f",result);
```

Use test file
-----
```C
float test_res = test_file("/sdcard/seq-3341-1-16bit.wav",-23.0,EBUR128_MODE_S);
ESP_LOGI(TAG,"res:%0.6f",test_res);
```
test files can be found in [test_signals.gz](test_files.tar.gz)

License
-----
All source code is licensed under the MIT license.
