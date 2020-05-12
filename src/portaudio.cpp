
#include <iostream>
#include "../portaudio/include/portaudio.h"
#include "../SOUL/source/API/soul_patch/API/soul_patch.h"

using namespace soul::patch;

// SOUL data we need for each frame
//
struct UserData {
    PatchPlayer::Ptr player;
};

// Pull from PortAudio frame, push to SOUL frame
//
int callback(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *data) {
    // Setup SOUL RenderContext
    const int maxMidi = 1024; // TODO what is a good number for this?
    MIDIMessage incomingMIDI[maxMidi], outgoingMIDI[maxMidi];
    PatchPlayer::RenderContext context;
    context.numFrames = frameCount;
    context.numInputChannels  = 2; // TODO derive from PortAudio
    context.numOutputChannels = 2; // TODO derive from PortAudio
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
    // Setup PortAudio
    UserData userData;
    PaStream *stream;
    PaError error;
    unsigned int bufferFrames = 64;
    double sampleRate = 44100; // TODO use device default
    error = Pa_OpenDefaultStream(&stream, 2, 2, paFloat32, sampleRate, bufferFrames, callback, &userData);

    // Setup SOUL
    SOULPatchLibrary library("lib/SOUL_PatchLoader.dylib");
    PatchInstance::Ptr patch = library.createPatchFromFileBundle("echo.soulpatch");
    PatchPlayerConfiguration playerConfig;
    playerConfig.sampleRate = sampleRate;
    playerConfig.maxFramesPerBlock = bufferFrames;
    userData.player = patch->compileNewPlayer(playerConfig, NULL, NULL, NULL, NULL);

    // Run until keypress
    if (error != paNoError) return 1;
    error = Pa_StartStream(stream);
    if (error != paNoError) return 2;
    std::cout << "Press Enter to exit...";
    std::getchar();
    error = Pa_StopStream(stream);
    if (error != paNoError) return 3;
}
