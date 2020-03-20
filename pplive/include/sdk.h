#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/console.hpp>
#include "handy/handy.h"

#include <iostream>
 
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;

inline int hello(){
    std::cout<<"hoost"<<std::endl;
}
