#include "ffmpeg_cli.h"
#include <boost/format.hpp>

namespace ffmpeg_cli {
    int FfmpegCli::StartPlay() {
        return std::system((boost::format("ffplay %1%") % _uri).str().c_str());
    }

    int FfmpegCli::StopPlay() {
        return 0;
    }

}
