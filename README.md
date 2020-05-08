***WARNING: Wear headphones!***

**Quick Start**

```
g++ -std=c++17 -L"<PATH TO `SOUL_PatchLoader.dylib` DIRECTORY>" main.cpp
./a.out
```

# What is this

This is the smallest audio programming proof-of-concept I could come up with:

 - Listen to primary audio device playback with [miniaudio](github.com/dr-soft/miniaudio)
 - Send audio input to [SOUL](github.com/soul-lang/SOUL)
 - Listen to audio output from SOUL
 - Write to primary audio device with miniaudio

It kinda works, but it sounds distorted.
When I use `MA_COPY_MEMORY` to bypass SOUL, things sound OK, so it's probably related to how I manage the I/O buffers.
This is my first time writing C++, so forgive any oddities and/or mistakes.
