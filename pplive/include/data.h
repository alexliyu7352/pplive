#include "defs.h"
#include <boost/core/noncopyable.hpp>

namespace pplive {

    class PPDataServer: public boost::noncopyable {
        public:
            int WriteData(const char * data);
            static PPDataServerPtr CreatePPDataServer(const std::string &resource_id,  const std::string & host, unsigned int port);
        private:
            std::string _resource_id;
            std::vector<handy::TcpConnPtr> _conns;
            handy::HSHAPtr _server;
    };  


}