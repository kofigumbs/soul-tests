***⚠️ WARNING: Wear headphones!***

**Quick Start (macOS)**

1. Download the relevant <https://github.com/soul-lang/SOUL/releases/latest> library into `./lib/`
2. 
    ```
    TMPDIR=`mktemp -d` \
      && g++ -std=c++17 -L"lib" main.cpp lib/soul_core.o -o"$TMPDIR/a.out" \
      && $TMPDIR/a.out
    ```

To build on non-macOS platforms, update the `SOULPatchLibrary` constructor call in `main()`.

# What is this

This is the smallest audio programming proof-of-concept I could come up with:

 - Listen to primary audio device playback with [miniaudio](https://github.com/dr-soft/miniaudio)
 - Send audio input to [SOUL](https://github.com/soul-lang/SOUL)
 - Listen to audio output from SOUL
 - Write to primary audio device with miniaudio

This is my first time writing C++, so forgive any oddities and/or mistakes.
