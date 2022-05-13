#include "../include/SSAL.hpp"
#include <windows.h>
#include <fstream>
#include <vector>
int num=0;

std::vector<std::string> id3_v2_tags = { "AENC", "APIC", "COMM", "COMR", "ENCR", "EQUA", "ETCO", "GEOB", "GRID", "IPLS", "LINK", "MCDI", "MLLT", "OWNE", "PRIV", "PCNT", "POPM", "POSS", "RBUF", "RVAD", "RVRB", "SYLT", "SYTC", "TALB", "TBPM", "TCOM", "TCON", "TCOP", "TDAT", "TDLY", "TENC", "TEXT", "TFLT", "TIME", "TIT1", "TIT2", "TIT3", "TKEY", "TLAN", "TLEN", "TMED", "TOAL", "TOFN", "TOLY", "TOPE", "TORY", "TOWN", "TPE1", "TPE2", "TPE3", "TPE4", "TPOS", "TPUB", "TRCK", "TRDA", "TRSN", "TRSO", "TSIZ", "TSRC", "TSSE", "TYER", "TXXX", "UFID", "USER", "USLT", "WCOM", "WCOP", "WOAF", "WOAR", "WOAS", "WORS", "WPAY", "WPUB", "WXXX" };
std::vector<std::string> id3_v1_tags = { "BUF","CNT","COM","CRA","CRM","ETC","EQU","GEO","IPL","LNK","MCI","MLL","PIC","POP","REV","RVA","SLT","STC","TAL","TBP","TCM","TCO","TCR","TDA","TDY","TEN","TFT","TIM","TKE","TLA","TLE","TMT","TOA","TOF","TOL","TOR","TOT","TP1","TP2","TP3","TP4","TPA","TPB","TRC","TRD","TRK","TSI","TSS","TT1","TT2","TT3","TXT","TXX","TYE","UFI","ULT","WAF","WAR","WAS","WCM","WCP","WPB","WXX"};

int SSAL::audio::checkError(int error_code, bool ssal_error/*=false*/) {
    if(!ssal_error) {
        error="Error "+std::to_string(error_code);
        switch (error_code) {
            case 257: error += ": Invalid device ID (SSAL-"+std::to_string(num)+")"; break;
            case 263: error += ": Invalid device name (SSAL-"+std::to_string(num)+")"; break;
            case 275: error += ": File not found ("+file+")"; break;
            case 296: error += ": File invalid ("+file+")"; break;
            default:  break;
        }
    }
    return error_code;
}

std::vector<std::string> split(std::string str) {
    std::vector<std::string> list;
    std::string word;
    for (auto x : str)  {
        if (x == '/') {
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
  

int SSAL::audio::load(std::string file, audioArgs a/*={}*/) {
    char filename[1024];
    std::string cmd = "IF EXIST "+file+" echo yes";
    FILE* f = popen(cmd.c_str(), "r");
    fgets(filename, 1024, f);
    cmd=(std::string)filename;

    if (cmd!="yes\n") {
        error="Error 275: File not found ("+file+")";
        valid=false;
        
        return -1; 
    }
    else if (file.find("mp3") != std::string::npos || file.find("wav") != std::string::npos) {
        char path[MAX_PATH];
        GetShortPathNameA(file.c_str(),path,sizeof(path));
        SSAL::audio::file=file; arg=a;
    
        if (a.loops==-1) inf_loops=true;
    
        char szCmd[MAX_PATH + 64];
        sprintf( szCmd, "Open \"%s\" Type MPEGVideo alias SSAL-%d", path, num);
        id="SSAL-"+std::to_string(num);
        num++;
        checkError(mciSendStringA( szCmd, NULL, 0, NULL ));
        SSAL::track m(file);
        track=m;
        valid=true;
    }
    else { 
        error="Error: You must provide an ID3 audio file (.mp3/.wav)"; 
        valid=false;
        return -1; 
    }
    
    return 0;
}

void SSAL::audio::play() {
    std::string szCmd="play "+id+" notify";//+" Wait";
    if (arg.start_at>0) {szCmd+=" from "+std::to_string(arg.start_at*1000); }
    if (arg.end_at>0) { szCmd+=" to "+std::to_string(arg.end_at*1000); }

    if (arg.volume.value > -1) {
        std::string c="setaudio "+id+"  volume to "+std::to_string(arg.volume.value);
        checkError(mciSendStringA(c.c_str(), NULL, 0, 0 ));
    }
        
    if (arg.volume.left_side || arg.volume.right_side) {
        if (arg.volume.left_side) {
            std::string c="setaudio "+id+" right volume to 0";
            checkError(mciSendStringA(c.c_str(), NULL, 0, 0 ));
            c = "setaudio "+id+" left volume to "+std::to_string(arg.volume.value);
            checkError(mciSendStringA(c.c_str(), NULL, 0, 0 ));
        }
        else {
            std::string c="setaudio "+id+" left volume to 0";
            checkError(mciSendStringA(c.c_str(), NULL, 0, 0 ));
            c = "setaudio "+id+" right volume to "+std::to_string(arg.volume.value);
            checkError(mciSendStringA(c.c_str(), NULL, 0, 0 ));
        }
        
    } 
        
    if (arg.speed != -1) {
        std::string c = "set "+id+" speed "+std::to_string(int(arg.speed*1000) );
        checkError(mciSendStringA(c.c_str(), NULL, 0, 0 ));
    }

    checkError(mciSendStringA(szCmd.c_str(), NULL, 0, 0 ));
}

void SSAL::audio::pause() {
    std::string szCmd = "pause "+id;
    checkError(mciSendStringA(szCmd.c_str(), NULL, 0, 0 ));
    done=true;
}

void SSAL::audio::resume() {
    std::string szCmd = "resume "+id;
    checkError(mciSendStringA(szCmd.c_str(), NULL, 0, 0 ));
    done=false;
}

void SSAL::audio::stop() {
    std::string szCmd = "stop "+id;
    checkError(mciSendStringA(szCmd.c_str(), NULL, 0, 0 ));
    done=true;
}

bool SSAL::audio::isPlaying() {
    char szCmd[MAX_PATH + 64];
    sprintf( szCmd, "status %s mode", id.c_str());
    checkError(mciSendStringA(szCmd, szCmd, MAX_PATH + 64, 0 ));
    std::string res = szCmd;

    done=false;
    if (res == "playing") {done=true;}
    return done;
}

void SSAL::quit() { 
    mciSendString(TEXT("Close All"), NULL, 0, 0 ); 
}

SSAL::audio::audio(std::string file, audioArgs a/*={}*/) { 
    load(file, a); 
}

std::string SSAL::audio::getError() { 
    return error; 
}


int SSAL::audio::checkEvents() {
    if (!isPlaying() && (arg.loops > 0 || inf_loops) ) {
        if (arg.loops != -1) arg.loops--;
        arg.start_at=0;
        arg.end_at=0;

        load(file,arg); 
        play(); 
    }

    return 0;
}


void SSAL::track::load(std::string filename) {
    if (filename != "") {
        std::ifstream file(filename, std::ios::binary);

        file.seekg(0, std::ios::end);
        std::streampos fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
    
        std::vector<unsigned char> fileData(fileSize);
        file.read((char*) &fileData[0], fileSize);
        data=fileData; id3_version=data[0x03];
        getAll();
    }
}

SSAL::track::track(std::string filename, bool load/*=true*/) { if (load) track::load(filename); }

void SSAL::track::getAll() {
    char str[5];
    for (int i=0x0a; i<data.size(); i++) { 
        sprintf(str, "%c%c%c%c", data[i], data[i+1], data[i+2], data[i+3]); std::string res = str;
        if (id3_version > 2) {

            if (res == "TIT2") { i+=6; title = findID3Data(i); }
            if (res == "TALB") { i+=6; album = findID3Data(i); }
            if (res == "TCOM") { i+=6; composers = split(findID3Data(i)); }
            if (res == "TPE2") { i+=6; album_artist = findID3Data(i); }

            if (res == "TYER") { i+=6; year = std::stoi(findID3Data(i)); }
        }
        else {
            if (res == "TT2") { i+=3; title = findID3Data(i); }
            if (res == "TAL") { i+=3; album = findID3Data(i); }
            if (res == "TCM") { i+=3; composers = split(findID3Data(i)); }
            if (res == "TP1") { i+=3; album_artist = findID3Data(i); }

            if (res == "TYE") { i+=3; year = std::stoi(findID3Data(i)); }
        }
        if (title.size() > 0 && album.size() > 0 && composers.size() > 0 && album_artist.size() > 0 && year > 0) break;
    }
}

std::string SSAL::track::findID3Data(int& i, bool genre/*=false*/) {
    std::string result;
    bool done=false;
    int non_ascii_count=0;
    bool ascii=false;

    for (int x=4; x<100; x++) {
        int pos = i+x;
        if (data[pos]>=32 && data[pos]<=126) {
            ascii=true;
            char str[5];
            unsigned char tag[4] = { data[pos], data[pos+1], data[pos+2], data[pos+3] };

            if (id3_version > 2) {
                sprintf(str, "%c%c%c%c", tag[0], tag[1], tag[2], tag[3]);
                std::string res=str;

                for (int tags=0; tags<id3_v2_tags.size(); tags++) { if (res == id3_v2_tags[tags]) {done=true; break;} }
            } else {
                sprintf(str, "%c%c%c", tag[0], tag[1], tag[2]);
                std::string res=str;

                for (int tags=0; tags<id3_v1_tags.size(); tags++) { if (res == id3_v1_tags[tags]) {done=true; break;}; }
            }
            if (done) { break;}
        
            result += data[pos];
        }
        else if (ascii) non_ascii_count++;
    }
    return result;
}