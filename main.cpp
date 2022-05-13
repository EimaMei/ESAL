#include "include/SSAL.hpp" // Include the library
#include <windows.h>
#define FOREGROUND_YELLOW FOREGROUND_RED | FOREGROUND_GREEN

std::string printText(std::string txt, int color=FOREGROUND_GREEN) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
    std::cout << txt;
    SetConsoleTextAttribute(hConsole, 7);

    return "";
}

int main(int argc, char** argv) {
    std::string arg="null";
    SSAL::audioArgs a;
    for (int i=1; i<argc; i++) {
        std::string s(argv[i]);
        if (s.find("mp3") != std::string::npos) arg=s;
        else if (s == "--help" || s == "-h") {arg="help"; break;}
        else if (s == "--speed" || s == "-s") { i++; a.speed=std::stof(argv[i]); }
        else if (s == "--volume" || s == "-v") { i++; a.volume.value=std::stoi(argv[i]); }
        else if (s == "--loop" || s == "-l") { i++; a.loops=std::stoi(argv[i]); }
        else if (s == "--start" || s == "-ss") { i++; a.start_at=std::stoi(argv[i]); }
        else if (s == "--end" || s == "-e") { i++; a.end_at=std::stoi(argv[i]); }
        else if (s == "--mute" || s == "-m") { 
            i++;
            s=argv[i];
            if (s == "left") { a.volume.left_side=false; a.volume.right_side=true;}
            if (s == "right") { a.volume.left_side=true; a.volume.right_side=false;}
        }
    }
    SSAL::audio audio(arg,a);

    if (arg == "help") {
        arg = "SSAL "+std::string(VERSION);
        std::cout << 
        printText(arg) << "\n" <<
        "Created by SaCode, idea by Allyedge\n" <<
        "SSAL (SaCode's Simple Audio Library), as the name implies, is a simple audio library for playing .mp3 and .wav files on Windows written in C++" <<
        "\n\n" << printText("USAGE: ", FOREGROUND_YELLOW) <<"\n    ssal [OPTIONS] <FILE>\n\n" << 
        printText("ARGS:", FOREGROUND_YELLOW) << "\n    " << printText("<FILE>") << "    The file you want to play\n\n" <<
        printText("OPTIONS:", FOREGROUND_YELLOW) << "\n    " << printText("-h") << ", " << printText("--help") << "               Print help information on how to use this tool\n    " <<
        printText("-s") << ", " << printText("--speed <SPEED>") <<  "      Set the speed of the audio\n    " <<
        printText("-v") << ", " << printText("--volume <VOLUME>") <<  "    Set the volume of the audio\n    " <<
        printText("-l") << ", " << printText("--loop <LOOP>") <<  "        How many times the audio should be played (-1 is infinite)\n    " <<
        printText("-ss") << ", " << printText("--start <TIME>") <<  "      What time the audio should start at (in seconds)\n    " <<
        printText("-e") << ", " << printText("--end <TIME>") <<  "         What time the audio should end at (in seconds)\n    " <<
        printText("-m") << ", " << printText("--mute <left/right>") <<  "  Mute a certain channel\n    ";
    }
    else if (arg == "null") {
        std::cout << printText("Error: ", FOREGROUND_RED) << "You must provide a file to play it!\n\n" << printText("USAGE: ", FOREGROUND_YELLOW) <<"\n    ssal [OPTIONS] <FILE>\n\nFor more information, use " << printText("--help");
    }
    else  {
        if (audio.track.title.size() == 0) audio.track.title=audio.file;
        std::cout << "Now playing: \"" << audio.track.title << "\"" << std::endl;
        audio.play();
        while (audio.isPlaying()) {
            audio.checkEvents();
        }
    }
    SSAL::quit(); // Quit SSAL when all is done

    return 0;
};