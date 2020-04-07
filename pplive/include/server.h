#include "handy/handy.h"
#include <string>
#include <atomic>
#include <vector>
#include "defs.h"
#include "data.h"
#include "proto.h"
#include "node.h"

namespace pplive {
    class PPToplyInfo :public boost::noncopyable {
        public:
            std::vector<std::shared_ptr<PPResourceNode>> PickBestNode(const std::string & node_id);
            int UpdateToply(const std::string& node_id, const std::string & host, unsigned int port,  u_int32_t weight);
            int RemoteToply(const std::string& node_id);
        private:
            std::string _resource_id;
            std::map<std::string, std::shared_ptr<PPResourceNode>> _node_map; // node_id, node 数据库
    };

    class PPControllServer : public boost::noncopyable {
        public:
            std::atomic<uint64_t> node_id;
        public:
            PPControllServer(const std::string & host, unsigned short port);
        public:
            int CreateData(const std::string & resource_id, const std::string & host, unsigned int port);
            int WriteData(const std::string & resource_id, const char * data );
            int CloseData(const std::string & resource_id);

        public:
            char * genNodeId( char * str);
        public:
            handy::HSHAPtr _server; 
            std::map<std::string,handy::TcpConnPtr> _conn_maps;
            std::map<std::string, std::shared_ptr<PPDataServer>> _data_server_maps;
            std::unique_ptr<handy::EventBase> _loop;
            std::map<std::string, PPToplyInfo> _toply_map; // resource id, 
        private:
            int handleMsgConnect(const handy::TcpConnPtr& conn, const ConnectReqMsg& msg);
            int handleMsgFetch(const handy::TcpConnPtr& conn, const FetchReqMsg& msg);
    };



}   