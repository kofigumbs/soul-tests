#include "../SOUL/source/API/soul_patch/API/soul_patch.h"

using namespace soul::patch;

struct UserData {
    PatchPlayer::Ptr player;
    unsigned int channelCount;
};

// Implementations _MUST_ define this
void play(double sampleRate, unsigned int frameCount, UserData *userData);

// Implementations may call this between starting and stopping the driver
void wait() {
    std::cout << "Press Enter to exit...";
    std::getchar();
}

// Implementations _MUST_ call this from their callback function
void push(const void *input, void *output, unsigned int frameCount, void *data) {
    auto inputArray = (float(*)) input;
    auto outputArray = (float(*)) output;
    auto userData = (UserData (*)) data;

    const int maxMidi = 1024; // TODO what is a good number for this?
    MIDIMessage incomingMIDI[maxMidi], outgoingMIDI[maxMidi];
    PatchPlayer::RenderContext context;
    context.incomingMIDI = incomingMIDI;
    context.outgoingMIDI = outgoingMIDI;
    context.numMIDIMessagesIn = 0;
    context.numMIDIMessagesOut = 0;
    context.maximumMIDIMessagesOut = maxMidi;
    context.numFrames = frameCount;
    context.numInputChannels = userData->channelCount;
    context.numOutputChannels = userData->channelCount;

    // Deinterleave audio data  [ l0, r0, l1, r1, ... ]
    // into SOUL pointer frame  [ &l0, &l1, ..., &r0, &r1, ... ]
    const float* inputChannels[context.numInputChannels][context.numFrames];
    const float* outputChannels[context.numOutputChannels][context.numFrames];
    context.inputChannels = (const float* const*) inputChannels;
    context.outputChannels = (float* const*) outputChannels;
    for (int channel = 0; channel < context.numInputChannels; channel++)
        for (int frame = 0; frame < context.numFrames; frame++)
            inputChannels[channel][frame]  = inputArray  + channel + frame*context.numInputChannels;
    for (int channel = 0; channel < context.numOutputChannels; channel++)
        for (int frame = 0; frame < context.numFrames; frame++)
            outputChannels[channel][frame] = outputArray + channel + frame*context.numOutputChannels;

    userData->player->render(context);
}

// <https://github.com/soul-lang/SOUL/blob/master/source/API/soul_patch/helper_classes/soul_patch_AudioProcessor.h#L222-L230>
int countBuses(Span<Bus> buses) {
    int total = 0;
    for (auto& b : buses) total += static_cast<int> (b.numChannels);
    return total;
}

int main(int argc, char *argv[]) {
    bool mono;
    if      (argc == 2 && strcmp(argv[1], "mono")   == 0) mono = true;
    else if (argc == 2 && strcmp(argv[1], "stereo") == 0) mono = false;
    else { std::cout << "USAGE: a.out {mono,stereo}\n"; return 1; }

    SOULPatchLibrary library("build/SOUL_PatchLoader.dylib");
    PatchInstance::Ptr patch = library.createPatchFromFileBundle(mono ? "audio/mono.soulpatch" : "audio/stereo.soulpatch");
    PatchPlayerConfiguration playerConfig;
    playerConfig.sampleRate = 44100; // TODO
    playerConfig.maxFramesPerBlock = 64; // TODO

    UserData userData;
    userData.channelCount = mono ? 1 : 2;
    userData.player = patch->compileNewPlayer(playerConfig, NULL, NULL, NULL, NULL);

    assert(countBuses(userData.player->getInputBuses())  == userData.channelCount);
    assert(countBuses(userData.player->getOutputBuses()) == userData.channelCount);

    play(playerConfig.sampleRate, playerConfig.maxFramesPerBlock, &userData);
    return 0;
}
