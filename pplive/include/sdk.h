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
#include <thread>
#include "config.h"
#include "defs.h"
#include "proto.h"
#include "node.h"

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
            
            /**
             * @brief 运行 sdk
             * 
             * @param threading 是否在另一线程中运行 
             */
            void Run(bool threading = false);
        public: 
            /**
             * @brief 链接服务器
             * 
             * @param host 
             * @param port 
             */
            void Connect(const std::string & host,  uint16_t port); 

            /**
             * @brief 链接成功回调
             * 
             * @param cb 
             */
            void OnConnected(const PPConnCb & cb); // 链接服务器成功的回调

            /**
             * @brief 获取某个资源
             * 
             * @param resource 
             */
            int Fetch(const std::string & resource_id); // 获取资源

            /**
             * @brief 获取资源成功
             * 
             * @param cb 
             */
            void OnFetched(const PPDatab &cb); // 获取资源成功的回调

            /**
             * @brief 断开连接
             * 
             */
            int DisConnect(const std::string & resource_id); // 断开连接

            /**
             * @brief 可以安全断开
             * 
             * @param cb 
             */
            void OnSafeDisConnect(const PPRsoureceCb & cb); //断开连接的回调

            /**
             * @brief 错误
             * 
             * @param cb 
             */
            void OnError(const PPErrorCb & cb); // 错误时候的接口
            
            /**
             * @brief 注册资源
             * 
             * @param resrouce_id 
             * @param uri 
             */
            void RegistResource(std::string resrouce_id, std::string uri); 
        private:
            
            /**
             * @brief 处理链接成功也就是 node id resp
             * 
             * @param con 
             * @param msg 
             */
            int handlePPConnect(const handy::TcpConnPtr& con,  BaseMsg & msg );

            /**
             * @brief 处理重定向
             * 
             * @param con 
             * @param msg 
             * @return int 
             */
            int handleRedirect(const handy::TcpConnPtr& con,  BaseMsg & msg);

            /**
             * @brief 处理 ping
             * 
             * @param con 
             * @param msg 
             * @return int 
             */
            int handlePing(const handy::TcpConnPtr& con,  BaseMsg & msg);
            
            /**
             * @brief 处理链接断开
             * 
             * @param con 
             * @param msg 
             * @return int 
             */
            int handleSafeDisConnect(const handy::TcpConnPtr& con,  BaseMsg & msg); 

            /**
             * @brief 同步拓扑
             * 
             * @param resource_id 
             * @return int 
             */
            int syncToply(const std::string resource_id);

        private:

            std::thread _thread;
            std::string _node_id;
            uint32_t _weight;

            NodeControllStatus _status;

            handy::TcpConnPtr _d2_conn;
            std::unique_ptr<handy::EventBase> _loop;
            PPSDKConfig _config;

            // 各种回调
            PPConnCb _conn_cb;
            PPDatab _fetch_cb;
            PPRsoureceCb _dis_conn_cb;
            PPErrorCb _err_cb;

            std::map<std::string, std::shared_ptr<PPResourceNode>> _resource_map;

        };  
}