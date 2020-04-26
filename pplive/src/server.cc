#include <server.h>
#include <proto.h>
#include <session.h>
#include <iostream>
#include <algorithm>

namespace pplive {

    std::vector<std::shared_ptr<PPResourceNode>> PPToplyInfo::PickBestNode(const std::string & node_id){
        auto nodes = std::vector<std::shared_ptr<PPResourceNode>>();
        for( auto const it: _node_map) {
            nodes.push_back(it.second);
        }
        auto res = _load_balance->FetchNodes(nodes);
        return res;
    }

    std::shared_ptr<PPResourceNode> PPToplyInfo::FindNode(const std::string & node_id){
        auto it = _node_map.find(node_id);
        if (it == _node_map.end()){
            return nullptr;
        }
        return it->second;
    }

    int PPToplyInfo::UpdateToply(const std::string& node_id,  std::shared_ptr<PPResourceNode> parent_node, const std::string uri,  u_int32_t weight) {
        auto node_it = _node_map.find(node_id);
        if (node_it == _node_map.end() ){
            // 未找到
            auto p = std::make_pair(node_id, PPResourceNode::CreatePPNode(node_id,_resource_id, weight,parent_node));
            _node_map.insert(p);
            node_it = _node_map.find(node_id);
        } 
        node_it->second->_parent_node = parent_node;
        node_it->second->_uri = uri;
        node_it->second->_weight = weight;
        return PP_OK;  
    }

    int PPToplyInfo::RemoveToply(const std::string& node_id) {
        auto it = _node_map.find(node_id);
        if (it == _node_map.end() ){
            return  PP_NOT_FOUND;
        }
        _node_map.erase(it);
        return PP_OK;  
    }

    int PPToplyInfo::AddToply(const std::string& node_id) {
        _child_map.insert(std::make_pair(node_id, std::vector<std::shared_ptr<PPResourceNode>>()));
        _node_map.insert(std::make_pair(node_id, std::make_shared<PPResourceNode>()));
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
        _toply_map.insert(std::make_pair(resource_id, std::make_unique<PPToplyInfo>(resource_id,_load_balance->Clone())));
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

    PPControllServer::PPControllServer(const std::string& host,
                                       unsigned short port,
                                       LoadBalanceABC* load_balance)
        : _load_balance(load_balance) {
        _loop = std::make_unique<handy::EventBase>();

        _server = handy::HSHA::startServer(_loop.get(), host, port, 10);

        _server->server_->onConnState([&](const handy::TcpConnPtr conn){
            conn->context<PPNodeSession>();
            if (conn->getState() == handy::TcpConn::State::Connected){
                // 设置消息处理
                conn->onMsg(new handy::LineCodec, [&](const handy::TcpConnPtr& con, handy::Slice msg){
                    if (msg.empty()){
                        return;
                    }
                    auto PP_msg = BaseMsg(msg.toString());
                    std::cout<<"收到消息: " <<msg.toString()<<std::endl;
                    int ret; //错误
                    switch (PP_msg.GetMsgType())
                    {
                    case MsgType::CONNECT:
                        // 建立连接
                        std::cout<<"尝试连接"<<std::endl;
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

    int PPControllServer::handleMsgConnect(const handy::TcpConnPtr& conn,   BaseMsg & msg) {
        auto resp = BaseMsg(MsgType::NODE_ID);
        auto data = NodeIdData();
        char node_id[DEFAULT_NODE_ID_LEN] = {0};
        data.node_id = genNodeId(node_id);
        resp.SetMsg(data);
        conn->sendMsg(resp.ToString());
        conn->context<PPNodeSession>().node_id = node_id;
        _conn_map.insert(std::make_pair(node_id, conn));

        

        return PP_OK;
    };
    
    int PPControllServer::handleMsgFetch(const handy::TcpConnPtr& conn,  BaseMsg & msg){
        auto resp = BaseMsg(MsgType::REDIRECT);
        // 解析请求的信息
        auto resource_id_info = ResourceIdData();
        resource_id_info.BindJson(msg.GetJsonDataRef());
        auto toply_it = _toply_map.find(resource_id_info.resource_id);
        if(toply_it == _toply_map.end()) {
            return PP_NOT_FOUND;
        }
        
        // 构造 resp
        auto redirect_data = RedirectData();
        redirect_data.resource_id = resource_id_info.resource_id;
        auto pick_res = toply_it->second->PickBestNode(conn->context<PPNodeSession>().node_id);
        for(auto res : pick_res ){
            auto server_info = ServerInfoData(               
                res->_node_id,
                res->_uri);
            
            redirect_data.servers.push_back(
                server_info
            );

        }

        toply_it->second->AddToply(conn->context<PPNodeSession>().node_id);
        conn->context<PPNodeSession>().resouce_ids.push_back(resource_id_info.resource_id);
        resp.SetMsg(redirect_data);
        //发回
        conn->sendMsg(resp.ToString());
        return PP_OK;
    }

    
    int PPControllServer::handleMsgPing(const handy::TcpConnPtr& conn) {
        auto resp = BaseMsg(MsgType::PONG);
        conn->sendMsg(resp.ToString());
        return PP_OK;
    }

    int PPControllServer::handleMsgToplySync(const handy::TcpConnPtr& conn,  BaseMsg & msg){
        auto toply_info = ToplySyncData(msg.GetJsonDataRef());

        auto it = _toply_map.find(toply_info.resource_id);
        if (it == _toply_map.end()) {
            return PP_NOT_FOUND;
        }
        auto parent_node = it->second->FindNode(toply_info.parent_id);
        it->second->UpdateToply(conn->context<PPNodeSession>().node_id, parent_node , toply_info.server.resource.GenUrl(), toply_info.weight);
        return PP_OK;   
    }


    int PPControllServer::handleDisConn(const handy::TcpConnPtr& conn,  BaseMsg & msg) {
        auto resource_id_info = ResourceIdData(msg.GetJsonDataRef());

        // 先处理拓扑变动
        auto toply_it = _toply_map.find(resource_id_info.resource_id);
        if (toply_it != _toply_map.end()) {
            return PP_NOT_FOUND;
        }

        toply_it->second->RemoveToply(conn->context<std::string>());
        auto resouce_id_it = std::find( conn->context<PPNodeSession>().resouce_ids.begin(),conn->context<PPNodeSession>().resouce_ids.end(),resource_id_info.resource_id);
        conn->context<PPNodeSession>().resouce_ids.erase(resouce_id_it);
        
        auto resp = BaseMsg(MsgType::SAFE_DISCONNECT);
        resp.SetMsg(resource_id_info);
        conn->sendMsg(resp.ToString());
        return PP_OK;
    }
    
    int PPControllServer::RegistData(const std::string& node_id, const std::string & resource_id,const std::string & uri,  uint32_t weight){
        auto it = _toply_map.find(resource_id);
        if (it == _toply_map.end()) {
            return PP_NOT_FOUND;
        }    
        it->second->UpdateToply(node_id, nullptr, uri, weight); 
        return PP_OK;
    }
    

    

}       