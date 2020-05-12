#include <iostream>
#include "shared.h"
#include "../portaudio/include/portaudio.h"
#include "../SOUL/source/API/soul_patch/API/soul_patch.h"

using namespace soul::patch;

// Pull from PortAudio frame, push to SOUL frame
//
int callback(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *data) {
    PatchPlayer::RenderContext context = loadSoulContext();
    context.numFrames = frameCount;
    context.numInputChannels  = 2; // TODO derive from PortAudio
    context.numOutputChannels = 2; // TODO derive from PortAudio

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

int main(int argc, char* argv[]) {
    UserData userData;
    PaStream *stream;
    PaError error;
    unsigned int bufferFrames = 64;
    double sampleRate = 44100; // TODO use device default
    error = Pa_OpenDefaultStream(&stream, channelCount(argc, argv), channelCount(argc, argv), paFloat32, sampleRate, bufferFrames, callback, &userData);
    userData.channelCount = channelCount(argc, argv);
    userData.player = loadSoulPlayer(argc, argv, sampleRate, bufferFrames);

    // Run until keypress
    if (error != paNoError) { std::cout << Pa_GetErrorText(error); return 1; }
    error = Pa_StartStream(stream);
    if (error != paNoError) { std::cout << Pa_GetErrorText(error); return 2; }
    std::cout << "Press Enter to exit...";
    std::getchar();
    error = Pa_StopStream(stream);
    if (error != paNoError) { std::cout << Pa_GetErrorText(error); return 3; }
}
