# SSAL
SSAL (SaCode's Simple Audio Library), as the name implies, is a simple audio library for playing .mp3 and .wav files on Windows written in C++.
# How to build the library
1. Install MingW with the Win32 headers and binaries.
2. Build the library with entering `make build` in the cmd prompt.
# Simple example
```c++
#include "include/SSAL.hpp" // Include the library

int main(){
    SSAL::audio music("your file location.mp3"); // Init the music file
    music.play(); // Play the file

    while (music.isPlaying()) { // The loop while the song is being played
        music.checkEvents(); // Check for any new events
    } 
    SSAL::quit(); // Quit SSAL when all is done

    return 0;
};
```
# Credit
The idea for this library came from this [repo](https://github.com/Allyedge/allyaudio) by [Allyedge](https://github.com/Allyedge).