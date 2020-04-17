#include <iostream>
#include "pplive/include/sdk.h"

using namespace std;
int main() {
    auto sdk = pplive::PPSDK();
    sdk.OnConnected([&](){
        cout<<"链接成功"<<endl;
        sdk.Fetch("test1");
    });
    sdk.OnFetched([](const std::string& resource_id, const std::string& host, uint16_t port){
        cout<<"获取成功"<< resource_id << " " << host <<endl;
    });
    sdk.Connect("127.0.0.1", 10800);
    sdk.Run();
}