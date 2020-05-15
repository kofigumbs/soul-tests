#define MA_NO_JACK
#define MINIAUDIO_IMPLEMENTATION

#include <iostream>
#include "shared.h"
#include "../miniaudio/miniaudio.h"

void callback(ma_device* device, void* output, const void* input, ma_uint32 frameCount) {
    // TODO ma_deinterleave_pcm_frames and ma_interleave_pcm_frames should work here, but I couldn't figure it out
    float deinterleavedInput[device->capture.channels][frameCount],
          deinterleavedOutput[device->playback.channels][frameCount];

    for (int channel = 0; channel < device->capture.channels; channel++)
        for (int frame = 0; frame < frameCount; frame++)
            deinterleavedInput[channel][frame]  = ((float*) input)[channel + frame*device->capture.channels];

    push(deinterleavedInput, deinterleavedOutput, frameCount, device->pUserData);

    for (int channel = 0; channel < device->playback.channels; channel++)
        for (int frame = 0; frame < frameCount; frame++)
            ((float*) output)[channel + frame*device->playback.channels] = deinterleavedOutput[channel][frame];
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
