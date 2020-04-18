#include <sdk.h>
#include <arpa/inet.h>

namespace pplive {

  PPSDK::PPSDK() {
      _loop = std::make_unique<handy::EventBase >( );
  }

  void PPSDK::Run(bool threading) {
      _loop->loop();
  }

  void PPSDK::Connect(const std::string & host,  uint16_t port) {
      _d2_conn = handy::TcpConn::createConnection(_loop.get(), host, port);

      // 注册一开始链接的时候的
      _d2_conn->onState([&](const handy::TcpConnPtr& con){
          if (con->getState() == handy::TcpConn::State::Connected) {
            // 处理链接成功
             auto conn_req_msg = BaseMsg(MsgType::CONNECT);
             con->sendMsg(conn_req_msg.ToString());
             _status = NodeControllStatus::HANDSHAKING;
          } else if (con->getState() == handy::TcpConn::State::Connected) {
            // 关闭连接
              _status = NodeControllStatus::CLOSING;
          }
      });

      _d2_conn->onMsg(new handy::LineCodec, [=](const handy::TcpConnPtr& con, handy::Slice msg){
          if (msg.empty()){
            return;
          }
          auto server_msg = BaseMsg(msg.toString());
          int ret = 0;
          std::cout<<"收到消息: " <<msg.toString()<<std::endl;
          switch (server_msg.GetMsgType())
          {
          case MsgType::NODE_ID:
            // 处理链接
            ret = handlePPConnect(con, server_msg);
            break;
          
          case MsgType::REDIRECT:
            ret = handleRedirect(con, server_msg);
            break;

          case MsgType::SAFE_DISCONNECT:
            ret = handleSafeDisConnect(con, server_msg);
            break;

          default:
            break;
          }
      });


  }

  void PPSDK::OnConnected(const PPConnCb & cb) {
    _conn_cb = cb;
  }

  int PPSDK::Fetch(const std::string & resource_id) {
    auto req = BaseMsg(MsgType::Fetch);
    auto resource_id_info = ResourceIdData();
    resource_id_info.resource_id = resource_id;
    req.SetMsg(resource_id_info);
    _d2_conn->sendMsg(req.ToString());
    return PP_OK;
  }


  void PPSDK::OnFetched(const PPDatab &cb) {
    _fetch_cb = cb;
  }


  int PPSDK::DisConnect(const std::string & resource_id) {
    auto req = BaseMsg(MsgType::DISCONNECT);
    auto resource_id_info = ResourceIdData();
    resource_id_info.resource_id = resource_id;
    req.SetMsg(resource_id_info);

    _d2_conn->sendMsg(req.ToString());   
    return PP_OK;
  }


  int PPSDK::handlePPConnect(const handy::TcpConnPtr& con, BaseMsg & msg ) {
    if (_status == NodeControllStatus::HANDSHAKING) {
      auto node_id_info = NodeIdData(msg.GetJsonDataRef());
      _node_id = node_id_info.node_id;
      _status = NodeControllStatus::CONNECTING;
      _conn_cb();
    }

    return PP_OK;
  }

  
  int PPSDK::handleRedirect(const handy::TcpConnPtr& con,  BaseMsg & msg ) {
    auto redirect_info = RedirectData(msg.GetJsonDataRef());
    if (redirect_info.servers.empty() ){
      return PP_NOT_FOUND;
    }
    _fetch_cb(redirect_info.resource_id, redirect_info.servers[0]);
    return PP_OK;
  }



    
  int PPSDK::handleSafeDisConnect(const handy::TcpConnPtr& con,  BaseMsg & msg){
    auto resource_id_info = ResourceIdData(msg.GetJsonDataRef());
   _dis_conn_cb(resource_id_info.resource_id);
    return PP_OK;
  }

}