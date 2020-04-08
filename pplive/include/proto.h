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
    inline const int DEFAULT_RESOURCE_ID_LEN = 20;
    inline const int MAX_MSG_SIZE = 1024;

    // 普通字段的读写
    #define PROTO_FIELD_ACCESSER(type, name, addr) \
        int name##_offset() const { return addr; }; \
        const type get_##name() const { \
            return *(reinterpret_cast<type*>(const_cast<char*>(_buf + addr)));\
        } \
        void set_##name( const type& val) { \
            *reinterpret_cast<type*>(_buf + name##_offset()) = val; \
        }

    // 指针字段的读写
    #define PROTO_FIELD_PTR_ACCESSER(type, name, addr) \
        int name##_offset() const { return addr; }; \
        const type* ptr_##name() const {\
            return (reinterpret_cast<type*>(const_cast<char*>(_buf+ addr))); \
        }\ 
        type*  ptr_##name()  { \
            return reinterpret_cast<type*>( reinterpret_cast<type* const>(_buf+addr)); \
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
        char*  _buf; // buf
        int _cap;

        const int BASE_OFFSET = sizeof(MsgType);

    public:
        const char*  get_buf() const {
            return _buf;
        };

        BaseMsg(int cap=DEFAULT_MSG_SIZE) : _cap(cap) {
            _buf = new char[_cap];
            bzero(_buf, _cap);
        }

        BaseMsg(const char * buf){
            // 拷贝
            _cap = strnlen(buf, MAX_MSG_SIZE);
            _buf = new char[_cap];
            bzero(_buf, _cap);
            strncpy(_buf, buf, MAX_MSG_SIZE);
        }

        BaseMsg(const BaseMsg & msg) noexcept{
            _buf = new char[strnlen(msg._buf, MAX_MSG_SIZE)];
            bzero(_buf, _cap);
            strncpy(_buf, msg._buf, MAX_MSG_SIZE);
            _cap = msg._cap;
        }

        ~BaseMsg() {
            delete [] _buf;
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
            ConnectReqMsg(const BaseMsg & msg):BaseMsg(std::move(msg)){};
    };

    // 连接成功返回 node_id
    //  CONNECT
    class NodeIdRespMsg : public BaseMsg {
        public:
            NodeIdRespMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::NODE_ID);
            }; 
            NodeIdRespMsg(const BaseMsg & msg):BaseMsg(std::move(msg)){};
            
            PROTO_FIELD_PTR_ACCESSER(char, node_id, BASE_OFFSET)
    };

    // 获取资源
    // FETCH || RESOURCE_ID
    class FetchReqMsg : public BaseMsg {
        public:
            FetchReqMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::Fetch);
            }; 
            FetchReqMsg(const BaseMsg & msg):BaseMsg(std::move(msg)){};
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
            RedirectRespMsg(const BaseMsg & msg):BaseMsg(std::move(msg)){};
            PROTO_FIELD_PTR_ACCESSER(char, resouce_id, BASE_OFFSET) // ipv4 host
            PROTO_FIELD_ACCESSER(uint8_t, host_len, resouce_id_offset() + DEFAULT_RESOURCE_ID_LEN)
            PROTO_FIELD_PTR_ACCESSER(uint32_t, hosts, host_len_offset() + sizeof(uint8_t)) // ipv4 host
            PROTO_FIELD_PTR_ACCESSER(uint32_t, ports,  ports_offset() + get_host_len() * sizeof(uint32_t)) // ipv4 port
    };

    // 获取资源成功
    // OK || NODE ID
    class OkRespMsg : public BaseMsg {
        public:
            OkRespMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::OK);
            }; 
            OkRespMsg(const BaseMsg & msg):BaseMsg(std::move(msg)){};
            PROTO_FIELD_PTR_ACCESSER(char, node_id, BASE_OFFSET)
    };


    // ping 
    // NODE ID
    class PingReqMsg : public BaseMsg {
        public:
            PingReqMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::PING);
            }; 
            PingReqMsg(const BaseMsg & msg):BaseMsg(std::move(msg)){};
            PROTO_FIELD_PTR_ACCESSER(char, node_id, BASE_OFFSET) // node id
    };

    // pong
    class PongRespMsg : public BaseMsg {
        public:
            PongRespMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::PONG);
            }; 
            PongRespMsg(const BaseMsg & msg):BaseMsg(std::move(msg)){};
    };


    // 拓扑同步
    // 
    class ToplySyncReqMsg : public BaseMsg {
        public:
            ToplySyncReqMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::TOPLY_SYNC);
            }; 
            ToplySyncReqMsg(const BaseMsg & msg):BaseMsg(std::move(msg)){};
            PROTO_FIELD_PTR_ACCESSER(char, resource_id, BASE_OFFSET) // 资源 id
            PROTO_FIELD_PTR_ACCESSER(char, parent_node_id, resource_id_offset() + DEFAULT_RESOURCE_ID_LEN) // 父辈节点
            PROTO_FIELD_ACCESSER(uint32_t, data_host, parent_node_id_offset() + DEFAULT_NODE_ID_LEN) // 父辈节点
            PROTO_FIELD_ACCESSER(uint32_t, data_port, data_host_offset() + sizeof(uint32_t)) // 父辈节点
            PROTO_FIELD_ACCESSER(uint32_t, weight, data_port_offset() + sizeof(uint32_t)) // 父辈节点
    };

    // 断开链接
    class DisConnectReaMsg : public BaseMsg {
        public:
            DisConnectReaMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::DISCONNECT);
            }; 
            DisConnectReaMsg(const BaseMsg & msg):BaseMsg(std::move(msg)){};
            PROTO_FIELD_PTR_ACCESSER(char, resource_id, BASE_OFFSET) // node id

    };

    // 错误
    // 
    class ErrorMsg : public BaseMsg {
        public:
            ErrorMsg(int cap=DEFAULT_MSG_SIZE):BaseMsg(cap){
                set_msg_type(MsgType::ERROR);
            }; 
            ErrorMsg(const BaseMsg & msg):BaseMsg(std::move(msg)){};
            PROTO_FIELD_ACCESSER(uint32_t, code, BASE_OFFSET ) // 父辈节点
            PROTO_FIELD_PTR_ACCESSER(char, msg, code_offset()+sizeof(uint32_t)) // 父辈节点
    };

}   

