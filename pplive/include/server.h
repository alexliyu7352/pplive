#pragma once
#include "handy/handy.h"
#include <string>
#include <atomic>
#include <vector>
#include "defs.h"
#include "proto.h"
#include "node.h"

namespace pplive {
    class PPToplyInfo :public boost::noncopyable {

        // 拓扑信息
        // 描述某个资源的拓扑关系
        public:
            inline static const uint32_t MAX_PICK_NUM = 100;
        public:
            PPToplyInfo(const std::string & resource_id): _resource_id(resource_id) {};

        public:
            /**
             * @brief 选取最佳的节点
             * 
             * @param node_id 
             * @return std::vector<std::shared_ptr<PPResourceNode>> 
             */
            std::vector<std::shared_ptr<PPResourceNode>> PickBestNode(const std::string & node_id); // 选取最佳链接

            /**
             * @brief 更新节点
             * 
             * @param node_id 
             * @param host 
             * @param port 
             * @param weight 
             * @return int 
             */
            int UpdateToply(const std::string& node_id, std::shared_ptr<PPResourceNode> parent_node, const std::string uri,  u_int32_t weight); // 更新拓扑信息

            /**
             * @brief 删除节点
             * 
             * @param node_id 
             * @return int 
             */
            int RemoteToply(const std::string& node_id); // 删除拓扑节点

            std::shared_ptr<PPResourceNode> FindNode(const std::string & node_id);
        private:
            std::string _resource_id; // 资源 id
            std::map<std::string, std::shared_ptr<PPResourceNode>> _node_map; // node_id, node 数据库
    };

    class PPControllServer : public boost::noncopyable {


        /**
         * @brief 控制服务器, 最中心的节点, 用于同步整个拓扑网络的状态
         * 
         */
        
        public: 
            std::atomic<uint64_t> node_id;
        public:
            PPControllServer(const std::string & host, unsigned short port);
            /**
             * @brief 运行服务器
             * 
             * @param threading  是否在单独的线程中
             */
            void Run(bool threading = false);
        public:

            /**
             * @brief 创建一个数据 指定运行的端口之类的信息, 会插入到 _data_server_maps 中
             * 
             * @param resource_id 
             * @param host 
             * @param port 
             * @return int 
             */
            int CreateData(const std::string & resource_id) ;

            /**
             * @brief 想一个资源写入
             * 
             * @param resource_id 
             * @param data 
             * @return int 
             */
            /**
             * @brief 关闭一个数据
             * 
             * @param resource_id 
             * @return int 
             */
            int CloseData(const std::string & resource_id);
            
            /**
             * @brief 注册数据
             * 
             * @param node_id 
             * @param parent_node_id 
             * @param host 
             * @param port 
             * @param weight 
             * @return int 
             */
            int RegistData(const std::string& node_id, const std::string & resource_id, const std::string& uri, uint32_t weight);

        public:

            /**
             * @brief 生成一个节点 id
             * 
             * @param str 
             * @return char* 
             */
            char * genNodeId( char * str);
        public:
            handy::HSHAPtr _server; 
            std::unique_ptr<handy::EventBase> _loop;
            std::map<std::string, std::unique_ptr<PPToplyInfo>> _toply_map; // resource id, 
        private:
            /**
             * @brief 处理 链接请求
             * 
             * @param conn 
             * @param msg 
             * @return int 
             */
            int handleMsgConnect(const handy::TcpConnPtr& conn,  BaseMsg & msg);

            /**
             * @brief 处理 资源请求
             * 
             * @param conn 
             * @param msg 
             * @return int 
             */
            int handleMsgFetch(const handy::TcpConnPtr& conn,  BaseMsg & msg);
            
            /**
             * @brief 处理 ping
             * 
             * @param conn 
             * @return int 
             */
            int handleMsgPing(const handy::TcpConnPtr& conn);

            /**
             * @brief 处理拓扑信息
             * 
             * @param conn 
             * @param msg 
             * @return int 
             */
            int handleMsgToplySync(const handy::TcpConnPtr& conn,  BaseMsg & msg);
            
            /**
             * @brief 处理断开
             * 
             * @param conn 
             * @param msg 
             * @return int 
             */
            int handleDisConn(const handy::TcpConnPtr& conn,  BaseMsg & msg);
    };



}   