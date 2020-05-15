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

- 
    ```
    g++ -std=c++17 main/rtaudio.cpp rtaudio/RtAudio.cpp -D__MACOSX_CORE__ -framework CoreAudio -framework CoreFoundation -lpthread -o build/a.out
    ./build/a.out {mono,stereo}
    ```


# What is this

This is the smallest audio programming proof-of-concept I could come up with:

 - Listen to primary audio device playback with some DSP library
 - Send audio input to [SOUL](https://github.com/soul-lang/SOUL)
 - Listen to audio output from SOUL
 - Write to primary audio device with DSP library

I started at "I want to build a cool audio tool, but I don't want to spend my time in the weeds of C++ DSP programming".
These are the examples I wish I had when I was getting started.
This is my first time writing C++, so forgive any oddities and/or mistakes.

# Contributing

> It's a tough skill set, and it takes years and years.
> And it seems to be a skill set that mostly belongs to white male engineers with beards and poor dress sense...
> Surely we can do better than that right?
>
> — Jules Storer, ADC 2018 <https://www.youtube.com/watch?v=-GhleKNaPdk>

I published this repo to solicit ideas and help the next would-be audio programmer.
I was inspired by Jules' talk introducing SOUL, and I resonate with the demographic the team is aiming to include.
If you are also interested in helping get new beginners started, please feel free to submit contributions.
The more working examples, the better!
