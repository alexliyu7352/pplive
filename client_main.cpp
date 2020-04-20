#include <iostream>
#include "pplive/include/sdk.h"

using namespace std;
int main() {
    auto sdk = pplive::PPSDK();
    sdk.OnConnected([&](){
        cout<<"链接成功"<<endl;
        sdk.Fetch("test1");
    });
    sdk.OnFetched([&](const std::string& resource_id, const pplive::ServerInfoData& server_info) -> int {
        cout<<"获取成功"<< resource_id << " " << server_info.node_id<<endl;
        // 新建 rtmp 客户端 根据 server_info 信息获取数据
        // 启动 rtmp 服务器 
        return pplive::PP_OK;
    });
    sdk.Connect("127.0.0.1", 10800);
    sdk.DisConnect("test1");
    sdk.Run();
}