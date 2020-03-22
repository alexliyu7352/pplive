#pragma once

#include <string>
#include <vector>
#include <utility>
#include <cstdint>
#include <cstring>
#include "util.h"


namespace pplive {



    // 普通字段的读写
    #define PROTO_FIELD_ACCESSER(type, name, addr) \
        type get_##name() { \
            return *reinterpret_cast<type*>(_buf.data() + addr); \
        } \
        void set_##name( const type& val) { \
            *reinterpret_cast<type*>(_buf.data() + addr) = val; \
        }

    // 指针字段的读写
    #define PROTO_FIELD_PTR_ACCESSER(type, name, addr) \
        type* ptr_##name() { \
            return reinterpret_cast<type*>(_buf.data() + addr); \
        } 


    enum class MsgType { // 消息类型
        CONNECT  = 0, // 链接
        NODE_ID,
        Fetch,
        REDIRECT,  // 重定向
        OK, // 成功,
        PING, // ping
        PONG, // 心跳回应
        TOPLY_SYNC, // 拓扑同步,
        DISCONNECT,
        ATTR, // 属性同步
        ERROR, // 错误
    }; 
    
    class BaseMsg {
    
    public:
        std::vector<char> _buf; // buf

        std::vector<char>GetBuf() {
            // 获取一个 buf
            return _buf;
        };

        void SetBuf(std::vector< char> & buf) {
            // 设置 buf
            _buf = buf;
        };
        void SetBuf(std::vector< char> && buf ) {
            // 设置 buf 移动
            _buf = std::move(buf);
        };

        const int BASE_OFFSET = sizeof(MsgType);

    public:
        // 处理字段
        PROTO_FIELD_ACCESSER(MsgType, msg_type, 0)
        PROTO_FIELD_PTR_ACCESSER(char, msg_body, BASE_OFFSET)
        
    };


    // 链接
    //  CONNECT
    class ConnectReqMsg : public BaseMsg {

    };

    // 节点 ID 
    // NOID_ID
    class NodeIdRespMsg: public BaseMsg {
        public:
            PROTO_FIELD_ACCESSER(MsgType, msg_type, 0)
    };

    // 获取资源
    // FETCH || RESOURCE_ID
    class FetchReqMsg : public BaseMsg {
        public:
            PROTO_FIELD_PTR_ACCESSER(char, resource_id, BASE_OFFSET)
    };


    // 定向资源
    //  REDIRECT || RESOURCE_ID
    class RedirectRespMsg : public BaseMsg {
        public:
            PROTO_FIELD_ACCESSER(uint8_t, host_len, BASE_OFFSET)
            PROTO_FIELD_PTR_ACCESSER(uint32_t, host, BASE_OFFSET + sizeof(get_host_len())) // ipv4 host
            PROTO_FIELD_PTR_ACCESSER(uint32_t, port,  BASE_OFFSET + sizeof(get_host_len())  + (sizeof(uint32_t) * get_host_len())) // ipv4 port
    };

    // 获取资源成功
    // OK || NODE ID
    class OkRespMsg : public BaseMsg {
        public:
            PROTO_FIELD_PTR_ACCESSER(char, node_id, BASE_OFFSET)
    };


    // ping 
    // NODE ID
    class PingReqMsg : public BaseMsg {
        public:
            PROTO_FIELD_PTR_ACCESSER(char, node_id, BASE_OFFSET) // node id
    };

    // pong
    class PongRespMsg : public BaseMsg {};


    // 拓扑同步
    // 
    class ToplySyncReqMsg : public BaseMsg {
        public:
            PROTO_FIELD_PTR_ACCESSER(char, node_id, BASE_OFFSET) // node id
            PROTO_FIELD_PTR_ACCESSER(char, resource_id, BASE_OFFSET + strnlen(ptr_node_id(), _buf.size())) // 资源 id
            PROTO_FIELD_PTR_ACCESSER(char, parent_node_id, BASE_OFFSET + strnlen(ptr_resource_id(), _buf.size()) + strnlen(ptr_node_id(), _buf.size())) // 父辈节点
            PROTO_FIELD_ACCESSER(uint32_t, data_host, BASE_OFFSET + strnlen(ptr_resource_id(), _buf.size()) + strnlen(ptr_node_id(), _buf.size())) // 父辈节点
            PROTO_FIELD_ACCESSER(uint32_t, data_port, BASE_OFFSET + strnlen(ptr_resource_id(), _buf.size()) + strnlen(ptr_node_id(), _buf.size()) + sizeof(uint32_t)) // 父辈节点
            PROTO_FIELD_ACCESSER(uint32_t, weight, BASE_OFFSET + strnlen(ptr_resource_id(), _buf.size()) + strnlen(ptr_node_id(), _buf.size()) + sizeof(uint32_t) + sizeof(uint32_t)) // 父辈节点
    };

    // 断开链接
    class DisConnectReaMsg : public BaseMsg {
        public:
            PROTO_FIELD_PTR_ACCESSER(char, node_id, BASE_OFFSET) // node id

    };


    // 错误
    // 
    class ErrorMsg : public BaseMsg {
        public:
            PROTO_FIELD_ACCESSER(uint32_t, code, BASE_OFFSET ) // 父辈节点
            PROTO_FIELD_PTR_ACCESSER(char, msg, BASE_OFFSET + sizeof(uint32_t)) // 父辈节点
    };

}   