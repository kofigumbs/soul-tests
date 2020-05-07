#ifndef SOUL_ECHO_H
#define SOUL_ECHO_H

#include <vector>
#include <functional>
#include <cmath>
#include <cstddef>
#include <limits>
#include <cstring>
#include <cassert>

//==============================================================================
// Generated from graph 'echo', source file: echo.soul
//
class echo
{
public:
    void init (double newSampleRate, int sessionID);
    void reset() noexcept;
    uint32_t getNumXRuns() noexcept;

    static constexpr uint32_t numAudioInputChannels  = 1;
    static constexpr uint32_t numAudioOutputChannels = 1;

    struct MIDIMessage
    {
        uint32_t frameIndex;
        uint8_t byte0, byte1, byte2;
    };

    struct MIDIMessageArray
    {
        const MIDIMessage* messages;
        uint32_t numMessages;
    };

    struct RenderContext
    {
        const float*      inputChannels[numAudioInputChannels];
        float*            outputChannels[numAudioOutputChannels];
        MIDIMessageArray  incomingMIDI;

        uint32_t numFrames;
    };

    void render (RenderContext context);
};

#endif // SOUL_ECHO_H
