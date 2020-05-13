#include "../SOUL/source/API/soul_patch/API/soul_patch.h"

using namespace soul::patch;

struct UserData {
    PatchPlayer::Ptr player;
    unsigned int channelCount;
};

// Implementations define this
void play(double sampleRate, unsigned int frameCount, UserData *userData);

// Implementations call this between starting and stopping the driver
void wait() {
    std::cout << "Press Enter to exit...";
    std::getchar();
}

// Implementations call this from their callback function
void push(const void *input, void *output, unsigned int frameCount, void *data) {
    auto inputArray = *((float(*)[]) input);
    auto outputArray = *((float(*)[]) output);
    UserData* userData = (UserData (*)) data;

    const int maxMidi = 1024; // TODO what is a good number for this?
    MIDIMessage incomingMIDI[maxMidi], outgoingMIDI[maxMidi];
    PatchPlayer::RenderContext context;
    context.incomingMIDI = incomingMIDI;
    context.outgoingMIDI = outgoingMIDI;
    context.numMIDIMessagesIn = 0;
    context.numMIDIMessagesOut = 0;
    context.maximumMIDIMessagesOut = maxMidi;
    context.numInputChannels = userData->channelCount;
    context.numOutputChannels = userData->channelCount;

    const float* inputChannels[context.numInputChannels][context.numFrames];
    const float* outputChannels[context.numOutputChannels][context.numFrames];
    context.inputChannels = (const float* const*) inputChannels;
    context.outputChannels = (float* const*) outputChannels;
    for (int channel = 0; channel < context.numInputChannels; channel++)
        for (int frame = 0; frame < context.numFrames; frame++)
            inputChannels[channel][frame] = &inputArray[frame*context.numInputChannels + channel];
    for (int channel = 0; channel < context.numOutputChannels; channel++)
        for (int frame = 0; frame < context.numFrames; frame++)
            outputChannels[channel][frame] = &outputArray[frame*context.numOutputChannels + channel];

    userData->player->render(context);
}

int main(int argc, char *argv[]) {
    bool mono;
    if      (argc == 2 && strncmp(argv[1], "mono",   4)) mono = true;
    else if (argc == 2 && strncmp(argv[1], "stereo", 6)) mono = false;
    else { std::cout << "USAGE: a.out {mono,stereo}\n"; return 1; }

    UserData userData;
    userData.channelCount = mono ? 1: 2;
    SOULPatchLibrary library("build/SOUL_PatchLoader.dylib");
    PatchInstance::Ptr patch = library.createPatchFromFileBundle(mono ? "audio/mono.soulpatch" : "audio/stereo.soulpatch");
    PatchPlayerConfiguration playerConfig;
    playerConfig.sampleRate = 44100; // TODO
    playerConfig.maxFramesPerBlock = 64; // TODO
    userData.player = patch->compileNewPlayer(playerConfig, NULL, NULL, NULL, NULL);

    play(playerConfig.sampleRate, playerConfig.maxFramesPerBlock, &userData);
    return 0;
}
