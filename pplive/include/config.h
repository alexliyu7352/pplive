#pragma once
#include <string>
#include <stdint.h>

namespace pplive {

    inline const std::string DEFAULT_PP_HOST = "0.0.0.0";
    inline const uint32_t DEFAULT_PP_PORT = 10086;
    inline const std::string DEFAULT_DATA_HOST = "0.0.0.0";    
    inline const uint32_t DEFAULT_DATA_PORT = 10087;
    inline const uint32_t DEFAULT_WEIGHT = 1000;
    
    inline const uint32_t DEFAULT_MSG_SIZE = 512;


    struct PPSDKConfig {
        std::string PP_host = DEFAULT_PP_HOST;
        uint32_t PP_port = DEFAULT_PP_PORT;
        std::string data_host = DEFAULT_DATA_HOST;
        uint32_t data_port = DEFAULT_DATA_PORT;
        uint32_t weight = DEFAULT_WEIGHT;
    };
}