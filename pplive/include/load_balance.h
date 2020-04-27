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

std::vector<std::shared_ptr<PPResourceNode>> DefaultLoadBalance::FetchNodes(
    std::vector<std::shared_ptr<PPResourceNode>>& servers) {
    std::sort(servers.begin(), servers.end(), cmp);
    std::vector<std::shared_ptr<PPResourceNode>> result;
    for (auto& server : servers) {
        if (result.size() < _select_num) {
            result.push_back(server);
        } else {
            return result;
        }
    }
    return result;
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

int DefaultServerSelect::pingServer(const std::string& ip){
    _context.reset();
    Pinger p(_context, ip.c_str());
    _context.run();
    return p.spend_time();
}

ServerInfoData
    DefaultServerSelect::FetchNode(std::vector<ServerInfoData>& servers) {
    ServerInfoData best_server;
    auto wait_time = max_wait;
    for (auto& server : servers) {
        auto cur_wait = pingServer(server.resource._host);
        if (cur_wait>0&&cur_wait < wait_time) {
            best_server = server;
        }
    }
    return best_server;
}
};  // namespace pplive