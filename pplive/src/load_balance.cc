/*
 * @Author: your name
 * @Date: 2020-04-27 22:44:26
 * @LastEditTime: 2020-04-27 22:45:34
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /pplive/pplive/src/load_balance.cpp
 */
#include <load_balance.h>

namespace pplive {

int DefaultServerSelect::pingServer(const std::string& ip){
    _context.reset();
    Pinger p(_context, ip.c_str());
    _context.run();
    return p.spend_time();
}

ServerInfoData DefaultServerSelect::FetchNode(std::vector<ServerInfoData>& servers) {
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

}
