#include <iostream>
#include "pplive/include/server.h"

using namespace std;
int main() {
   auto pp_server =  pplive::PPControllServer("127.0.0.1", 10800);
   pp_server.CreateData("test1");
   pp_server.RegistData("0000000000","test1",  "127.0.0.1:10801", 0);
   pp_server.Run();
}