***⚠️ WARNING: Wear headphones!***

**Quick Start (macOS)**

```
TMPDIR=`mktemp -d` \
  && g++ -std=c++17 -L"lib" main.cpp -o"$TMPDIR/a.out" \
  && $TMPDIR/a.out
```

To build on different platforms, reference the relevant library in the `-L` flag _and_ the `SOULPatchLibrary` constructor.

# What is this

This is the smallest audio programming proof-of-concept I could come up with:

 - Listen to primary audio device playback with [miniaudio](https://github.com/dr-soft/miniaudio)
 - Send audio input to [SOUL](https://github.com/soul-lang/SOUL)
 - Listen to audio output from SOUL
 - Write to primary audio device with miniaudio

This is my first time writing C++, so forgive any oddities and/or mistakes.
