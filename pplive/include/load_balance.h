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
    virtual std::vector<std::shared_ptr<PPResourceNode>> FetchNodes(
        std::vector<std::shared_ptr<PPResourceNode>>& nodes) = 0;
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

    virtual std::vector<std::shared_ptr<PPResourceNode>> FetchNodes(
        std::vector<std::shared_ptr<PPResourceNode>>& servers);

    virtual DefaultLoadBalance* Clone() { return new DefaultLoadBalance(); };

   private:
    int _select_num;
};


class ServerSelectABC : boost::noncopyable {
   public:
    ServerSelectABC(){};
    virtual ServerInfoData FetchNode(std::vector<ServerInfoData>& servers) = 0;
    virtual ServerSelectABC* Clone() = 0;
};

class DefaultServerSelect : public ServerSelectABC {
   public:
    const static long int max_wait = std::numeric_limits<long int>::max();

    DefaultServerSelect(){};
    virtual ServerInfoData FetchNode(std::vector<ServerInfoData>& servers);
    virtual DefaultServerSelect* Clone() { return new DefaultServerSelect(); };

   private:
    int pingServer(const std::string& ip);
    boost::asio::io_context _context;
};

};  // namespace pplive