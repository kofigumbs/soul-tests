#include <iostream>
#include "shared.h"
#include "../rtaudio/RtAudio.h"

int callback(void *output, void *input, unsigned int frameCount, double streamTime, RtAudioStreamStatus status, void *data) {
    push(input, output, frameCount, data);
    return 0;
}

void play(double sampleRate, unsigned int frameCount, UserData *userData) {
    RtAudio dac;
    RtAudio::StreamParameters iParams, oParams;
    iParams.nChannels = userData->channelCount;
    oParams.nChannels = userData->channelCount;
    iParams.deviceId = dac.getDefaultInputDevice();
    oParams.deviceId = dac.getDefaultOutputDevice();
    dac.openStream(&oParams, &iParams, RTAUDIO_FLOAT32, sampleRate, &frameCount, &callback, (void*) &userData);
    dac.startStream();
    wait();
    dac.stopStream();
}
