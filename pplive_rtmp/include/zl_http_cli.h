#pragma once

#include <string>
#include <iostream>
#include <stdint.h>
#include <cstdlib>
#include "json/json.h"
#include "httplib.h"


namespace zl_cli {
    
    class ZlHttpClient {
         enum class ApiErr:int {
            Exception = -400,//代码抛异常
            InvalidArgs = -300,//参数不合法
            SqlFailed = -200,//sql执行失败
            AuthFailed = -100,//鉴权失败
            OtherFailed = -1,//业务代码执行失败，
            Success = 0//执行成功
        };
        private:
            std::string _host = "127.0.0.1";
            uint16_t _port = 1080;
            std::string _secret;
            Json::FastWriter _writer;
            Json::Reader _reader;
        public:
            ZlHttpClient() {};
            ZlHttpClient(const std::string host, uint16_t port, const std::string& secret): _host(host), _port(port), _secret(secret) {}; 

        public:
            /**
             * @brief 添加代理 返回值用 res_key 返回
             * 
             * @param vhost 
             * @param steam 
             * @param app 
             * @param uri 
             * @param enable_rtsp 
             * @param enable_rtmp 
             * @param enable_hls 
             * @param res_key 
             * @return int 
             */
            int AddStreamProxy(const std::string& vhost, const std::string& stream, const std::string& app, 
            const std::string& url, bool enable_rtsp, bool enable_rtmp, bool enable_hls, std::string& res_key);
            
            /**
             * @brief 删除代理
             * 
             * @param key 
             * @return int 
             */
            int DelStreamProxy(const std::string& key);
            
            /**
             * @brief 运行
             * 
             * @param path 
             * @return int 
             */
            int StartServer(const std::string & path);

    };
}