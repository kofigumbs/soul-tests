***⚠️ WARNING: Use headphones!***

# Quick Start (macOS)

- Download the relevant <https://github.com/soul-lang/SOUL/releases/latest> library into `./tmp`

## miniaudio

-
    ```
    g++ -std=c++17 src/miniaudio.cpp L"./tmp" -o"./tmp/echo"
    ./tmp/echo
    ```

To build on non-macOS platforms, update the `SOULPatchLibrary` constructor call in `main()`.

# What is this

This is the smallest audio programming proof-of-concept I could come up with:

 - Listen to primary audio device playback with some DSP library
 - Send audio input to [SOUL](https://github.com/soul-lang/SOUL)
 - Listen to audio output from SOUL
 - Write to primary audio device with miniaudio

This is my first time writing C++, so forgive any oddities and/or mistakes.
