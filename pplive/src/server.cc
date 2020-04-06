#include "server.h"

namespace pplive {

    char * PPControllServer::genNodeId( char * str){
        node_id++;
        sprintf(str, "%016d", node_id);
        return str;
    }

    int PPControllServer::CreateData(const std::string & resource_id, const std::string & host, unsigned int port) {
        auto it = _data_server_maps.find(resource_id); 
        if (it != _data_server_maps.end()){
            return PP_DUP;
        }
        _data_server_maps.insert(std::make_pair(resource_id, PPDataServer::CreatePPDataServer(resource_id, host, port)));
        return PP_OK;
    }

    int PPControllServer::WriteData(const std::string & resource_id, const char * data ){
        auto it = _data_server_maps.find(resource_id);
        if (it == _data_server_maps.end()) {
            return PP_NOT_FOUND;
        }
        return it->second->WriteData(data);
    }

    int PPControllServer::CloseData(const std::string & resource_id) {
        auto it = _data_server_maps.find(resource_id);
        if (it == _data_server_maps.end()) {
            return PP_NOT_FOUND;
        } 
        _data_server_maps.erase(it);
        return PP_OK;
    }



    PPControllServer::PPControllServer(const std::string & host, unsigned short port){
        _loop = std::make_unique<handy::EventBase>();
        _server = handy::HSHA::startServer(_loop.get(), host, port, 10);

        _server->server_->onConnState([&](const handy::TcpConnPtr conn){
            
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

                    default:
                        break;
                    }
                });


            }

        });

    }

    int PPControllServer::handleMsgConnect(const handy::TcpConnPtr& conn,  const ConnectReqMsg& msg) {
        auto resp = NodeIdRespMsg();
        char node_id[20] = {0};
        strcpy(resp.ptr_node_id(), genNodeId(node_id));
        conn->sendMsg(resp._buf.data());
        conn->context<std::string>() = node_id;
        return PP_OK;
    };
    
    int PPControllServer::handleMsgFetch(const handy::TcpConnPtr& conn, const FetchReqMsg& msg){
        auto resp = RedirectRespMsg();
        
    }
    
    

}       