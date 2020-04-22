#include <iostream>
#include <boost/format.hpp>
#include "pplive/include/sdk.h"
#include "pprtmp/include/pprtmp.h"

using namespace std;
int main() {
    auto sdk = pplive::PPSDK();
        auto proxy_server = zl_cli::ZlHttpClient("172.16.192.132", 1080, "035c73f7-bb6b-4889-a715-d9eb2d1925cc");
    sdk.OnConnected([&](){
        cout<<"链接成功"<<endl;
        sdk.Fetch("test1");
    });
    sdk.OnFetched([&](const std::string& resource_id, const pplive::ServerInfoData& server_info) -> int {
        cout<<"获取成功"<< resource_id << " " << server_info.uri<<endl;
        std::string res_key;
        cout<<"test1"<<endl;
        auto ok = proxy_server.AddStreamProxy("127.0.0.1", "1" , "test1", server_info.uri, false, true, false, res_key);
        if (ok){
            auto ffplayer = ffmpeg_cli::FfmpegCli((boost::format("rtmp://%1%") % server_info.uri).str());
        }
        return pplive::PP_OK;
    });
    sdk.Connect("127.0.0.1", 10800);
    sdk.DisConnect("test1");
    sdk.Run();
}