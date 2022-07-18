#include "../include/SSAL.hpp"
#include <windows.h>
#include <fstream>
#include <vector>
#include <algorithm>
int num=0;

std::vector<std::string> const id3_v2_tags = { "AENC", "APIC", "COMM", "COMR", "ENCR", "EQUA", "ETCO", "GEOB", "GRID", "IPLS", "LINK", "MCDI", "MLLT", "OWNE", "PRIV", "PCNT", "POPM", "POSS", "RBUF", "RVAD", "RVRB", "SYLT", "SYTC", "TALB", "TBPM", "TCOM", "TCON", "TCOP", "TDAT", "TDLY", "TENC", "TEXT", "TFLT", "TIME", "TIT1", "TIT2", "TIT3", "TKEY", "TLAN", "TLEN", "TMED", "TOAL", "TOFN", "TOLY", "TOPE", "TORY", "TOWN", "TPE1", "TPE2", "TPE3", "TPE4", "TPOS", "TPUB", "TRCK", "TRDA", "TRSN", "TRSO", "TSIZ", "TSRC", "TSSE", "TYER", "TXXX", "UFID", "USER", "USLT", "WCOM", "WCOP", "WOAF", "WOAR", "WOAS", "WORS", "WPAY", "WPUB", "WXXX" };
std::vector<std::string> const id3_v1_tags = { "BUF","CNT","COM","CRA","CRM","ETC","EQU","GEO","IPL","LNK","MCI","MLL","PIC","POP","REV","RVA","SLT","STC","TAL","TBP","TCM","TCO","TCR","TDA","TDY","TEN","TFT","TIM","TKE","TLA","TLE","TMT","TOA","TOF","TOL","TOR","TOT","TP1","TP2","TP3","TP4","TPA","TPB","TRC","TRD","TRK","TSI","TSS","TT1","TT2","TT3","TXT","TXX","TYE","UFI","ULT","WAF","WAR","WAS","WCM","WCP","WPB","WXX"};
const char* const id3v2_genre[192] = { "Blues","Classic Rock","Country","Dance","Disco","Funk","Grunge","Hip-Hop","Jazz","Metal","New Age","Oldies","Other","Pop","R&B","Rap","Reggae","Rock","Techno","Industrial","Alternative","Ska","Death Metal","Pranks","Soundtrack","Euro-Techno","Ambient","Trip-Hop","Vocal","Jazz+Funk","Fusion","Trance","Classical","Instrumental","Acid","House","Game","Sound Clip","Gospel","Noise","AlternRock","Bass","Soul","Punk","Space","Meditative","Instrumental Pop","Instrumental Rock","Ethnic","Gothic","Darkwave","Techno-Industrial","Electronic","Pop-Folk","Eurodance","Dream","Southern Rock","Comedy","Cult","Gangsta","Top 40","Christian Rap","Pop/Funk","Jungle","Native American","Cabaret","New Wave","Psychedelic","Rave","Showtunes","Trailer","Lo-Fi","Tribal","Acid Punk","Acid Jazz","Polka","Retro","Musical","Rock & Roll","Hard Rock","Folk","Folk-Rock","National Folk","Swing","Fast Fusion","Bebop","Latin","Revival","Celtic","Bluegrass","Avantgarde","Gothic Rock","Progressive Rock","Psychedelic Rock","Symphonic Rock","Slow Rock","Big Band","Chorus","Easy Listening","Acoustic","Humour","Speech","Chanson","Opera","Chamber Music","Sonata","Symphony","Booty Bass","Primus","Porn Groove","Satire","Slow Jam","Club","Tango","Samba","Folklore","Ballad","Power Ballad","Rhythmic Soul","Freestyle","Duet","Punk Rock","Drum Solo","A Cappella","Euro-House","Dance Hall","Goa","Drum & Bass","Club-House","Hardcore Techno","Terror","Indie","BritPop","Negerpunk","Polsk Punk","Beat","Christian Gangsta Rap","Heavy Metal","Black Metal","Crossover","Contemporary Christian","Christian Rock","Merengue","Salsa","Thrash Metal","Anime","Jpop","Synthpop","Abstract","Art Rock","Baroque","Bhangra","Big Beat","Breakbeat","Chillout","Downtempo","Dub","EBM","Eclectic","Electro","Electroclash","Emo","Experimental","Garage","Global","IDM","Illbient","Industro-Goth","Jam Band","Krautrock","Leftfield","Lounge","Math Rock","New Romantic","Nu-Breakz","Post-Punk","Post-Rock","Psytrance","Shoegaze","Space Rock","Trop Rock","World Music","Neoclassical","Audiobook","Audio Theatre","Neue Deutsche Welle","Podcast","Indie Rock","G-Funk","Dubstep","Garage Rock","Psybient"};
bool powershell=false;
char tmp_path[MAX_PATH];

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

std::vector<std::string> split(std::string str, char letter=' ') {
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
  

int SSAL::audio::load(std::string file, audioArgs a/*={}*/) {
    char filename[1024];
    std::string cmd = "IF EXIST \""+file+"\" echo yes";
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
        GetShortPathNameA(file.c_str(),path, sizeof(path));
        SSAL::audio::file=file; arg=a;
    
        if (a.loops==-1) inf_loops=true;
    
        std::string szCmd = "Open \""+std::string(path)+"\" Type MPEGVideo alias SSAL-"+std::to_string(num);
        id="SSAL-"+std::to_string(num); num++;
        checkError(mciSendStringA( szCmd.c_str(), NULL, 0, NULL ));

        SSAL::track m(file);
        if (!powershell) {
            std::string powershell_script = "#1\nparam([string]$filename) \nfunction Get-Mp3Files() {\n$shellObject = New-Object -ComObject Shell.Application\n\n$file = Get-ChildItem \"$pwd\\$filename\" \n$directoryObject = $shellObject.NameSpace( $file.Directory.FullName )\n$fileObject = $directoryObject.ParseName( $file.Name )\n$directoryObject.GetDetailsOf( $fileObject, 27 )\n}\nGet-Mp3Files(0)";

            GetTempPathA(MAX_PATH+1, tmp_path);
            std::fstream powershell_file(std::string(tmp_path)+"RSAL.ps1", std::ios::trunc|std::ios::out);
            powershell_file << powershell_script;
            powershell_file.close();
            powershell=true;

        }
        std::string cmd = "powershell -ExecutionPolicy Bypass "+std::string(tmp_path)+"RSAL.ps1 "+path;
        char filename[1024];
        FILE* f = popen(cmd.c_str(), "r");
        fgets(filename, 1024, f);
        fclose(f);
        
        std::vector<std::string> list = split((std::string)filename, ':');
        m.duration={std::stoi(list[0]), std::stoi(list[1]), std::stoi(list[2].substr(0,list[2].find('\n'))) };
        
        track=m;
        if (arg.start_at!=0) { current_pos = {int(arg.start_at/60)/60, int(arg.start_at/60), arg.start_at%60 }; time_cache=arg.start_at; }
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
    if (arg.start_at>0) {szCmd+=" from "+std::to_string( int( (arg.start_at+4)*1000-800 ) ); }
    if (arg.end_at>0) {  szCmd+=" to "+std::to_string(int( (arg.end_at*arg.speed+4)*1000-800*arg.speed ) ); }

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
        
    if (arg.speed != 1.0) {
        std::string c = "set "+id+" speed "+std::to_string(int(arg.speed*1000) );
        checkError(mciSendStringA(c.c_str(), NULL, 0, 0 ));
    }
    start = std::chrono::system_clock::now();

    checkError(mciSendStringA(szCmd.c_str(), NULL, 0, 0 ));
}

void SSAL::audio::pause() {
    std::string szCmd = "pause "+id;
    checkError(mciSendStringA(szCmd.c_str(), NULL, 0, 0 ));
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

    done=true;
    if (res == "playing") {done=false;}
    return !done;
}

void SSAL::quit() { 
    mciSendString(TEXT("Close All"), NULL, 0, 0 ); 
    if (powershell) {
        std::string cmd = "rm "+std::string(tmp_path)+"RSAL.ps1";
        FILE* f = popen(cmd.c_str(), "r");
        fclose(f);
    }
}

SSAL::audio::audio(std::string file, audioArgs a/*={}*/) { 
    load(file, a); 
}

std::string SSAL::audio::getError() { 
    return error; 
}

int SSAL::audio::checkEvents() {
    if (isPlaying()) {
        std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now()-start;
        if (fps_lasttime < elapsed_seconds.count() - 1.0) {
            fps_lasttime = elapsed_seconds.count();
            time_cache += arg.speed;
            current_pos.s=time_cache;
            if (current_pos.s >= 60) {
                current_pos.m=time_cache/60;
                current_pos.s=int(time_cache)%60;
            }
            if (current_pos.m >= 60) {
                current_pos.h=current_pos.m/60;
                current_pos.m=current_pos.m%60;
            }
        }
    }
    else {
        if (arg.loops > 0 || inf_loops) {
            if (arg.loops != -1) arg.loops--;
            arg.start_at=0;
            arg.end_at=0;
            current_pos={};
    
            load(file,arg); 
            play(); 
        }
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
    bool image=false;
    for (int i=0x0a; i<0x1df20; i++) { 
        sprintf(str, "%c%c%c%c", data[i], data[i+1], data[i+2], data[i+3]);
        if (image) {
            for (int tags=0; tags<id3_v2_tags.size(); tags++) { 
                if (str == id3_v2_tags[tags] && id3_v2_tags[tags] != "TEXT") { 
                    image=false; break;
                }
            }
            if (image) cover.insert(cover.end(), data[i]);
        }
        else {
        
            std::string res = str;
            limit=0;
            if (id3_version > 2) {
                    
                if (res == "TIT2") { i+=6; title = findID3Data(i); }
                if (res == "TALB") { i+=6; album = findID3Data(i); }
                if (res == "TCOM") { i+=6; composers = split(findID3Data(i), '/'); }
                if (res == "TPE2") { i+=6; album_artist = findID3Data(i); }
                if (res == "TPE1") { i+=6; lead_artist = findID3Data(i); }
    
                if (res == "TYER") { i+=6; std::string g = findID3Data(i,5); if (g.size() >0) year = std::stoi(g); }
                if (res == "TLEN") { i+=6; float x = std::stoi(findID3Data(i,10))/1000; duration = {.h=0, .m=int(x/60), .s=int(x-(int(x/60)*60)) };}
                if (res == "TCON") { i+=6; genre=findID3Data(i,10); std::string x = genre.substr(1, genre.find(")")-1); if (isdigit(x[0])) {genre=id3v2_genre[std::stoi(x)];}  }
                if (res == "TRCK") { i+=6; track_number=std::stoi(findID3Data(i,2));}

                if (res == "jpeg" || res.find("png") != std::string::npos  || res.find("jpg") != std::string::npos ) {
                    image=true;
                    i+=6;
                }
            }
            else {
                if (res == "TT2") { limit += 7; i+=3; title = findID3Data(i,30); }
                if (res == "TAL") { limit += 7; i+=3; album = findID3Data(i,30); }
                if (res == "TCM") { limit += 7; i+=3; composers = split(findID3Data(i,true), '/'); }
                if (res == "TP1") { limit += 6; i+=3; album_artist = findID3Data(i,true); }
    
                if (res == "TYE") { limit += 7; i+=3; year = std::stoi(findID3Data(i,true)); }
                if (res == "TLE") { limit += 7; i+=3; float x = std::stoi(findID3Data(i,true))/1000; duration = {.h=0, .m=int(x/60), .s=int(x-(int(x/60)*60)) };}
                if (res == "TCO") { limit += 7; i+=3; genre=findID3Data(i,true); std::string x = genre.substr(1, genre.find(")")-1); if (isdigit(x[0])) {genre=id3v2_genre[std::stoi(x)];}  }
                if (res == "TRK") { limit += 7; i+=3; track_number=std::stoi(findID3Data(i,true));}
            }
            if (title.size() > 0 && album.size() > 0 && composers.size() > 0 && album_artist.size() > 0 && year > 0 && (duration.s > 0 && duration.m > 0) && genre.size() > 0 ) break;
        }
    }
}

std::string SSAL::track::findID3Data(int& i, int limit_count/*=0*/) {
    std::string result;
    bool done=false;

    for (int x=4; x<100; x++) {
        int pos = i+x;
        limit++;
        if (data[pos]>=32 && data[pos]<=126) {
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
        else if (limit >= limit_count && limit_count != 0) break;
    }
    return result;
}

void SSAL::audio::resetSettings() {
    arg={};
}