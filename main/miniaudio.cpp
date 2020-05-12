#define MINIAUDIO_IMPLEMENTATION

#include <iostream>
#include "shared.h"
#include "../miniaudio/miniaudio.h"
#include "../SOUL/source/API/soul_patch/API/soul_patch.h"

using namespace soul::patch;

// Pull from miniaudio frame, push to SOUL frame
//
void callback(ma_device* device, void* output, const void* input, ma_uint32 frameCount) {
    // Grab required data from miniaudio
    PatchPlayer::RenderContext context = loadSoulContext();
    context.numFrames = frameCount;
    context.numInputChannels = device->capture.channels;
    context.numOutputChannels = device->playback.channels;

    // De-interleave miniaudio frame  [ l0, r0, l1, r1, ... ]
    //               into SOUL frame  [[ l0, l1, ... ], [ r0, r1, ... ]]
    auto inputArray = *((float(*)[]) input);
    auto outputArray = *((float(*)[]) output);
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
    userData.player = loadSoulPlayer(argc, argv, device.sampleRate, config.periodSizeInFrames);

    ma_device_start(&device);
    std::cout << "Press Enter to exit...";
    std::getchar();
    ma_device_uninit(&device);
}
