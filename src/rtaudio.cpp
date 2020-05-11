
#include <iostream>
#include "rtaudio/RtAudio.h"
#include "SOUL/source/API/soul_patch/API/soul_patch.h"

using namespace soul::patch;

// SOUL data we need for each frame
//
struct UserData {
    PatchPlayer::Ptr player;
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

    unsigned int framesIn  = context.numFrames * context.numInputChannels,
                 framesOut = context.numFrames * context.numOutputChannels;
    float *inputChannels[framesIn], *outputChannels[framesOut];
    for (int i = 0; i < framesIn;  i++) inputChannels[i]  = ((float (*)) input)  + i;
    for (int i = 0; i < framesOut; i++) outputChannels[i] = ((float (*)) output) + i;
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
        double sampleRate = 44100; // TODO use device default
        unsigned int bufferFrames = 64;
        RtAudio::StreamOptions options;
        options.flags = RTAUDIO_NONINTERLEAVED;
        RtAudio::StreamParameters iParams, oParams;
        iParams.nChannels = 2;
        oParams.nChannels = 2;
        iParams.deviceId = dac.getDefaultInputDevice();
        oParams.deviceId = dac.getDefaultOutputDevice();
        dac.openStream(&oParams, &iParams, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &callback, (void*) &userData, &options);

        // Setup SOUL
        SOULPatchLibrary library("lib/SOUL_PatchLoader.dylib");
        PatchInstance::Ptr patch = library.createPatchFromFileBundle("echo.soulpatch");
        PatchPlayerConfiguration playerConfig;
        playerConfig.sampleRate = sampleRate;
        playerConfig.maxFramesPerBlock = bufferFrames;
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
