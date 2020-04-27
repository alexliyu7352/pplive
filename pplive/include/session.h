#pragma once

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
}
