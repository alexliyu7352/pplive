#pragma once
#include <boost/core/noncopyable.hpp>
#include <string>
#include "handy/handy.h"
#include <functional>
#include <iostream>
#include <map>
#include <cstring>
#include <cstdint>
#include <utility>

#include "config.h"
#include "defs.h" 

namespace pplive {


    class PPSDK : public boost::noncopyable {

        public:
            enum class PPSDK_ROLE {
                IDLE,
                FETCHING,
                DATING
            };
        public: 
            PPSDK(); // 构建

        public: 
            void Connect(const std::string & host,  uint port); // 链接服务器
            void OnConnected(const PPConnCb & cb); // 链接服务器成功的回调
            void Fetch(const std::string & resource); // 获取资源
            void OnFetched(const PPDataCb &cb); // 获取资源成功的回调
            void DisConnect(); // 断开连接
            void OnDisConnect(const PPCb & cb); //断开连接的回调
            void OnError(const PPErrorCb & cb); // 错误时候的接口

        private:
            void handlePPConnect(const handy::TcpConnPtr& con);

        private:
            std::string _node_id;
            uint32_t _weight;
            PPSDK_ROLE _sdk_role;
            std::map<const std::string &, DataConnPtr> _data_conn_maps; // 连接的映射
            
            // 各种回调
            PPConnCb _conn_cb;
            PPDataCb _fetch_ch;
            PPCb _dis_conn_cb;
            PPErrorCb _err_cb;
        
        };  
}