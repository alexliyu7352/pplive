#pragma once
#include <iostream>
#include <string>


namespace ffmpeg_cli {
    class FfmpegCli {
        private:
            std::string _uri;
        public:
            
            FfmpegCli(const std::string & uri):_uri(uri){}

            /**
             * @brief 开始播放
             * 
             * @param uri 
             */
            int StartPlay();

            /**
             * @brief 
             * 
             * @return int 
             */
            int StopPlay();
    };
}