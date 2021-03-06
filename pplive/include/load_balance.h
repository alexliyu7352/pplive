#pragma once
#include <boost/asio.hpp>
#include <boost/core/noncopyable.hpp>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <string>
#include <utility>

#include "node.h"
#include "pinger.h"
#include "proto.h"

namespace pplive {

class LoadBalanceABC : boost::noncopyable {
   public:
    LoadBalanceABC(){};
    /**
     * @brief 根据自定义算法筛选合理的节点
     * 
     * @param nodes 符合条件的服务器节点
     * @return std::vector<std::shared_ptr<PPResourceNode>> 
     */
    virtual std::vector<std::shared_ptr<PPResourceNode>> FetchNodes(
        std::vector<std::shared_ptr<PPResourceNode>>& nodes) = 0;
    /**
     * @brief 拷贝自身
     * 
     * @return LoadBalanceABC* 在堆上新建的LoadBalanceABC的指针 
     */
    virtual LoadBalanceABC* Clone() = 0;
};

class DefaultLoadBalance : public LoadBalanceABC {
   public:
    const static int kSelectNum = 10;

    DefaultLoadBalance() : _select_num(kSelectNum){};
    DefaultLoadBalance(int select_num) : _select_num(select_num){};

    static bool cmp(const std::shared_ptr<PPResourceNode>& l,
                    const std::shared_ptr<PPResourceNode>& r) {
        return l->_weight < r->_weight;
    }

    /**
     * @brief 根据weight筛选_select_num个服务器
     * 
     * @param servers 符合条件的服务器
     * @return std::vector<std::shared_ptr<PPResourceNode>> 
     */
    virtual std::vector<std::shared_ptr<PPResourceNode>> FetchNodes(
        std::vector<std::shared_ptr<PPResourceNode>>& servers);

    virtual DefaultLoadBalance* Clone() { return new DefaultLoadBalance(); };

   private:
    int _select_num; //初筛选择的服务器数量
};


class ServerSelectABC : boost::noncopyable {
   public:
    ServerSelectABC(){};
    /**
     * @brief 根据算法选出一个服务器
     * 
     * @param servers 从服务端返回的服务器列表
     * @return ServerInfoData 
     */
    virtual ServerInfoData FetchNode(std::vector<ServerInfoData>& servers) = 0;
    /**
     * @brief 拷贝自身
     *
     * @return ServerSelectABC* 在堆上新建的ServerSelectABC的指针
     */
    virtual ServerSelectABC* Clone() = 0;
};

class DefaultServerSelect : public ServerSelectABC {
   public:
    const static long int max_wait = std::numeric_limits<long int>::max();

    DefaultServerSelect(){};
    /**
     * @brief 根据ping得到的结果选择一个最低延迟的服务器
     *
     * @param servers 从服务端返回的服务器列表
     * @return ServerInfoData 最低延迟的服务器的信息
     */
    virtual ServerInfoData FetchNode(std::vector<ServerInfoData>& servers);
    virtual DefaultServerSelect* Clone() { return new DefaultServerSelect(); };

   private:
    /**
     * @brief 测试ip速度
     * 
     * @param ip 待测ip
     * @return int 延迟数值
     */
    int pingServer(const std::string& ip);
    boost::asio::io_context _context;
};

};  // namespace pplive