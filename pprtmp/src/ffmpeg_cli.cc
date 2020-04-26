#include "ffmpeg_cli.h"
#include <boost/format.hpp>
#include <cstdio>


namespace ffmpeg_cli {
    int FfmpegCli::StartPlay(const std::string & uri) {
        if (_player_bp){
            _player_bp->kill();
        }
        std::vector<std::string> cmd = {"ffplay", uri};
        _player_bp = std::make_unique<bp::Popen>(cmd,  bp::output{"null"});
        return 0;
    }

    int FfmpegCli::StopPlay() {
        if(_player_bp) {
            _player_bp->kill();
            _player_bp.reset(nullptr);
        }
    }


    int FfmpegCli::RtmpProxy(const std::string & src_url , const std::string & dst_url) {
        if(_proxy_bp){
            _proxy_bp->kill();
        }
        std::vector<std::string> cmd = {"ffmpeg", "-re", "-i", src_url,  
        "-c:v", "libx264", "-f", "flv", dst_url};
        _proxy_bp = std::make_unique<bp::Popen>(cmd,  bp::output{bp::PIPE}, bp::input{bp::PIPE});
        return 0;
    }



}
