#include <iostream>
#include "shared.h"
#include "../rtaudio/RtAudio.h"
#include "../SOUL/source/API/soul_patch/API/soul_patch.h"

using namespace soul::patch;

// Pull from RtAudio frame, push to SOUL frame
//
int callback(void *output, void *input, unsigned int frameCount, double streamTime, RtAudioStreamStatus status, void *data) {
    // Setup SOUL RenderContext
    UserData* userData = (UserData (*)) data;
    PatchPlayer::RenderContext context = loadSoulContext();
    context.numFrames = frameCount;
    context.numInputChannels  = userData->channelCount;
    context.numOutputChannels = userData->channelCount;
    deinterleavePointers(context, input, output);

    // Render SOUL frame
    userData->player->render(context);
    return 0;
}

int main(int argc, char* argv[]) {
    RtAudio dac;
    try {
        UserData userData;
        double sampleRate = 44100; // TODO use device default
        unsigned int bufferFrames = 64;
        RtAudio::StreamParameters iParams, oParams;
        iParams.nChannels = channelCount(argc, argv);
        oParams.nChannels = channelCount(argc, argv);
        iParams.deviceId = dac.getDefaultInputDevice();
        oParams.deviceId = dac.getDefaultOutputDevice();
        dac.openStream(&oParams, &iParams, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &callback, (void*) &userData);
        userData.channelCount = channelCount(argc, argv);
        userData.player = loadSoulPlayer(argc, argv, sampleRate, bufferFrames);

        // Run until keypress
        dac.startStream();
        std::cout << "Press Enter to exit...";
        std::getchar();
        dac.stopStream();
    } catch (RtAudioError& e) {
        e.printMessage();
        if (dac.isStreamOpen()) dac.closeStream();
    }
}
