#include "include/SSAL.hpp" // Include the library

int main(){
    SSAL::audio a("res/example.mp3"); // Init the music file
    a.play(); // Play the file

    while (a.isPlaying()) { // The loop while the song is being played
        a.checkEvents(); // Check for any new events
    } 
    SSAL::quit(); // Quit SSAL when all is done

    return 0;
};