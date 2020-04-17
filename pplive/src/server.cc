#include <server.h>
#include <proto.h>
#include <session.h>
#include <iostream>
#include <arpa/inet.h>

namespace pplive {

    std::vector<std::shared_ptr<PPResourceNode>> PPToplyInfo::PickBestNode(const std::string & node_id){
        auto res = std::vector<std::shared_ptr<PPResourceNode>>(MAX_PICK_NUM);
        for( auto const it: _node_map) {
            res.push_back(it.second);
        }
        return res;   
    }

    std::shared_ptr<PPResourceNode> PPToplyInfo::FindNode(const std::string & node_id){
        auto it = _node_map.find(node_id);
        if (it == _node_map.end()){
            return nullptr;
        }
        return it->second;
    }

    int PPToplyInfo::UpdateToply(const std::string& node_id,  std::shared_ptr<PPResourceNode> parent_node, uint32_t host, uint32_t port,  u_int32_t weight) {
        auto it = _node_map.find(node_id);
        if (it == _node_map.end() ){
            // 未找到
            auto p = std::make_pair(node_id, PPResourceNode::CreatePPNode(node_id,_resource_id, weight,parent_node));
            p.second->_data_host = host;
            p.second->_data_port = port;
            _node_map.insert(p);
        }  else {
            // 找到了
            it->second->_parent_node = parent_node;
            it->second->_data_host = host;
            it->second->_data_port = port;
            it->second->_weight = weight;
        }

        return PP_OK;
  
    }

    int PPToplyInfo::RemoteToply(const std::string& node_id) {
        auto it = _node_map.find(node_id);
        if (it == _node_map.end() ){
            return  PP_NOT_FOUND;
        }     
        _node_map.erase(it);
        return PP_OK;  
    }

    void PPControllServer::Run(bool threading) {
        _loop->loop();
    }

    char * PPControllServer::genNodeId( char * str){
        node_id++;
        sprintf(str, "%016ld", node_id.load());
        return str;
    }

    int PPControllServer::CreateData(const std::string & resource_id) {
        auto it = _toply_map.find(resource_id); 
        if (it != _toply_map.end()){
            return PP_DUP;
        }
        _toply_map.insert(std::make_pair(resource_id, std::make_unique<PPToplyInfo>(resource_id)));
        return PP_OK;
    }


    int PPControllServer::CloseData(const std::string & resource_id) {
        auto it = _toply_map.find(resource_id); 
        if (it == _toply_map.end()) {
            return PP_NOT_FOUND;
        } 
        _toply_map.erase(it);
        return PP_OK;
    }



    PPControllServer::PPControllServer(const std::string & host, unsigned short port){
        _loop = std::make_unique<handy::EventBase>();

        _server = handy::HSHA::startServer(_loop.get(), host, port, 10);

        _server->server_->onConnState([&](const handy::TcpConnPtr conn){
            conn->context<PPNodeSession>();

            if (conn->getState() == handy::TcpConn::State::Connected){
                // 设置消息处理
                conn->onMsg(new handy::LineCodec, [&](const handy::TcpConnPtr& con, handy::Slice msg){
                    auto PP_msg = BaseMsg(msg.data());
                    int ret; //错误
                    switch (PP_msg.get_msg_type())
                    {
                    case MsgType::CONNECT:
                        // 建立连接
                        ret = handleMsgConnect(con,PP_msg);
                        break;
                    case MsgType::Fetch:
                        ret = handleMsgFetch(con, PP_msg);
                        break;
                    case MsgType::PING:
                        ret = handleMsgPing(conn);
                        break;
                    case MsgType::TOPLY_SYNC:
                        ret = handleMsgToplySync(con, PP_msg);
                        break;
                    case MsgType::DISCONNECT:
                        ret = handleDisConn(con, PP_msg);
                        break;
                    default:
                        break;
                    }
                });
            }

        });

    }

    int PPControllServer::handleMsgConnect(const handy::TcpConnPtr& conn,  const ConnectReqMsg& msg) {
        auto resp = NodeIdRespMsg();
        char node_id[DEFAULT_NODE_ID_LEN] = {0};
        strncpy(resp.ptr_node_id(), genNodeId(node_id), DEFAULT_NODE_ID_LEN);
        conn->sendMsg(resp.get_buf());
        conn->context<PPNodeSession>()._node_id = node_id;
        return PP_OK;
    };
    
    int PPControllServer::handleMsgFetch(const handy::TcpConnPtr& conn,  const FetchReqMsg& msg){
        auto resp = RedirectRespMsg();
        char resource_id[DEFAULT_RESOURCE_ID_LEN];
        strncpy(resource_id, msg.ptr_resource_id(), DEFAULT_RESOURCE_ID_LEN);
        auto it = _toply_map.find(resource_id);
        if(it == _toply_map.end()) {
            return PP_NOT_FOUND;
        }
        
        // 构造 resp
        auto pick_res = it->second->PickBestNode(conn->context<PPNodeSession>()._node_id);
        strncpy(resp.ptr_resouce_id(), resource_id, strlen(resource_id));
        resp.set_host_len(pick_res.size());
        for(int i=0;i<resp.get_host_len(); i++ ){
            resp.ptr_hosts()[i] = (uint32_t)::inet_addr(pick_res[i]->_data_host.c_str());
            resp.ptr_ports()[i] =  (uint16_t)pick_res[i]->_data_port;
        }
        //发回
        conn->sendMsg(resp.get_buf());
        return PP_OK;
    }

    
    int PPControllServer::handleMsgPing(const handy::TcpConnPtr& conn) {
        auto resp = PingReqMsg();
        conn->sendMsg(resp.get_buf());
        return PP_OK;
    }

    int PPControllServer::handleMsgToplySync(const handy::TcpConnPtr& conn, const ToplySyncReqMsg& msg){
        auto it = _toply_map.find(msg.ptr_resource_id());
        if (it == _toply_map.end()) {
            return PP_NOT_FOUND;
        }
        auto parent_node = it->second->FindNode(msg.ptr_parent_node_id());
        it->second->UpdateToply(conn->context<PPNodeSession>()._node_id, parent_node , msg.get_data_host(), msg.get_data_port(), msg.get_weight());
        return PP_OK;
    }
    
    int PPControllServer::RegistData(const std::string& node_id, const std::string & resource_id,const std::string & host, uint32_t port,  uint32_t weight){
        auto it = _toply_map.find(resource_id);
        if (it == _toply_map.end()) {
            return PP_NOT_FOUND;
        }    
        it->second->UpdateToply(node_id, nullptr, (uint32_t)::inet_addr(host.c_str()), port, weight); 
        return PP_OK;
    }
    

    int PPControllServer::handleDisConn(const handy::TcpConnPtr& conn, const DisConnectReqMsg& msg) {
        auto toply_it = _toply_map.find(msg.ptr_resource_id());
        if (toply_it != _toply_map.end()) {
            return PP_NOT_FOUND;
        }
        toply_it->second->RemoteToply(conn->context<std::string>());
        auto resp = SafeDisConnectRespMsg();
        strncpy(resp.ptr_resource_id(), msg.ptr_resource_id(), DEFAULT_RESOURCE_ID_LEN);
        conn->sendMsg(resp.get_buf());
        return PP_OK;
    }
    

}       