#pragma once

#include <cstdint>
#include <string>
#include <iostream>
#include <algorithm>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>


namespace pplive{

    inline const int IP_V4 = sizeof(uint32_t);
    inline const int IP_V6 = sizeof(uint32_t) * 4;

    class ResourceUrl {
        /**
         * @brief 用于生成 Url
         * 
         */
        public:
            std::string _scheme;
            std::string _host;
            uint16_t _port;
            std::string _uri; // 资源
        public:
            /**
             * @brief Construct a new Resource Url object
             * 
             * @param scheme 
             * @param host 
             * @param port 
             * @param uri 
             */
            ResourceUrl(const std::string & scheme, const std::string & host, 
            uint16_t port, const std::string & uri): _scheme(scheme), _host(host),_port(port), _uri(uri) {};
            ResourceUrl() {};
            ResourceUrl(const std::string & url ){
                BuildFromUrl(url);
            }
            /**
             * @brief 生成 url
             * 
             * @return std::string 
             */
            inline std::string GenUrl() const {
                return (boost::format("%1%://%2%:%3%/%4%") % _scheme 
                % _host % _port % _uri).str();
            };

            inline void BuildFromUrl(const std::string & url) {
                const std::string scheme_flag = "://";
                const std::string host_flag = ":";
                const std::string port_flag = "/";
                auto schema_end = url.find_first_of(scheme_flag);
                _scheme = url.substr(0, schema_end);
               
                auto host_start = schema_end+scheme_flag.length();
                auto host_end = url.find_first_of(host_flag, host_start);
                _host = url.substr(host_start, host_end -host_start);

                auto port_start =  host_end + host_flag.length();
                auto port_end = url.find_first_of(port_flag, port_start);
                _port = std::stoi(url.substr(port_start, port_end-port_start).c_str());
                
                auto uri_start = port_end + port_flag.length();
                _uri = url.substr(uri_start);
                
            }
            
    };
}



