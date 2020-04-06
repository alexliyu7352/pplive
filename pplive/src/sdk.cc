#include "sdk.h"

namespace pplive {

  PPSDK::PPSDK() {
      _loop = std::make_unique<handy::EventBase>();
      _server = std::make_shared<handy::TcpServer>(_loop.get());
      _server->bind(_config.PP_host, _config.PP_port);
  }

  void PPSDK::Connect(const std::string & host,  uint port) {
      _d2_conn = handy::TcpConn::createConnection(_loop.get(),_config.PP_host, _config.PP_port);

      _d2_conn->onMsg(new handy::LineCodec, [&](const handy::TcpConnPtr& con, handy::Slice msg){

      });

      _d2_conn->onState([&](const handy::TcpConnPtr& con){
          if(con->getState() == handy::TcpConn::State::Connected){
            // 连接成功
            this->handlePPConnect(con);
          } else if 
      })
  }

  

  


  
  


    
}