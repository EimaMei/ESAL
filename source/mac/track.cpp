#include "../../include/rsal.hpp"
#include <fstream>
#include <vector>
#include <algorithm>

std::vector<std::string> const id3_v2_tags = { "AENC", "APIC", "COMM", "COMR", "ENCR", "EQUA", "ETCO", "GEOB", "GRID", "IPLS", "LINK", "MCDI", "MLLT", "OWNE", "PRIV", "PCNT", "POPM", "POSS", "RBUF", "RVAD", "RVRB", "SYLT", "SYTC", "TALB", "TBPM", "TCOM", "TCON", "TCOP", "TDAT", "TDLY", "TENC", "TEXT", "TFLT", "TIME", "TIT1", "TIT2", "TIT3", "TKEY", "TLAN", "TLEN", "TMED", "TOAL", "TOFN", "TOLY", "TOPE", "TORY", "TOWN", "TPE1", "TPE2", "TPE3", "TPE4", "TPOS", "TPUB", "TRCK", "TRDA", "TRSN", "TRSO", "TSIZ", "TSRC", "TSSE", "TYER", "TXXX", "UFID", "USER", "USLT", "WCOM", "WCOP", "WOAF", "WOAR", "WOAS", "WORS", "WPAY", "WPUB", "WXXX" };
std::vector<std::string> const id3_v1_tags = { "BUF","CNT","COM","CRA","CRM","ETC","EQU","GEO","IPL","LNK","MCI","MLL","PIC","POP","REV","RVA","SLT","STC","TAL","TBP","TCM","TCO","TCR","TDA","TDY","TEN","TFT","TIM","TKE","TLA","TLE","TMT","TOA","TOF","TOL","TOR","TOT","TP1","TP2","TP3","TP4","TPA","TPB","TRC","TRD","TRK","TSI","TSS","TT1","TT2","TT3","TXT","TXX","TYE","UFI","ULT","WAF","WAR","WAS","WCM","WCP","WPB","WXX"};
const char* const id3v2_genre[192] = { "Blues","Classic Rock","Country","Dance","Disco","Funk","Grunge","Hip-Hop","Jazz","Metal","New Age","Oldies","Other","Pop","R&B","Rap","Reggae","Rock","Techno","Industrial","Alternative","Ska","Death Metal","Pranks","Soundtrack","Euro-Techno","Ambient","Trip-Hop","Vocal","Jazz+Funk","Fusion","Trance","Classical","Instrumental","Acid","House","Game","Sound Clip","Gospel","Noise","AlternRock","Bass","Soul","Punk","Space","Meditative","Instrumental Pop","Instrumental Rock","Ethnic","Gothic","Darkwave","Techno-Industrial","Electronic","Pop-Folk","Eurodance","Dream","Southern Rock","Comedy","Cult","Gangsta","Top 40","Christian Rap","Pop/Funk","Jungle","Native American","Cabaret","New Wave","Psychedelic","Rave","Showtunes","Trailer","Lo-Fi","Tribal","Acid Punk","Acid Jazz","Polka","Retro","Musical","Rock & Roll","Hard Rock","Folk","Folk-Rock","National Folk","Swing","Fast Fusion","Bebop","Latin","Revival","Celtic","Bluegrass","Avantgarde","Gothic Rock","Progressive Rock","Psychedelic Rock","Symphonic Rock","Slow Rock","Big Band","Chorus","Easy Listening","Acoustic","Humour","Speech","Chanson","Opera","Chamber Music","Sonata","Symphony","Booty Bass","Primus","Porn Groove","Satire","Slow Jam","Club","Tango","Samba","Folklore","Ballad","Power Ballad","Rhythmic Soul","Freestyle","Duet","Punk Rock","Drum Solo","A Cappella","Euro-House","Dance Hall","Goa","Drum & Bass","Club-House","Hardcore Techno","Terror","Indie","BritPop","Negerpunk","Polsk Punk","Beat","Christian Gangsta Rap","Heavy Metal","Black Metal","Crossover","Contemporary Christian","Christian Rock","Merengue","Salsa","Thrash Metal","Anime","Jpop","Synthpop","Abstract","Art Rock","Baroque","Bhangra","Big Beat","Breakbeat","Chillout","Downtempo","Dub","EBM","Eclectic","Electro","Electroclash","Emo","Experimental","Garage","Global","IDM","Illbient","Industro-Goth","Jam Band","Krautrock","Leftfield","Lounge","Math Rock","New Romantic","Nu-Breakz","Post-Punk","Post-Rock","Psytrance","Shoegaze","Space Rock","Trop Rock","World Music","Neoclassical","Audiobook","Audio Theatre","Neue Deutsche Welle","Podcast","Indie Rock","G-Funk","Dubstep","Garage Rock","Psybient"};

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

void ESAL::track::load(std::string filename) {
    if (filename != "") {
        std::ifstream file(filename, std::ios::binary);

        file.seekg(0, std::ios::end);
        std::streampos fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
    
        std::vector<unsigned char> fileData(fileSize);
        file.read((char*) &fileData[0], fileSize);
        data=fileData; 
        if (data[0x0] == 'I' && data[0x01] == 'D') {filetype="ID3"; version_id=data[0x03]; }
        else if (data[0x0] == 'f' && data[0x01] == 'L' && data[0x02] == 'a' && data[0x03] == 'C') {filetype="flac"; }
        getAll();
        data={};
        file.close();
    }
}

ESAL::track::track(std::string filename, bool load/*=true*/) { if (load) track::load(filename); }

void ESAL::track::getAll() {
    char str[99];
    bool image=false;

    bool ref=false;
    for (int i=0x0a; i<0x1df20; i++) { 
        if (filetype == "ID3") {
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
                if (version_id>=3) {
                        
                    if (res == "TIT2") { i+=6; title = findID3Data(i); }
                    if (res == "TALB") { i+=6; album = findID3Data(i); }
                    if (res == "TCOM") { i+=6; composers = split(findID3Data(i), '/'); }
                    if (res == "TPE2") { i+=6; album_artist = findID3Data(i); }
                    if (res == "TPE1") { i+=6; lead_artist = findID3Data(i); }
        
                    if (res == "TYER") { i+=6; std::string g = findID3Data(i,5); if (g.size() >0) year = std::stoi(g); }
                    if (res == "TLEN") { 
                        i+=6; int s = std::stoi(findID3Data(i,10))/1000; 
                        duration.h = s / 3600;
	                    duration.m = (s % 3600) / 60;
	                    duration.s = (s % 3600) % 60;
                    }
                    if (res == "TCON") { i+=6; genre=findID3Data(i,10); std::string x = genre.substr(1, genre.find(")")-1); if (isdigit(x[0])) {genre=id3v2_genre[std::stoi(x)];}  }
                    if (res == "TRCK") { i+=6; track_number=std::stoi(findID3Data(i,2));}
    
                    if (res == "jpeg" || res.find("png") != std::string::npos  || res.find("jpg") != std::string::npos ) {
                        image=true;
                        i+=6;
                    }
                }
                else if (version_id==2) {
                    if (res == "TT2") { limit += 7; i+=3; title = findID3Data(i,30); }
                    if (res == "TAL") { limit += 7; i+=3; album = findID3Data(i,30); }
                    if (res == "TCM") { limit += 7; i+=3; composers = split(findID3Data(i,true), '/'); }
                    if (res == "TP1") { limit += 6; i+=3; album_artist = findID3Data(i,true); }
        
                    if (res == "TYE") { limit += 7; i+=3; year = std::stoi(findID3Data(i,true)); }
                    if (res == "TLE") { 
                        limit += 7; i+=3; 
                        int s = std::stoi(findID3Data(i,true))/1000; 
                        duration.h = s / 3600;
	                    duration.m = (s % 3600) / 60;
	                    duration.s = (s % 3600) % 60;
                    }
                    if (res == "TCO") { limit += 7; i+=3; genre=findID3Data(i,true); std::string x = genre.substr(1, genre.find(")")-1); if (isdigit(x[0])) {genre=id3v2_genre[std::stoi(x)];}  }
                    if (res == "TRK") { limit += 7; i+=3; track_number=std::stoi(findID3Data(i,true));}
                }
            }
        }
        else if (filetype == "flac") {
            if (!ref && data[i] == 'r') {
                if (data[i+1] == 'e' && data[i+2] == 'f' && data[i+3] == 'e') { // 99.9% the correct path
                    i+=32;
                    ref=true;
                }
            }
            if (ref) {
                if (data[i]>=32 && data[i]<=126 && !search) {
                    sprintf(str, "%c%c%c%c", data[i], data[i+1], data[i+2], data[i+3]);
                    std::string res = str;
                    std::transform(res.begin(), res.end(), res.begin(), ::tolower);

                    if (res == "tle=") { i+=4; title=findFLACData(i); };
                    if (res == "bum=") { i+=4; album=findFLACData(i); };
                    if (res == "m ar") { i+=9; album_artist=findFLACData(i); }
                    if (res == "ser=") { i+=4; composers.insert(composers.end(), findFLACData(i));}
                    if (res == "ist=") { i+=4; lead_artist=findFLACData(i);}
                    if (res == "nre=") { i+=4; genre=findFLACData(i); }
                    if (res == "ate=") { i+=4; year=std::stoi(findFLACData(i));}
                    if (res == "ber=") { i+=4; track_number=std::stoi(findFLACData(i)); }
                }
            }
        }

        if (title.size() > 0 && album.size() > 0 && composers.size() > 0 && album_artist.size() > 0 && year > 0 && (duration.s > 0 && duration.m > 0) && genre.size() > 0 ) break;
    }
}

std::string ESAL::track::findFLACData(int& i) {
    std::string result;
    int non_ascii_count=0;
    for (int pos=i; pos<i+100; pos++) {
        if (data[pos]>=32 && data[pos]<=126) {
            if (non_ascii_count > 0) {
                result=result.substr(0, result.size()-(4-non_ascii_count));
                break;
            }
            result += data[pos];
        }
        else { non_ascii_count++;}
        if (non_ascii_count >=4)  break;
    }
    return result;
}

std::string ESAL::track::findID3Data(int& i, int limit_count/*=0*/) {
    std::string result;
    bool done=false;

    for (int x=4; x<100; x++) {
        int pos = i+x;
        limit++;
        if (data[pos]>=32 && data[pos]<=126) {
            char str[5];
            unsigned char tag[4] = { data[pos], data[pos+1], data[pos+2], data[pos+3] };

            if (version_id > 2) {
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
