#pragma once
#include <string>
#include <vector>
#include <utility>
#include <cstdint>
#include <cstring>
#include "config.h"
#include "util.h"
#include <iostream>
#include "json/json.h"

namespace pplive {


    

    enum class MsgType:uint8_t { // 消息类型
        CONNECT  = 1, // 链接
        NODE_ID,
        Fetch,
        REDIRECT,  // 重定向
        OK, // 成功,
        PING, // ping
        PONG, // 心跳回应
        TOPLY_SYNC, // 拓扑同步,
        DISCONNECT,
        SAFE_DISCONNECT, // 安全断开
        ATTR, // 属性同步
        ERROR, // 错误
    }; 


    class BaseDataAbc {
        public:
            BaseDataAbc() {};
            BaseDataAbc(const Json::Value & v) {};
            virtual void BindJson(const Json::Value & v) = 0;
            virtual Json::Value&  ToJson(Json::Value & v) const = 0;

    };

        
    class BaseMsg {
        /**
         * @brief 基本的消息
         * 
         */
        private:
            Json::Value _root;
            Json::Reader _reader;
            Json::FastWriter _writer;
        public:
            BaseMsg(const std::string & buf) {
                if (!_reader.parse(buf, _root)){
                    // 解析失败
                }
            }

            BaseMsg(MsgType type) {
                _root["data"] = Json::objectValue;
                _root["type"] = static_cast<uint8_t>(type);
            };

            virtual std::string ToString() {
                return _writer.write(_root);
            };

            /**
             * @brief 获取数据的 json
             * 
             * @return Json::Value 
             */
            Json::Value&  GetJsonDataRef(){
                return _root["data"];
            }
            
            /**
             * @brief 获取消息的类型
             * 
             * @return MsgType 
             */
            MsgType GetMsgType() {
                return MsgType(_root["type"].asInt());
            }

            /**
             * @brief Set the Msg object设置数据
             * 
             * @param msg_type 
             * @param data 
             */
            
            void SetMsg(MsgType msg_type, const BaseDataAbc& data){
                _root["type"] = static_cast<uint8_t>(msg_type);
                data.ToJson(_root["data"]);
            }

            void SetMsg(MsgType msg_type){
                _root["type"] = static_cast<uint8_t>(msg_type);
            }

            void SetMsg(const BaseDataAbc& data){
                data.ToJson(_root["data"]);
            }
    };

    class NodeIdData : public BaseDataAbc {
        public:
            std::string node_id;
        public:
            NodeIdData() {};
            NodeIdData(const Json::Value & v) : BaseDataAbc(v){
                BindJson(v);
            };
            virtual void BindJson(const Json::Value & v) override {
                    node_id = v["node_id"].asString();
            }
            virtual Json::Value& ToJson(Json::Value & v) const override {
                v["node_id"] = node_id;
                return v;
            }
    };

    class ResourceIdData : public BaseDataAbc {
        public:
            std::string resource_id;
        public:
            ResourceIdData() {};
            ResourceIdData(const Json::Value & v) : BaseDataAbc(v){
                BindJson(v);
            };
            virtual void BindJson(const Json::Value & v) override {
                    resource_id = v["resource_id"].asString();
            }
            virtual Json::Value& ToJson(Json::Value & v) const override {
                v["resource_id"] = resource_id;
                return v;
            }   
    };

    struct ServerInfoData: public BaseDataAbc{
        std::string node_id;
        std::string uri;

        ServerInfoData() {};
        ServerInfoData(const Json::Value & v) : BaseDataAbc(v){
            BindJson(v);
        };

        ServerInfoData( std::string node_id_, std::string uri_) : node_id(node_id_), uri(uri_){}

        virtual void BindJson(const Json::Value & v) override {
            node_id = v["node_id"].asString();
            uri = v["uri"].asString();
        }

        virtual Json::Value& ToJson(Json::Value & v) const override {
            v["node_id"] = node_id;
            v["uri"] = uri;
            return v;
        }
    };


    class RedirectData : public BaseDataAbc {
        public:
            std::vector<ServerInfoData> servers;
            std::string resource_id;
        public:
            RedirectData() {};
            RedirectData(const Json::Value & v) : BaseDataAbc(v){
                BindJson(v);
            };
            virtual void BindJson(const Json::Value & v)  override {
                resource_id = v["resource_id"].asString();
                for (auto s : v["servers"]) {
                    auto server_info = ServerInfoData(s);
                    servers.push_back(server_info);
                }
            }

            virtual Json::Value& ToJson(Json::Value & v) const override {
                v["resource_id"] = resource_id;
                for (auto s : servers) {
                    Json::Value server_info;
                    s.ToJson(server_info);
                    v["servers"].append(server_info);
                }
                return v;
            }
    };

    class ToplySyncData : public BaseDataAbc {
        public:
            std::string resource_id;
            ServerInfoData server;
            std::string  parent_id;
            int weight;
        public:
            ToplySyncData() {};
            ToplySyncData(const Json::Value & v) : BaseDataAbc(v){
                BindJson(v);
            };

            virtual void BindJson(const Json::Value & v) override {
                resource_id = v["resource_id"].asString();
                server.BindJson(v["server"]);
                parent_id = v["parent_id"].asString();
                weight = v["weight"].asInt();
            }
            
            virtual Json::Value& ToJson(Json::Value & v) const override {
                v["resource_id"] = resource_id;
                server.ToJson(v["server"]);
                v["parent_id"] = parent_id;
                v["weight"] = weight;
                return v;
            }
    };

    class ErrorMsg : public BaseDataAbc {
        public:
            int code;
            std::string msg; 
        public:
            ErrorMsg() {};
            ErrorMsg(const Json::Value & v) : BaseDataAbc(v){
                BindJson(v);
            };

            virtual void BindJson(const Json::Value & v) override {
                code = v["code"].asInt();
                msg = v["msg"].asString();
            }
            
            virtual Json::Value& ToJson(Json::Value & v) const override {
               v["code"] = code;
               v["msg"] = msg;
               return v;
            }
    };
}   

