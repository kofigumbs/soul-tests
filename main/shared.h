#include "../SOUL/source/API/soul_patch/API/soul_patch.h"

using namespace soul::patch;

struct UserData {
    PatchPlayer::Ptr player;
    int channelCount;
};

bool isStereo(int argc, char* argv[]) {
  return argc > 1 && strncmp(argv[1], "stereo", 6) == 0;
}

PatchPlayer::Ptr loadSoulPlayer(int argc, char *argv[], double sampleRate, unsigned int bufferFrames) {
    SOULPatchLibrary library("build/SOUL_PatchLoader.dylib");
    PatchInstance::Ptr patch = library.createPatchFromFileBundle(
        isStereo(argc, argv) ? "audio/stereo.soulpatch" : "audio/mono.soulpatch"
    );
    PatchPlayerConfiguration playerConfig;
    playerConfig.sampleRate = sampleRate;
    playerConfig.maxFramesPerBlock = bufferFrames;
    return patch->compileNewPlayer(playerConfig, NULL, NULL, NULL, NULL);
}

PatchPlayer::RenderContext loadSoulContext() {
    const int maxMidi = 1024; // TODO what is a good number for this?
    MIDIMessage incomingMIDI[maxMidi], outgoingMIDI[maxMidi];
    PatchPlayer::RenderContext context;
    context.incomingMIDI = incomingMIDI;
    context.outgoingMIDI = outgoingMIDI;
    context.numMIDIMessagesIn = 0;
    context.numMIDIMessagesOut = 0;
    context.maximumMIDIMessagesOut = maxMidi;
    return context;
}

int channelCount(int argc, char* argv[]) {
  return isStereo(argc, argv) ? 2 : 1;
}

// De-interleave    [ l0, r0, l1, r1, ... ]
// into SOUL frame  [[ l0, l1, ... ], [ r0, r1, ... ]]
#define deinterleavePointers(context, input, output) \
    auto inputArray = *((float(*)[]) input); \
    auto outputArray = *((float(*)[]) output); \
    const float* inputChannels[context.numInputChannels][context.numFrames]; \
    const float* outputChannels[context.numOutputChannels][context.numFrames]; \
    context.inputChannels = (const float* const*) inputChannels; \
    context.outputChannels = (float* const*) outputChannels; \
    for (int channel = 0; channel < context.numInputChannels; channel++) \
        for (int frame = 0; frame < context.numFrames; frame++) \
            inputChannels[channel][frame] = &inputArray[frame*context.numInputChannels + channel]; \
    for (int channel = 0; channel < context.numOutputChannels; channel++) \
        for (int frame = 0; frame < context.numFrames; frame++) \
            outputChannels[channel][frame] = &outputArray[frame*context.numOutputChannels + channel]; \
