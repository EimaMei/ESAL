#include <iostream>
#include <vector>
#define VERSION "0.1.0"
#define MAX_SPEED 4 // BLUE STREAK (BLUE STREAK), SPEED BY, AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

namespace SSAL{
    struct track {
        std::string title;
        std::string album;
        std::vector <std::string> composers;
        std::string album_artist;
        int id3_version;
        int year=0;

        private: 
            std::string filename; std::vector<unsigned char> data;
            std::string findID3Data(int& i, bool genre=false);
        public:
            track(std::string filename="", bool load=true);
            void load(std::string filename);
            void getAll();
    };
    struct Volume {
        int value=-1;
        bool left_side=false;
        bool right_side=false;
    };
    struct audioArgs {
        int start_at=0;
        int end_at=-1;
        int loops=0;
        float speed=-1;
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
        bool valid=false;

        private:
            std::string id, error;
            audioArgs arg;
            bool inf_loops=false;
            int checkError(int error_code, bool ssal_error=false);
        public: 
            audio(std::string file, audioArgs a={});
    };
    void quit();
};