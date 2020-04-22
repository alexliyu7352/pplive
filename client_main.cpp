#include <iostream>
#include <boost/format.hpp>
#include "pplive/include/sdk.h"
#include "pplive_rtmp/include/pprtmp.h"

using namespace std;
int main() {
    auto sdk = pplive::PPSDK();

    auto proxy_server = zl_cli::ZlHttpClient();

    sdk.OnConnected([&](){
        cout<<"链接成功"<<endl;
        sdk.Fetch("test1");
    });
    sdk.OnFetched([&](const std::string& resource_id, const pplive::ServerInfoData& server_info) -> int {
        cout<<"获取成功"<< resource_id << " " << server_info.node_id<<endl;
        std::string res_key;
        auto ok = proxy_server.AddStreamProxy("127.0.0.1", 0, "test1", server_info.uri, false, true, false, res_key);
        if (ok){
            auto ffplayer = ffmpeg_cli::FfmpegCli((boost::format("rtmp://%1%") % server_info.uri).str());
        }

        return pplive::PP_OK;
    });
    sdk.Connect("127.0.0.1", 10800);
    sdk.DisConnect("test1");
    sdk.Run();
}