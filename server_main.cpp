#include <iostream>
#include "pplive/include/server.h"
#include "pprtmp/include/pprtmp.h"

using namespace std;

int main() {
   auto pp_server =  pplive::PPControllServer("127.0.0.1", 10800);
   pp_server.CreateData("test1");
   pp_server.RegistData("0000000000","test1",  "rtmp://172.16.192.132:1935/test1", 0);
   pp_server.Run();
}