#pragma once
#include "handy/handy.h"
#include <functional>
#include <iostream>
#include <map>
#include <cstring>
#include <cstdint>
#include <utility>
#include <string>
#include <boost/core/noncopyable.hpp>



namespace pplive {
    using PPConnPtr = handy::TcpConnPtr;
    using DataConnPtr = handy::TcpConnPtr;
    using PPConnCb = std::function<void(PPConnPtr )>;
    using PPDataCb = std::function<void(DataConnPtr)>;
    using PPCb = std::function<void(DataConnPtr)>;
    using PPErrorCb = std::function<void()>;


    inline const int8_t PP_OK = 0;
    inline const int8_t PP_ERROR = -1;
    inline const int8_t PP_DUP = -2;
    inline const int8_t PP_NOT_FOUND = -4;

}