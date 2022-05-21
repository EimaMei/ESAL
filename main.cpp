#include "include/SSAL.hpp"
#include <windows.h>
#include <fstream>
#include <iterator>
#define FOREGROUND_YELLOW   FOREGROUND_RED|FOREGROUND_GREEN

#define AUTHORS "Created by SaCode, idea by Allyedge"
#define DESC "SSAL (SaCode's Simple Audio Library), as the name implies, is a simple audio library for playing .mp3 and .wav files on Windows written in C++"
#define CMD_NAME "SSAL"

std::string printText(std::string txt, int color=FOREGROUND_GREEN) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
    std::cout << txt;
    SetConsoleTextAttribute(hConsole, 7);

    return "";
}

void clear() {
    COORD topLeft  = { 0, 0 };
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(console, &screen);
    FillConsoleOutputCharacterA(
        console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    FillConsoleOutputAttribute(
        console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
        screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    SetConsoleCursorPosition(console, topLeft);
}

std::vector<std::string> strsplit(std::string str, char letter=' ') {
    std::vector<std::string> list;
    std::string word;
    for (auto x : str)  {
        if (x == letter) {
            list.insert(list.end(), word);
            word = "";
        }
        else {
            word += x;
        }
    }
    list.insert(list.end(), word);

    return list;
}
std::string length;
std::string str;
int last=0;

void printAudio(SSAL::audio audio, SSAL::audioArgs a, std::string t, int blocks=20) {
    int g = (audio.current_pos.m*60+audio.current_pos.s)*1.0 / float((audio.track.duration.m*60+audio.track.duration.s)/(blocks*1.0));

    clear();
    std::cout << "Now playing " << printText(t);
    if (!audio.track.lead_artist.empty())  std::cout << " by " << printText(audio.track.lead_artist, FOREGROUND_YELLOW);
    std::cout << " (Speed: " << std::to_string(a.speed) << "; Volume: " << std::to_string(a.volume.value) << "; Loops: " << a.loops << ")\n|";

    if (str.empty()) for (int i=0; i<blocks; i++) str += "-";
    if (g != last) {length += "#"; str.pop_back(); last=g;}
    std::cout << length << str << "| (" << audio.current_pos.m << ":" << audio.current_pos.s << "/" << audio.track.duration.m << ":" << audio.track.duration.s << ")" << std::endl;
}

int main(int argc, char** argv) {
    std::string arg="null"; std::string filename;
    std::string secondary_path;
    SSAL::audioArgs a;
    for (int i=1; i<argc; i++) {
        std::string s(argv[i]);
        if (s.find("mp3") != std::string::npos) {filename=s; if (arg.size() == 0) arg="play";}
        else if (s == "--help" || s == "-h") {arg="help"; break;}
        else if (s == "--info" || s == "-i") {arg="info"; }
        else if (s == "--speed" || s == "-s") { i++; a.speed=std::stof(argv[i]); }
        else if (s == "--volume" || s == "-v") { i++; a.volume.value=std::stoi(argv[i]); }
        else if (s == "--loop" || s == "-l") { i++; a.loops=std::stoi(argv[i]); }
        else if (s == "--start" || s == "-ss") { i++; a.start_at=std::stoi(argv[i]); }
        else if (s == "--stop" || s == "-e") { i++; a.end_at=std::stoi(argv[i]); }
        else if (s == "--mute" || s == "-m") { 
            i++;
            s=argv[i];
            if (s == "left") { a.volume.left_side=false; a.volume.right_side=true;}
            if (s == "right") { a.volume.left_side=true; a.volume.right_side=false;}
        }
        else if (s == "--cover" || s == "-c") {
            i++;
            if (i >= argc) {
                std::cout << printText("Error: ", FOREGROUND_RED) << "Argument " << printText("<OUTPUT>") << " is missing!\n\n" << 
                printText("USAGE: ", FOREGROUND_YELLOW) <<"\n    " << CMD_NAME << " -c, --cover <OUTPUT> <FILE>\n" <<
                printText("EXAMPLE: ", FOREGROUND_YELLOW) << "\n    " << CMD_NAME " -c cover.png audio.mp3\n\nFor more information, use " << printText("--help") << std::endl;
                return 0;
            } else {
                s=argv[i];
                arg="cover"; 
                secondary_path=s;
            }
        }
    }
    SSAL::audio audio(filename,a);

    if (audio.track.title.size() == 0) audio.track.title=audio.file;
    std::string t = audio.track.title;

    if (t.find(".mp3") != std::string::npos) {
        t=t.substr(0, t.find(".mp3"));
        if (t.find("/") != std::string::npos) {
            std::vector<std::string> g = strsplit(t, '/');
            t=g[g.size()-1];
        }
    }

    if (arg == "help") {
        std::cout << 
        printText(VERSION) << "\n" <<
        AUTHORS << "\n" << 
        DESC <<
        "\n\n" << printText("USAGE: ", FOREGROUND_YELLOW) <<"\n    "+std::string(CMD_NAME)+" [OPTIONS] <FILE>\n\n" << 
        printText("ARGS:", FOREGROUND_YELLOW) << "\n    " << printText("<FILE>") << "    The file you want to play\n\n" <<
        printText("OPTIONS:", FOREGROUND_YELLOW) << "\n    " << 
        printText("-h") << ", " << printText("--help") <<  "               Print help information on how to use this tool\n    " <<
        printText("-s") << ", " << printText("--speed <SPEED>") <<  "      Set the speed of the audio\n    " <<
        printText("-v") << ", " << printText("--volume <VOLUME>") <<  "    Set the volume of the audio\n    " <<
        printText("-l") << ", " << printText("--loop <LOOP>") <<  "        How many times the audio should be played (-1 is infinite)\n    " <<
        printText("-ss") << ", " << printText("--start <TIME>") <<  "      What time the audio should start at (in seconds)\n    " <<
        printText("-e") << ", " << printText("--stop <TIME>") <<  "        What time the audio should end at (in seconds)\n    " <<
        printText("-m") << ", " << printText("--mute <left/right>") <<  "  Mute a certain channel\n    " <<
        printText("-c") << ", " << printText("--cover <OUTPUT>") <<  "   Extracts the album cover of the file and outputs it in that path.\n    " <<
        printText("-i") << ", " << printText("--info") <<  "               Output information about the audio file\n    ";
    }
    else if ( (audio.getError() == "Error 275: File not found ("+filename+")" && arg.size() == 0) || (filename.size() == 0 && arg != "help" )) {
        std::cout << printText("Error: ", FOREGROUND_RED) << "File '" << filename << "' doesn't exist!\n\n" << printText("USAGE: ", FOREGROUND_YELLOW) <<"\n    "+std::string(CMD_NAME)+" [OPTIONS] <FILE>\n\nFor more information, use " << printText("--help") << std::endl;
        arg.clear(); filename.clear();
    }
    else if (arg == "info") {
        std::cout << 
        printText("====== Information about the track ======\n", FOREGROUND_YELLOW) <<
        printText("Title: ") << audio.track.title << "\n" <<
        printText("Album: ") << audio.track.album << "\n" <<
        printText("Composers: "); for (int c=0; c<audio.track.composers.size(); c++) {std::cout << audio.track.composers[c]; if (c!=audio.track.composers.size()-1){std::cout << ", ";}} std::cout << "\n" <<
        printText("Album composer: ") << audio.track.album_artist << "\n" <<
        printText("Year: ") << audio.track.year << "\n" << 
        printText("Length: ") << audio.track.duration.m << ":" << audio.track.duration.s << "\n" <<
        printText("Genre: ") << audio.track.genre << "\n" <<
        printText("Track number: ") << audio.track.track_number << "\n" <<
        printText("ID3 version: ") << audio.track.id3_version << std::endl;
    }
    else if (arg == "cover") {
        if (audio.track.cover.size() == 0) std::cout << printText("Error:",FOREGROUND_RED) << " Song " << printText(t) << " doesn't have an album cover." << std::endl; 
        else  {
            std::fstream file(secondary_path, std::ios::trunc|std::ios::out|std::ios::binary);
            for (const auto &e : audio.track.cover) {file << e;}
            std::cout << "Sucessfully extracted the album cover for " << printText(t) << std::endl;
            file.close();
        }
    }
    else if (filename.size() > 0) {
        audio.play();
        SSAL::time old = audio.current_pos;

        while (!audio.done) {
            audio.checkEvents();
            if (old.s != audio.current_pos.s) {
                printAudio(audio, a, t);
                old=audio.current_pos;
            }
        }
    }
    else { std::cout << printText("Error: ", FOREGROUND_RED) << "You must provide a file to play it!\n\n" << printText("USAGE: ", FOREGROUND_YELLOW) <<"\n    "+std::string(CMD_NAME)+" [OPTIONS] <FILE>\n\nFor more information, use " << printText("--help") << std::endl; }
    SSAL::quit(); // Quit SSAL when all is done

    return 0;
};