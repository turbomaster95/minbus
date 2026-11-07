#pragma once

#include <minbus.hpp>

namespace m {
    class Router {
        private:
            std::unordered_map<std::string, std::vector<std::byte>> routes_;
        
        public:
            Router() : routes_{} {}

            bool newroute(const std::string& wk_id, const std::vector<std::byte>& routing_id) {
                auto [it, inserted] = routes_.insert({wk_id, routing_id});
                return inserted;
            }

            // find by wk_id
            std::optional<std::vector<std::byte>> findroute(const std::string& wk_id) const {
                auto it = routes_.find(wk_id);
                return (it != routes_.end()) ? std::make_optional(it->second) : std::nullopt;
            }

            // find by routing_id
            std::optional<std::string> findroute(const std::vector<std::byte>& routing_id) const {
                for (const auto& [wk_id, stored_routing] : routes_) {
                    if (stored_routing == routing_id) {
                        return std::make_optional(wk_id);
                    }
                }
                return std::nullopt;
            }

            bool deleteroute(const std::string& wk_id) {
                return routes_.erase(wk_id) > 0;
            }
    };
}
