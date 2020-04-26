#include <iostream>
#include <boost/format.hpp>
#include "pplive/include/sdk.h"
#include "pprtmp/include/pprtmp.h"

using namespace std;
int main() {
    auto sdk = pplive::PPSDK();
    auto ffpalyer = ffmpeg_cli::FfmpegCli();

    sdk.OnConnected([&](){
        cout<<"链接成功"<<endl;
        sdk.Fetch("test1");
    });
    sdk.OnFetched([&](const std::string& resource_id, const pplive::ServerInfoData& server_info) -> int {
        cout<<"获取成功"<< resource_id << " " << server_info.resource.GenUrl()<<endl;
        auto dst_url = (boost::format("rtmp://127.0.0.1:1935/%1%") % resource_id).str();
        ffpalyer.RtmpProxy(server_info.resource.GenUrl(), dst_url);
        ffpalyer.StartPlay(dst_url);
        sdk.RegistResource(resource_id, dst_url);
        return pplive::PP_OK;
    });
    sdk.Connect("127.0.0.1", 10800);
    sdk.DisConnect("test1");
    sdk.Run();
}