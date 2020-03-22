#pragma once

#include <boost/core/noncopyable.hpp>
#include <string>
#include "handy/handy.h"
#include <functional>
#include <iostream>
#include <map>
#include <cstring>
 

namespace pplive {
    using D2DConnPtr = handy::TcpConnPtr;
    using DataConnPtr = handy::TcpConnPtr;
    using D2DConnCb = std::function<void(D2DConnPtr )>;
    using D2DDataCb = std::function<void(DataConnPtr)>;
    using D2DCb = std::function<void(DataConnPtr)>;
    using D2DErrorCb = std::function<void()>;
     
    class D2DSDK : public boost::noncopyable {
        public: 
            D2DSDK(); // 构建

        public: 
            int Connect(const std::string & host,  uint port); // 链接服务器
            void OnConnected(const D2DConnCb & cb); // 链接服务器成功的回调
            void Fetch(const std::string & resource); // 获取资源
            void OnFetched(const D2DDataCb &cb); // 获取资源成功的回调
            void DisConnect(); // 断开连接
            void OnDisConnect(const D2DCb & cb); //断开连接的回调
            void OnError(const D2DErrorCb & cb); // 错误时候的接口

        private:
            std::string _node_id;
            std::map<const std::string &, DataConnPtr> _data_conn_maps; // 连接的映射
            
            // 各种回调
            D2DConnCb _conn_cb;
            D2DDataCb _fetch_ch;
            D2DCb _dis_conn_cb;
            D2DErrorCb _err_cb;
    };
}