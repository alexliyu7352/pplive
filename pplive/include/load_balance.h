#include <boost/core/noncopyable.hpp>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <utility>

#include "node.h"

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
            std::vector<std::shared_ptr<PPResourceNode>>& nodes) {
        std::sort(nodes.begin(), nodes.end(), cmp);
        std::vector<std::shared_ptr<PPResourceNode>> result;
        for (auto& node : nodes) {
            if(result.size()<_select_num) {
                result.push_back(node);
            } else {
                return result;
            }
        }
        return result;
    };

    virtual DefaultLoadBalance* Clone() { return new DefaultLoadBalance(); };

   private:
    int _select_num;
};
};  // namespace pplive