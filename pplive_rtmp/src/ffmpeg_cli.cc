#include "ffmpeg_cli.h"

namespace ffmpeg_cli {
    int FfmpegCli::StartPlay() {
        std::vector<std::string> args;
        args.push_back(_uri); 
        _player = bp::child("ffplay", args);
        return 0;
    }

    int FfmpegCli::StopPlay() {
        _player.terminate();
        return 0;
    }

}
