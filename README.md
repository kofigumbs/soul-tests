***⚠️ WARNING: Use headphones!***

# Quick Start (macOS)

- Download the relevant <https://github.com/soul-lang/SOUL/releases/latest> library into `./tmp`

#### w/ miniaudio

-
    ```
    g++ -std=c++17 src/miniaudio.cpp L"./tmp" -o"./tmp/echo"
    ./tmp/echo
    ```

#### w/ RtAudio

- Build RtAudio submodule with instructions from <https://github.com/thestk/rtaudio/blob/master/install.txt>
- 
    ```
    g++ -std=c++17 src/rtaudio.cpp rtaudio/.libs/librtaudio.dylib -o ./tmp/a.out \
      && install_name_tool -change /usr/local/lib/librtaudio.6.dylib @executable_path/rtaudio/.libs/librtaudio.dylib ./tmp/a.out \
      && ./tmp/a.out
    ```

# What is this

This is the smallest audio programming proof-of-concept I could come up with:

 - Listen to primary audio device playback with some DSP library
 - Send audio input to [SOUL](https://github.com/soul-lang/SOUL)
 - Listen to audio output from SOUL
 - Write to primary audio device with miniaudio

This is my first time writing C++, so forgive any oddities and/or mistakes.
