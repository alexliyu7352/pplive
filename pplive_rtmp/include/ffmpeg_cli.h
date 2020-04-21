#pragma once
#include <iostream>
#include <string>
#include <boost/process.hpp>

namespace bp = boost::process; //we will assume this for all further examples

namespace ffmpeg_cli {
    class FfmpegCli {
        private:
            bp::child _player;
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