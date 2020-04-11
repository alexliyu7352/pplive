#include "sdk.h"
#include <arpa/inet.h>

namespace pplive {

  PPSDK::PPSDK() {
      _loop = std::make_unique<handy::EventBase>();
  }

  void PPSDK::Connect(const std::string & host,  uint16_t port) {
      _d2_conn = handy::TcpConn::createConnection(_loop.get(), host, port);

      // 注册一开始链接的时候的
      _d2_conn->onState([=](const handy::TcpConnPtr& con){
          if (con->getState() == handy::TcpConn::State::Connected) {
            // 处理链接成功
             auto conn_req_msg = ConnectReqMsg();
             con->sendMsg(conn_req_msg.get_buf());
          } else if (con->getState() == handy::TcpConn::State::Connected) {
            // 关闭连接
              _status = NodeControllStatus::CLOSING;
          }
      });

      _d2_conn->onMsg(new handy::LineCodec, [=](const handy::TcpConnPtr& con, handy::Slice msg){
          auto server_msg = BaseMsg(msg.data());
          int ret = 0;
          switch (server_msg.get_msg_type())
          {
          case MsgType::NODE_ID:
            // 处理链接
            ret = handlePPConnect(con, server_msg);
            break;
          
          case MsgType::REDIRECT:
            ret = handleRedirect(con, server_msg);
            break;
          default:
            break;
          }
      });


  }

  int PPSDK::handlePPConnect(const handy::TcpConnPtr& con, const NodeIdRespMsg & msg ) {
    if (_status == NodeControllStatus::HANDSHAKING) {
      _node_id = msg.ptr_node_id();
      _status = NodeControllStatus::CONNECTING;

    }

    return PP_OK;
  }

  
  int PPSDK::handleRedirect(const handy::TcpConnPtr& con, const RedirectRespMsg & msg ) {
      
      if(_status == NodeControllStatus::FETCHING) {
        int host_len = msg.get_host_len();
        if (host_len == 0) {
          return PP_NOT_FOUND;
        }
        struct in_addr ip_addr;
        ip_addr.s_addr = msg.ptr_hosts[0];
        std::string host = ::inet_ntoa(ip_addr);
        uint16_t port = msg.ptr_ports[0];
        _fetch_cb(host, port);
      }

      return PP_OK;
  }

  


  
  


    
}