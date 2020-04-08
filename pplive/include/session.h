#pragma once
#include "handy/handy.h"
#include "defs.h"

namespace pplive {

    class PPNodeSession : public boost::noncopyable {
        public:
            enum class NodeControllStatus {
                HANDSHAKING, // 握手阶段
                CONNECTING,  // 连接中
                CLOSING, //关闭
                ERROR, //错误
            };

            NodeControllStatus _conn_status;
            std::string _node_id;
            std::vector<std::string> _resouce_ids;
        
            PPNodeSession() {
                _conn_status = NodeControllStatus::HANDSHAKING;
            }
    };
}
