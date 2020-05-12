||Mono|Stereo|
|---|---|---|
|miniaudio|✅|⛔️|
|RtAudio|✅|⛔️|
|PortAudio|⛔️|⛔️|

# Building Executables (macOS)

- ***⚠️ WARNING: Use headphones!***
- Download the relevant <https://github.com/soul-lang/SOUL/releases/latest> library into `./build`

#### w/ miniaudio

-
    ```
    g++ -std=c++17 main/miniaudio.cpp -o"./build/a.out"
    ./build/a.out {mono,stereo}
    ```

#### w/ RtAudio

- Build RtAudio submodule with instructions from <https://github.com/thestk/rtaudio/blob/master/install.txt>
- 
    ```
    g++ -std=c++17 main/rtaudio.cpp rtaudio/.libs/librtaudio.dylib -o ./build/a.out
    install_name_tool -change /usr/local/lib/librtaudio.6.dylib @executable_path/../rtaudio/.libs/librtaudio.dylib ./build/a.out
    ./build/a.out {mono,stereo}
    ```

#### w/ PortAudio

- Build portaudio with `./configure --enable-mac-universal=no && make`
-
    ```
    g++ -std=c++17 main/portaudio.cpp portaudio/lib/.libs/libportaudio.dylib -o ./build/a.out
    install_name_tool -change /usr/local/lib/libportaudio.2.dylib @executable_path/../portaudio/lib/.libs/libportaudio.dylib ./build/a.out
    ./build/a.out {mono,stereo}
    ```
# What is this

This is the smallest audio programming proof-of-concept I could come up with:

 - Listen to primary audio device playback with some DSP library
 - Send audio input to [SOUL](https://github.com/soul-lang/SOUL)
 - Listen to audio output from SOUL
 - Write to primary audio device with DSP library

This is my first time writing C++, so forgive any oddities and/or mistakes.
