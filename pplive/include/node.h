#include "defs.h"

namespace pplive {
    class PPResourceNode : public boost::noncopyable {
        public:
            std::string _node_id;
            uint32_t _weight;
            std::string _resource_id;
            handy::TcpConnPtr _data_conn;
            std::shared_ptr<PPResourceNode> _parent_node;
            std::string _data_host;
            std::string _data_port;
        private:
            PPResourceNode(const std::string & node_id,const std::string& resource_id, 
             uint32_t weight, handy::TcpConnPtr data_conn, std::shared_ptr<PPResourceNode> parent_node) : _node_id(node_id), _resource_id(resource_id), _weight(weight), _parent_node(parent_node) {};

            static std::shared_ptr<PPResourceNode> CreatePPNode(const std::string & node_id,const std::string& resource_id, 
             uint32_t weight, handy::TcpConnPtr data_conn, std::shared_ptr<PPResourceNode> parent_node){
                 return std::make_shared<PPResourceNode>(node_id, resource_id, weight, data_conn, parent_node);
             }
    };
}