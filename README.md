***⚠️ WARNING: Wear headphones!***

**Quick Start (macOS)**

1. Download the relevant <https://github.com/soul-lang/SOUL/releases/latest> library into `./lib/`
2. Build RtAudio submodule with instructions from <https://github.com/thestk/rtaudio/blob/master/install.txt>
2. 
    ```
    g++ -std=c++17 main.cpp rtaudio/.libs/librtaudio.dylib \
      && install_name_tool -change /usr/local/lib/librtaudio.6.dylib @executable_path/rtaudio/.libs/librtaudio.dylib a.out \
      && ./a.out
    ```

To build on non-macOS platforms, update the `g++` command _and_ the `SOULPatchLibrary` constructor call in `main()`.

# What is this

This is the smallest audio programming proof-of-concept I could come up with:

 - Listen to primary audio device playback with [miniaudio](https://github.com/dr-soft/miniaudio)
 - Send audio input to [SOUL](https://github.com/soul-lang/SOUL)
 - Listen to audio output from SOUL
 - Write to primary audio device with miniaudio

This is my first time writing C++, so forgive any oddities and/or mistakes.
