#pragma once
#include "handy/handy.h"
#include "proto.h"
#include <functional>
#include <iostream>
#include <map>
#include <cstring>
#include <cstdint>
#include <utility>
#include <string>
#include <boost/core/noncopyable.hpp>
#include <iostream>



namespace pplive {
    using PPConnPtr = handy::TcpConnPtr;
    using DataConnPtr = handy::TcpConnPtr;
    using PPConnCb = std::function<void()>;
    using PPDatab = std::function<void(const std::string& resource_id, const ServerInfoData & server_info)>;
    using PPRsoureceCb = std::function<void(const std::string& resource_id)>;
    using PPErrorCb = std::function<void()>;


    inline const int8_t PP_OK = 1;
    inline const int8_t PP_ERROR = -1;
    inline const int8_t PP_DUP = -2;
    inline const int8_t PP_NOT_FOUND = -4;


    inline const uint8_t DEFAULT_NODE_ID_LEN = 20;
}