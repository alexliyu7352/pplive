#pragma once
#include <iostream>
#include <string>
#include <utility>
#include "subprocess.hpp"


namespace bp = subprocess;

namespace ffmpeg_cli {
    class FfmpegCli {
        private:
            std::unique_ptr<bp::Popen> _proxy_bp;
            std::unique_ptr<bp::Popen> _player_bp;
        public:
            
            FfmpegCli(){}
            ~FfmpegCli(){
                if(_proxy_bp){
                    _proxy_bp->kill();
                }
                if (_player_bp){
                    _player_bp->kill();
                }
            }
            /**
             * @brief 开始播放
             * 
             * @param uri 
             */
            int StartPlay(const std::string & uri);
            
            /**
             * @brief 关闭播放
             * 
             * @return int 
             */
            int StopPlay();

            /**
             * @brief 代理转发器
             * 
             * @param src_url 
             * @param dst_url 
             * @return int 
             */
            int RtmpProxy(const std::string & src_url , const std::string & dst_url);

    };
}