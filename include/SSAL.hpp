#include <iostream>
#include <vector>
#include <chrono>
#define VERSION "SSAL 0.2.1"
#define MAX_SPEED 4 // BLUE STREAK (BLUE STREAK), SPEED BY, AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

namespace SSAL{
    struct time {
        int h=0;
        int m=0;
        int s=0;
    };
    struct track {
        std::string title;
        std::string album;
        std::vector <std::string> composers;
        std::string album_artist;
        std::string lead_artist;
        std::string genre;

        std::vector<unsigned char> cover;
        
        int id3_version;
        int year=0;
        int track_number=0;
        time duration;

        track(std::string filename="", bool load=true);
        void load(std::string filename);
        void getAll();

        private: 
            std::string filename; std::vector<unsigned char> data;
            int limit=0;
            std::string findID3Data(int& i, int limit_count=0);
    };
    struct Volume {
        int value=-1;
        bool left_side=false;
        bool right_side=false;
    };
    struct audioArgs {
        int start_at=0;
        int end_at=0;
        int loops=0;
        float speed=1.0;
        Volume volume;
    };
    struct audio{
        void play();
        void stop();
        void pause();
        void resume();

        int checkEvents();
        bool isPlaying();
        int load(std::string file, audioArgs a={});
        void resetSettings();
        std::string getError();

        bool done=false;
        std::string file;
        track track;
        time current_pos;
        bool valid=false;

        private:
            std::string id, error;
            audioArgs arg;
            bool inf_loops=false;
            int fps_lasttime=0;
            float time_cache=0.0;
            std::chrono::_V2::system_clock::time_point start;
            int checkError(int error_code, bool ssal_error=false);
        public: 
            audio(std::string file, audioArgs a={});
    };
    void quit();
};