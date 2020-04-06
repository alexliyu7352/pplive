#include "handy/handy.h"
#include <string>
#include <atomic>
#include <vector>
#include "defs.h"
#include "data.h"
#include "proto.h"

namespace pplive {
    class PPToplyInfo {
        public:
            std::vector<std::string> PickBestNode();
            int UpdateToply(const std::string& node_id, const std::string& resource_id, const std::string & host, unsigned int port,  u_int32_t weight);
            int RemoteToply(const std::string& node_id);

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

        private:
            int handleMsgConnect(const handy::TcpConnPtr& conn, const ConnectReqMsg& msg);
            int handleMsgFetch(const handy::TcpConnPtr& conn, const FetchReqMsg& msg);
    };



}   