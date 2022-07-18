#include <string>
#include <vector>
#define VERSION "ESAL 0.3.1"
#define timeToSeconds(time)  ((time.h*3600) + (time.m*60) + (time.s))
#define secondsToTime(seconds)  {(seconds / 3600), ((seconds % 3600) / 60), ((seconds % 3600) % 60)}

#define SAMPLE_RATE_48000        48000
#define SAMPLE_RATE_44100        44100
#define SAMPLE_RATE_48000        48000
#define SAMPLE_RATE_32000        32000
#define SAMPLE_RATE_22050        22050
#define SAMPLE_RATE_16000        16000
#define SAMPLE_RATE_8000         8000
#define SAMPLE_RATE_UNDERWATER   2000

namespace ESAL{
    struct time {
        int h;
        int m;
        int s;
    };
    struct track {
        //std::string title;
        //std::string album;
        //std::vector <std::string> composers;
        //std::string album_artist;
        //std::string lead_artist;
        //std::string genre;
//
        //std::vector<unsigned char> cover;
        //
        //int year=0;
        //int track_number=0;
        ESAL::time duration;
//
        ////track(std::string filename="", bool load=true);
        //void load(std::string filename);
        //void getAll();
//
        //private: 
        //    std::string filename; std::vector<unsigned char> data;
        //    int limit=0;
        //    std::string filetype;
        //    int version_id;
        //    bool search=false;
        //    std::string findID3Data(int& i, int limit_count=0);
        //    std::string findFLACData(int& i);
    };
    struct audioArgs {
        time start;
        time end;
        int loops;
        float speed=1.0;
        float pitch=0;
        int volume=100;
        bool correctPitch=true;
        int sampleRate=SAMPLE_RATE_44100;
        int fadeIn;
        int fadeOut;
        int deadSpace;
        //int wetDryMix;
    };
    struct audio{
        int play();
        int stop();
        int pause();
        int resume();

        int checkEvents();
        bool isPlaying();
        int seekTo(time t); int seekTo(const char* format);
        std::string getError();
        audio(std::string file, audioArgs a={});

        std::string file;
        track track;
        time current={0,0,0};
        bool done=false;

        private:
            int num;
            int error_code=0;
            audioArgs arg;
            time old={0,0,0};
            bool inf_loops=false;
            int function=-2;
    };
    void quit();
};