#include "sdk.h"

namespace pplive {

  D2DSDK::D2DSDK() {
      _loop = std::make_unique<handy::EventBase>();
      _server = std::make_shared<handy::TcpServer>(_loop.get());
      _server->bind(_config.d2d_host, _config.d2d_port);
  }

  


    
}