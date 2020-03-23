#pragma once
#include <string>
#include <stdint.h>

namespace pplive {

    inline const std::string DEFAULT_D2D_HOST = "0.0.0.0";
    inline const uint32_t DEFAULT_D2D_PORT = 10086;
    inline const std::string DEFAULT_DATA_HOST = "0.0.0.0";    
    inline const uint32_t DEFAULT_DATA_PORT = 10087;
    inline const uint32_t DEFAULT_WEIGHT = 1000;


    struct D2DSDKConfig {
        std::string d2d_host = DEFAULT_D2D_HOST;
        uint32_t d2d_port = DEFAULT_D2D_PORT;
        std::string data_host = DEFAULT_DATA_HOST;
        uint32_t data_port = DEFAULT_DATA_PORT;
        uint32_t weight = DEFAULT_WEIGHT;
    };
}