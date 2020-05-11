#define MINIAUDIO_IMPLEMENTATION

#include <iostream>
#include "rtaudio/RtAudio.h"
#include "SOUL/source/API/soul_patch/API/soul_patch.h"

using namespace soul::patch;

// SOUL data we need for each frame
//
struct UserData {
    PatchPlayer::Ptr player;
    unsigned int bufferFrames = 0;
};

// Pull from RtAudio frame, push to SOUL frame
//
int callback(void *output, void *input, unsigned int frameCount, double streamTime, RtAudioStreamStatus status, void *data) {
    // Setup SOUL RenderContext
    const int maxMidi = 1024; // TODO what is a good number for this?
    MIDIMessage incomingMIDI[maxMidi], outgoingMIDI[maxMidi];
    PatchPlayer::RenderContext context;
    context.numFrames = frameCount;
    context.numInputChannels  = 2; // TODO derive from RtAudio
    context.numOutputChannels = 2; // TODO derive from RtAudio
    context.incomingMIDI = incomingMIDI;
    context.outgoingMIDI = outgoingMIDI;
    context.numMIDIMessagesIn = 0;
    context.numMIDIMessagesOut = 0;
    context.maximumMIDIMessagesOut = maxMidi;

    float* inputChannels[context.numFrames * context.numInputChannels];
    float* outputChannels[context.numFrames * context.numOutputChannels];
    for (int i = 0; i < context.numFrames * context.numInputChannels; i++)
        inputChannels[i] = ((float (*)) input) + i;
    for (int i = 0; i < context.numFrames * context.numOutputChannels; i++)
        outputChannels[i] = ((float (*)) output) + i;
    context.inputChannels = inputChannels;
    context.outputChannels = outputChannels;

    // Render SOUL frame
    UserData* userData = (UserData (*)) data;
    userData->player->render(context);
    return 0;
}

int main() {
    RtAudio dac;
    try {
        // Setup RtAudio
        UserData userData;
        double sampleRate = 44100;
        RtAudio::StreamOptions options;
        options.flags = RTAUDIO_NONINTERLEAVED;
        RtAudio::StreamParameters iParams, oParams;
        iParams.nChannels = 2;
        oParams.nChannels = 2;
        iParams.deviceId = dac.getDefaultInputDevice();
        oParams.deviceId = dac.getDefaultOutputDevice();
        dac.openStream(&oParams, &iParams, RTAUDIO_FLOAT32, sampleRate, &userData.bufferFrames, &callback, (void*) &userData, &options);

        // Setup SOUL
        SOULPatchLibrary library("lib/SOUL_PatchLoader.dylib");
        PatchInstance::Ptr patch = library.createPatchFromFileBundle("echo.soulpatch");
        PatchPlayerConfiguration playerConfig;
        playerConfig.sampleRate = sampleRate; // TODO derive from device default
        playerConfig.maxFramesPerBlock = userData.bufferFrames;
        userData.player = patch->compileNewPlayer(playerConfig, NULL, NULL, NULL, NULL);

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
