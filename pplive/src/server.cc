#include "server.h"
#include "proto.h"
#include "session.h"
#include <arpa/inet.h>

namespace pplive {

    char * PPControllServer::genNodeId( char * str){
        node_id++;
        sprintf(str, "%016d", node_id);
        return str;
    }

    int PPControllServer::CreateData(const std::string & resource_id, const std::string & host, unsigned int port) {
        auto it = _toply_map.find(resource_id); 
        if (it != _toply_map.end()){
            return PP_DUP;
        }
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
                        ret = handleMsgConnect(con,  std::move(PP_msg));
                        break;
                    case MsgType::Fetch:
                        ret = handleMsgFetch(con, std::move(PP_msg));
                        break;
                    case MsgType::PING:
                        ret = handleMsgPing(conn);
                        break;
                    case MsgType::TOPLY_SYNC:
                        ret = handleMsgToplySync(con, PP_msg);
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
        auto pick_res = it->second.PickBestNode(conn->context<PPNodeSession>()._node_id);
        strncpy(resp.ptr_resouce_id(), resource_id, strlen(resource_id));
        resp.set_host_len(pick_res.size());
        for(int i=0;i<resp.get_host_len(); i++ ){
            resp.ptr_hosts()[i] = ::inet_addr(pick_res[i]->_data_host.c_str());
            resp.ptr_ports()[i] = pick_res[i]->_data_port;
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
        it->second.UpdateToply(conn->context<PPNodeSession>()._node_id, msg.ptr_parent_node_id(), msg.get_data_host(), msg.get_data_port(), msg.get_weight());
        return PP_OK;
    }



    

}       