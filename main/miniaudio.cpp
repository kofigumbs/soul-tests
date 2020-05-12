#define MINIAUDIO_IMPLEMENTATION

#include <iostream>
#include "shared.h"
#include "../miniaudio/miniaudio.h"
#include "../SOUL/source/API/soul_patch/API/soul_patch.h"

using namespace soul::patch;

// Pull from miniaudio frame, push to SOUL frame
//
void callback(ma_device* device, void* output, const void* input, ma_uint32 frameCount) {
    const float* inputChannels[device->capture.channels][frameCount];
    const float* outputChannels[device->playback.channels][frameCount];
    PatchPlayer::RenderContext context = loadSoulContext();
    context.numFrames = frameCount;
    context.numInputChannels = device->capture.channels;
    context.numOutputChannels = device->playback.channels;
    context.outputChannels = (float* const*) outputChannels;
    context.inputChannels = (const float* const*) inputChannels;

    // Convert from miniaudio frame  [ l0, r0, l1, r1, ... ]
    //                to SOUL frame  [[ l0, l1, ... ], [ r0, r1, ... ]]
    auto inputArray = *((float(*)[]) input);
    auto outputArray = *((float(*)[]) output);
    for (int i = 0; i < frameCount; i++)
        for (int j = 0; j < device->capture.channels; j++)
            inputChannels[j][i] = &inputArray[i*device->capture.channels + j];
    for (int i = 0; i < frameCount; i++)
        for (int j = 0; j < device->playback.channels; j++)
            outputChannels[j][i] = &outputArray[i*device->playback.channels + j];

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

int main(int argc, char *argv[]) {
    UserData userData;
    ma_device device;
    ma_device_config config = ma_device_config_init(ma_device_type_duplex);
    config.capture.channels = channelCount(argc, argv);
    config.playback.channels = channelCount(argc, argv);
    config.periodSizeInFrames = 64;
    config.dataCallback = callback;
    config.pUserData = &userData;
    ma_device_init(NULL, &config, &device);
    userData.channelCount = channelCount(argc, argv);
    userData.player = loadSoulPlayer(argc, argv, device.sampleRate, config.periodSizeInFrames);

    ma_device_start(&device);
    std::cout << "Press Enter to exit...";
    std::getchar();
    ma_device_uninit(&device);
}
