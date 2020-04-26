#include <sdk.h>

namespace pplive {

  PPSDK::PPSDK() {
      _loop = std::make_unique<handy::EventBase >( );
  }

  void PPSDK::Run(bool threading) {
      if (threading) {
          _thread = std::thread([&](){_loop->loop();});
      } else {
        _loop->loop();
      }
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
    RegistResource(resource_id, "");
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

  void PPSDK::OnSafeDisConnect(const PPRsoureceCb & cb) {
    _dis_conn_cb = cb;
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
    auto server = redirect_info.servers[0]; //TODO: ping 完了选最快的
    int ret = _fetch_cb(redirect_info.resource_id, server); // 回调
    if (ret != PP_OK) {
      // 失败了
      throw std::runtime_error("资源使用失败");
    }
    // 建立本地的拓扑关系
    auto parent_resource = PPResourceNode::CreatePPNode(
      server.node_id,
      redirect_info.resource_id,
      0,
      nullptr
    );
    auto resource_it = _resource_map.find(redirect_info.resource_id);
    if (resource_it == _resource_map.end()){
      RegistResource(redirect_info.resource_id, "");
      resource_it = _resource_map.find(redirect_info.resource_id);
    }
    resource_it->second->_parent_node  = parent_resource;
    syncToply(redirect_info.resource_id);
    return PP_OK;
  }


    
  int PPSDK::handleSafeDisConnect(const handy::TcpConnPtr& con,  BaseMsg & msg){
    auto resource_id_info = ResourceIdData(msg.GetJsonDataRef());
   _dis_conn_cb(resource_id_info.resource_id);
    return PP_OK;
  }

  int PPSDK::syncToply(const std::string resource_id) {
    auto msg = BaseMsg(MsgType::TOPLY_SYNC);
    auto toply_data = ToplySyncData();
    auto resource_it = _resource_map.find(resource_id);
    if (resource_it == _resource_map.end()){
      return PP_NOT_FOUND;
    }
    toply_data.parent_id= resource_it->second->_parent_node->_node_id;
    toply_data.server.node_id = _node_id;
    toply_data.server.resource.BuildFromUrl(resource_it->second->_url);
    toply_data.resource_id = resource_id;
    toply_data.weight = 0;

    msg.SetMsg(toply_data);
    _d2_conn->sendMsg(msg.ToString());
    std::cout<<"拓扑同步: "<<resource_id <<std::endl;
    return PP_OK;
  }

  void PPSDK::RegistResource(std::string resrouce_id, std::string uri){
    auto it = _resource_map.find(resrouce_id);
    if (it == _resource_map.end()){
      // 没找到插入
      auto resource_node = PPResourceNode::CreatePPNode(
        _node_id,
        resrouce_id,
        0,
        nullptr
      );
      resource_node->_url = uri;
      _resource_map.insert(std::make_pair(resrouce_id,resource_node));
    } else {
      //找到修改
        it->second->_url = uri;
    }
  }

}