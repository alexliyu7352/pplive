#pragma once

#include <string>
#include <vector>
#include <utility>
#include <cstdint>
#include <cstring>
#include "config.h"
#include "util.h"


namespace pplive {


    inline const int DEFAULT_NODE_ID_LEN = 20;

    // 普通字段的读写
    #define PROTO_FIELD_ACCESSER(type, name, addr) \
        int name##_offset() const { return addr; }; \
        const type get_##name() const { \
            return *(reinterpret_cast<type*>(const_cast<char*>(_buf.data() + addr)));\
        } \
        void set_##name( const type& val) { \
            *reinterpret_cast<type*>(_buf.data() + name##_offset()) = val; \
        }

    // 指针字段的读写
    #define PROTO_FIELD_PTR_ACCESSER(type, name, addr) \
        int name##_offset() const { return addr; }; \
        const type* cptr_##name() const {\
            return (reinterpret_cast<type*>(const_cast<char*>(_buf.data() + addr))); \
        }\ 
        type*  ptr_##name()  { \
            return reinterpret_cast<type*>( reinterpret_cast<type* const>(_buf.data()+addr)); \
        } \


    enum class MsgType:uint8_t { // 消息类型
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
    
    class BaseMsg  {

    public:
        std::vector<char> _buf; // buf

        const int BASE_OFFSET = sizeof(MsgType);

    public:
        
        BaseMsg(int cap=DEFAULT_MSG_SIZE){
            _buf.resize(cap);
        }

        BaseMsg(const char * buf){
            // 拷贝
            auto buf_len = strlen(buf);
            _buf.resize(buf_len*2);
            for(auto i =0; i<buf_len; i++ ){
                _buf.push_back(buf[i]);
            }
        }

        BaseMsg(BaseMsg && msg) noexcept{
            _buf = std::move(msg._buf);
        }

    public:
        // 处理字段
        PROTO_FIELD_ACCESSER(MsgType, msg_type, 0)
        PROTO_FIELD_PTR_ACCESSER(char, msg_body, BASE_OFFSET)
    };


    // 链接
    //  CONNECT
    class ConnectReqMsg : public BaseMsg {
        public:
            ConnectReqMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::CONNECT);
            }; 
            ConnectReqMsg(BaseMsg && msg):BaseMsg(std::move(msg)){};
    };

    // 连接成功返回 node_id
    //  CONNECT
    class NodeIdRespMsg : public BaseMsg {
        public:
            NodeIdRespMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::NODE_ID);
            }; 
            NodeIdRespMsg(BaseMsg && msg):BaseMsg(std::move(msg)){};
            
            PROTO_FIELD_PTR_ACCESSER(char, node_id, BASE_OFFSET)
    };

    // 获取资源
    // FETCH || RESOURCE_ID
    class FetchReqMsg : public BaseMsg {
        public:
            FetchReqMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::Fetch);
            }; 
            FetchReqMsg(BaseMsg && msg):BaseMsg(std::move(msg)){};
        public:
            PROTO_FIELD_PTR_ACCESSER(char, resource_id, BASE_OFFSET);
    };


    // 定向资源
    //  REDIRECT || RESOURCE_ID
    class RedirectRespMsg : public BaseMsg {
        public:
            RedirectRespMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::REDIRECT);
            }; 
            RedirectRespMsg(BaseMsg && msg):BaseMsg(std::move(msg)){};
            PROTO_FIELD_ACCESSER(uint8_t, host_len, BASE_OFFSET)
            PROTO_FIELD_PTR_ACCESSER(uint32_t, host, host_len_offset()) // ipv4 host
            PROTO_FIELD_PTR_ACCESSER(uint32_t, port,  host_offset() + sizeof(int32_t) * get_host_len()) // ipv4 port
    };

    // 获取资源成功
    // OK || NODE ID
    class OkRespMsg : public BaseMsg {
        public:
            OkRespMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::OK);
            }; 
            OkRespMsg(BaseMsg && msg):BaseMsg(std::move(msg)){};
            PROTO_FIELD_PTR_ACCESSER(char, node_id, BASE_OFFSET)
    };


    // ping 
    // NODE ID
    class PingReqMsg : public BaseMsg {
        public:
            PingReqMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::PING);
            }; 
            PingReqMsg(BaseMsg && msg):BaseMsg(std::move(msg)){};
            PROTO_FIELD_PTR_ACCESSER(char, node_id, BASE_OFFSET) // node id
    };

    // pong
    class PongRespMsg : public BaseMsg {
        public:
            PongRespMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::PONG);
            }; 
            PongRespMsg(BaseMsg && msg):BaseMsg(std::move(msg)){};
    };


    // 拓扑同步
    // 
    class ToplySyncReqMsg : public BaseMsg {
        public:
            ToplySyncReqMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::TOPLY_SYNC);
            }; 
            ToplySyncReqMsg(BaseMsg && msg):BaseMsg(std::move(msg)){};
            PROTO_FIELD_PTR_ACCESSER(char, node_id, BASE_OFFSET) // node id
            PROTO_FIELD_PTR_ACCESSER(char, resource_id, node_id_offset() + strnlen(cptr_node_id(), _buf.size())) // 资源 id
            PROTO_FIELD_PTR_ACCESSER(char, parent_node_id, resource_id_offset() + strnlen(cptr_resource_id(), _buf.size())) // 父辈节点
            PROTO_FIELD_ACCESSER(uint32_t, data_host, parent_node_id_offset() + strnlen(cptr_parent_node_id(), _buf.size())) // 父辈节点
            PROTO_FIELD_ACCESSER(uint32_t, data_port, data_host_offset() + sizeof(uint32_t)) // 父辈节点
            PROTO_FIELD_ACCESSER(uint32_t, weight, data_port_offset() + sizeof(uint32_t)) // 父辈节点
    };

    // 断开链接
    class DisConnectReaMsg : public BaseMsg {
        public:
            DisConnectReaMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::DISCONNECT);
            }; 
            DisConnectReaMsg(BaseMsg && msg):BaseMsg(std::move(msg)){};
            PROTO_FIELD_PTR_ACCESSER(char, node_id, BASE_OFFSET) // node id

    };


    // 错误
    // 
    class ErrorMsg : public BaseMsg {
        public:
            ErrorMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::ERROR);
            }; 
            ErrorMsg(BaseMsg && msg):BaseMsg(std::move(msg)){};
            PROTO_FIELD_ACCESSER(uint32_t, code, BASE_OFFSET ) // 父辈节点
            PROTO_FIELD_PTR_ACCESSER(char, msg, code_offset()+sizeof(get_code())) // 父辈节点
    };

}   

