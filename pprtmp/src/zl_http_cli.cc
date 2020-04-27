#include "zl_http_cli.h"
#include <boost/format.hpp>

namespace zl_cli {
    int ZlHttpClient::AddStreamProxy(const std::string& vhost, const std::string& stream, const std::string& app, 
            const std::string& url, bool enable_rtsp, bool enable_rtmp, bool enable_hls, std::string& res_key){
                  httplib::Client cli(_host, _port);
                  // 构建请求 body
                  Json::Value req;
                  Json::Value resp;
                  req["secret"] = _secret;
                  req["vhost"] = vhost;
                  req["app"] = app;
                  req["stream"] = stream;
                  req["url"] = url;
                  req["enable_rtsp"] = enable_rtsp;
                  req["enable_rtmp"] = enable_rtmp;
                  req["enable_hls"] = enable_hls;
                  std::cout<<"请求: "<<_writer.write(req)<<std::endl;                 
                  // 请求
                  auto res = cli.Post("/index/api/addStreamProxy", _writer.write(req), "application/json");
                  if (res && res->status == 200){
                      std::cout<<"响应: "<<res->body<<std::endl;
                      if (!_reader.parse(res->body, resp)){
                          return static_cast<int>(ApiErr::OtherFailed);
                      }
                      auto code = resp["code"].asInt();
                      if (code != 0){
                          return code;
                      }
                      res_key = resp["data"]["key"].asString();
                      std::cout<<"read_key: "<< res_key <<std::endl;
                      return static_cast<int>(ApiErr::Success);;
                  } 
            return static_cast<int>(ApiErr::OtherFailed);
        }
    
    int ZlHttpClient::DelStreamProxy(const std::string& key){
        httplib::Client cli(_host, _port);
        Json::Value req;
        Json::Value resp;

        req["secret"] = _secret;
        req["key"] = key;

        auto res = cli.Post("/index/api/delStreamProxy", _writer.write(req), "application/json");
        
        if (res && res->status == 200){
                      if (!_reader.parse(res->body, resp)){
                          return static_cast<int>(ApiErr::OtherFailed);
                      }
                      auto code = resp["code"].asInt();
                      if (code != 0){
                          return code;
                      }
                      return resp["data"]["flag"].asBool();
        } 
        return static_cast<int>(ApiErr::OtherFailed);
    }
    
    int ZlHttpClient::StartServer(const std::string & path) {
        return std::system( (boost::format("%1% -d") % path).str().c_str());
    }
    
    int ZlHttpClient::GetWatcherNum(const std::string & app, const std::string & schema) {
        httplib::Client cli(_host, _port);
        Json::Value req;
        Json::Value resp;

        req["app"] = app;
        req["schema"] = schema;

        auto res = cli.Post("/index/api/getMediaList", _writer.write(req), "application/json");
        
        if (res && res->status == 200){
                      if (!_reader.parse(res->body, resp)){
                          return static_cast<int>(ApiErr::OtherFailed);
                      }
                      auto code = resp["code"].asInt();
                      if (code != 0){
                          return code;
                      }
                      return resp["data"]["flag"].asBool();
        } 
        return static_cast<int>(ApiErr::OtherFailed);
    }

}