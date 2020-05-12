#define MINIAUDIO_IMPLEMENTATION

#include <iostream>
#include "../miniaudio/miniaudio.h"
#include "../SOUL/source/API/soul_patch/API/soul_patch.h"

using namespace soul::patch;

// SOUL data we need for each frame
//
struct UserData {
    PatchPlayer::Ptr player;
};

// Pull from miniaudio frame, push to SOUL frame
//
void callback(ma_device* device, void* output, const void* input, ma_uint32 frameCount) {
    // Setup SOUL RenderContext
    const int maxMidi = 1024; // TODO what is a good number for this?
    MIDIMessage incomingMIDI[maxMidi], outgoingMIDI[maxMidi];
    PatchPlayer::RenderContext context;
    context.numInputChannels = device->capture.channels;
    context.numOutputChannels = device->playback.channels;
    context.incomingMIDI = incomingMIDI;
    context.outgoingMIDI = outgoingMIDI;
    context.numMIDIMessagesIn = 0;
    context.numMIDIMessagesOut = 0;
    context.maximumMIDIMessagesOut = maxMidi;

    // Convert from miniaudio frame  [ l0, r0, l1, r1, ... ]
    //                to SOUL frame  [[ l0, l1, ... ], [ r0, r1, ... ]]
    auto inputArray = *((float(*)[]) input);
    auto outputArray = *((float(*)[]) output);
    const float* inputChannels[device->capture.channels][frameCount];
    const float* outputChannels[device->playback.channels][frameCount];
    for (int i = 0; i < frameCount; i++)
        for (int j = 0; j < device->capture.channels; j++)
            inputChannels[j][i] = &inputArray[i*device->capture.channels + j];
    for (int i = 0; i < frameCount; i++)
        for (int j = 0; j < device->playback.channels; j++)
            outputChannels[j][i] = &outputArray[i*device->playback.channels + j];
    context.outputChannels = (float* const*) outputChannels;
    context.inputChannels = (const float* const*) inputChannels;
    context.numFrames = frameCount;

    //
    // SOME ATTEMPTS TO USE LIBRARY FUNCTIONS
    //
    // InterleavedChannelSet<const float> iInputSet = {};
    // iInputSet.numChannels = context.numInputChannels;
    // iInputSet.numFrames = frameCount;
    // iInputSet.stride = sizeof(float);
    // iInputSet.data = (const float(*)) input;
    // auto dInputSet = DiscreteChannelSet<float>::createAllocatedCopy(iInputSet);
    //
    // ma_deinterleave_pcm_frames(device->capture.internalFormat, context.numInputChannels, frameCount, (void*) inputArray, (void**) inputChannels);
    // ma_deinterleave_pcm_frames(device->playback.internalFormat, context.numOutputChannels, frameCount, (void*) outputArray, (void**) outputChannels);
    //

    // Render SOUL frame
    UserData* userData = (UserData (*)) device->pUserData;
    userData->player->render(context);
}

int main() {
    // Setup miniaudio
    UserData userData;
    ma_device device;
    ma_device_config audioConfig = ma_device_config_init(ma_device_type_duplex);
    audioConfig.dataCallback = callback;
    audioConfig.pUserData = &userData;
    ma_device_init(NULL, &audioConfig, &device);

    // Setup SOUL
    SOULPatchLibrary library("tmp/SOUL_PatchLoader.dylib");
    PatchInstance::Ptr patch = library.createPatchFromFileBundle("echo.soulpatch");
    PatchPlayerConfiguration playerConfig;
    playerConfig.sampleRate = device.sampleRate;
    playerConfig.maxFramesPerBlock = 64; // TODO what is a good number for this?
    userData.player = patch->compileNewPlayer(playerConfig, NULL, NULL, NULL, NULL);

    // Run device until any character is pressed
    ma_device_start(&device);
    std::cout << "Press Enter to exit...";
    std::getchar();
    ma_device_uninit(&device);
}
