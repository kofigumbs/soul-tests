#include <iostream>
#include "SOUL/source/API/soul_patch/API/soul_patch.h"

using namespace soul::patch;

int main() {
    const int stereo = 2;
    const int frameCount = 512;
    const int midiCount = 1024;

    SOULPatchLibrary library("/usr/local/lib/SOUL_PatchLoader.dylib");
    PatchInstance::Ptr patch = library.createPatchFromFileBundle("echo.soulpatch");
    PatchPlayerConfiguration config;
    config.sampleRate = 441000;
    config.maxFramesPerBlock = frameCount;

    MIDIMessage incomingMIDI[midiCount], outgoingMIDI[midiCount];
    PatchPlayer::Ptr player = patch->compileNewPlayer(config, NULL, NULL, NULL, NULL);
    PatchPlayer::RenderContext context;
    context.numInputChannels = stereo;
    context.numOutputChannels = stereo;
    context.numFrames = frameCount;
    context.incomingMIDI = incomingMIDI;
    context.outgoingMIDI = outgoingMIDI;
    context.numMIDIMessagesIn = 0;
    context.numMIDIMessagesOut = 0;
    context.maximumMIDIMessagesOut = midiCount;

    float leftInput[frameCount];
    float* leftInputChannel[frameCount];
    for (int i = 0; i < frameCount; i++) leftInputChannel[i] = &leftInput[i];
    float rightInput[frameCount];
    float* rightInputChannel[frameCount];
    for (int i = 0; i < frameCount; i++) rightInputChannel[i] = &rightInput[i];
    const float* const inputChannels[] = { leftInput, rightInput };
    context.inputChannels = inputChannels;

    float leftOutput[frameCount];
    float* leftOutputChannel[frameCount];
    for (int i = 0; i < frameCount; i++) leftOutputChannel[i] = &leftOutput[i];
    float rightOutput[frameCount];
    float* rightOutputChannel[frameCount];
    for (int i = 0; i < frameCount; i++) rightOutputChannel[i] = &rightOutput[i];
    float* const outputChannels[] = { leftOutput, rightOutput };
    context.outputChannels = outputChannels;

    enum PatchPlayer::RenderResult result;
    while (true) {
      result = player->render(context);
      switch (result) {
        case PatchPlayer::RenderResult::ok: continue;
        case PatchPlayer::RenderResult::noProgramLoaded: return 1;
        case PatchPlayer::RenderResult::wrongNumberOfChannels: return 2;
      }
    }
}
