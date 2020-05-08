#define MINIAUDIO_IMPLEMENTATION

#include <iostream>
#include "miniaudio/miniaudio.h"
#include "SOUL/source/API/soul_patch/API/soul_patch.h"

using namespace soul::patch;

// SOUL data we need for each "tick"
struct UserData {
  PatchPlayer::Ptr player;
  PatchPlayer::RenderContext context;
};

// miniaudio callback that runs on each frame
//
//   When we get this, we need to "tick" SOUL with `render()`.
//
void ma_ondata(ma_device* device, void* output, const void* input, ma_uint32 frameCount) {
    UserData* userData = (UserData (*)) device->pUserData;
    auto inputArray = (float(*)[]) input;
    auto outputArray = (float(*)[]) output;
    const float* inputChannels[2][frameCount];
    const float* outputChannels[2][frameCount];

    // SOUL requires a nested array, but miniaudio uses a flat array of pairs
    for (int i = 0; i < frameCount; i++) {
      for (int j = 0; j < device->playback.channels; j++) {
        inputChannels[j][i] = &(*inputArray)[i*2 + j];
        outputChannels[j][i] = &(*outputArray)[i*2 + j];
      }
    }

    userData->context.outputChannels = (float* const*) outputChannels;
    userData->context.inputChannels = (const float* const*) inputChannels;
    userData->context.numFrames = frameCount;
    userData->player->render(userData->context);
}

int main() {
    // Setup miniaudio
    UserData userData;
    ma_device device;
    ma_device_config audioConfig = ma_device_config_init(ma_device_type_duplex);
    audioConfig.dataCallback = ma_ondata;
    audioConfig.pUserData = &userData;
    audioConfig.playback.format = ma_format_f32;
    ma_device_init(NULL, &audioConfig, &device);

    // TODO what are good numbers for these?
    const int maxMidi = 1024;
    const int maxFrames = 1024;

    // Setup SOUL
    SOULPatchLibrary library("/usr/local/lib/SOUL_PatchLoader.dylib");
    PatchInstance::Ptr patch = library.createPatchFromFileBundle("echo.soulpatch");
    PatchPlayerConfiguration playerConfig;
    playerConfig.sampleRate = device.sampleRate;
    playerConfig.maxFramesPerBlock = maxFrames;

    MIDIMessage incomingMIDI[maxMidi], outgoingMIDI[maxMidi];
    PatchPlayer::Ptr player = patch->compileNewPlayer(playerConfig, NULL, NULL, NULL, NULL);
    PatchPlayer::RenderContext context;
    context.numInputChannels = device.playback.channels;
    context.numOutputChannels = device.playback.channels;
    context.incomingMIDI = incomingMIDI;
    context.outgoingMIDI = outgoingMIDI;
    context.numMIDIMessagesIn = 0;
    context.numMIDIMessagesOut = 0;
    context.maximumMIDIMessagesOut = maxMidi;

    userData.player = player;
    userData.context = context;

    // Run device until any character is pressed
    ma_device_start(&device);
    std::cout << "Press any key to quit...";
    std::getchar();
    ma_device_uninit(&device);
}
