#include "defs.h"

namespace pplive {
    class PPNode : public boost::noncopyable {
        public:
            std::string _node_id;
            uint32_t _weight;
            std::string _resource_id;
            handy::TcpConnPtr _data_conn;

            std::shared_ptr<PPNode> _parent_node;

            
    }
}