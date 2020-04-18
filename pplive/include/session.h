#pragma once
#include "handy/handy.h"
#include "defs.h"

namespace pplive {

    enum class NodeControllStatus {
        IDLE,
        HANDSHAKING, // 握手阶段
        CONNECTING,  // 连接中
        FETCHING,
        CLOSING, //关闭
        ERROR, //错误

    };


    class PPNodeSession : public boost::noncopyable {
        /**
         * @brief 节点信息的上下文
         * 
         */
        public:
            NodeControllStatus conn_status;
            std::string node_id;
            std::vector<std::string> resouce_ids;
        
            PPNodeSession() {
                conn_status = NodeControllStatus::HANDSHAKING;
            }   
    };

    class PPResourceSession: public boost::noncopyable {
        /**
         * @brief 资源相关的上下文
         * 
         */
        public:
            std::string resource_id;
            std::string node_id;
            std::string uri;
        
        PPResourceSession(const std::string resource_id_, const std::string node_id_,  const std::string uri_) : resource_id(resource_id_), uri(uri_), node_id(node_id_){

        }
    };
    
}
