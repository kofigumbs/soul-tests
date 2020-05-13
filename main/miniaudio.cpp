#define MINIAUDIO_IMPLEMENTATION

#include <iostream>
#include "shared.h"
#include "../miniaudio/miniaudio.h"

void callback(ma_device* device, void* output, const void* input, ma_uint32 frameCount) {
    push(input, output, frameCount, device->pUserData);
}

void play(double sampleRate, unsigned int frameCount, UserData *userData) {
    ma_device device;
    ma_device_config config = ma_device_config_init(ma_device_type_duplex);
    config.capture.channels = userData->channelCount;
    config.playback.channels = userData->channelCount;
    config.sampleRate = sampleRate;
    config.periodSizeInFrames = frameCount;
    config.dataCallback = callback;
    config.pUserData = userData;
    ma_device_init(NULL, &config, &device);
    ma_device_start(&device);
    wait();
    ma_device_uninit(&device);
}
