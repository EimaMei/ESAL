#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>

namespace SSAL{
    struct track {
        std::string title;
        std::string album;
        std::vector <std::string> composers={"",""};
        std::string album_artist;
        int id3_version;
        int year=0;

        private: 
            std::string filename; std::vector<BYTE> data;
            std::string findID3Data(int& i, bool genre=false);
        public:
            track(std::string filename="", bool load=true);
            void load(std::string filename);
            void getAll();
    };
    struct audio{
        void play();
        void stop();
        void pause();
        void resume();

        int checkEvents();
        bool isPlaying();
        void load(std::string file, int start=0, int end=0, int loops=0);
        std::string getError();

        bool done=false;
        std::string file;
        track track;

        private:
            std::string id, error;
            int Start,End, Loops;
            bool inf_loops=false;
            int checkError(int error_code, bool ssal_error=false);
        public: 
            audio(std::string file, int start=0, int end=0, int loops=0);
    };
    void quit();
};