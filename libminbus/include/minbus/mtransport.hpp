#pragma once

#include <zmq.hpp>
#include <minbus.hpp>
#include <cstddef>
#include <cstring>
#include <vector>

namespace t { // minbus transport namespace
    class Context {
        public:
            zmq::context_t ctx;
    };
    
    class LowLTransport {
        private:
            zmq::socket_t sock_;
        
        public:
            LowLTransport(Context& ctx, zmq::socket_type type): sock_(ctx.ctx, type) {}
            
            void llbind(const char* socket = "ipc:///var/run/minbus.sock") { sock_.bind(socket); }
            void llconnect(const char* socket = "ipc:///var/run/minbus.sock") { sock_.connect(socket); }
            
            void llclose() { sock_.close(); }
            
            void llsetid(const char* id) { sock_.set(zmq::sockopt::routing_id, id); }

            bool llsend(const void* data, size_t size) {
                if (!data) return false;
                
                zmq::message_t msg(size);
                std::memcpy(msg.data(), data, size);
                
                auto res = sock_.send(msg, zmq::send_flags::none);
                
                return res.has_value();
            }
            
            bool llrecv(std::vector<std::byte>& data) {
                zmq::message_t msg;
                auto result = sock_.recv(msg, zmq::recv_flags::none);
                
                if (!result.has_value()) return false;
                
                data.resize(msg.size());
                std::memcpy(data.data(), msg.data(), msg.size());
                
                return true;
            }
            
            bool llsendpart(const void* data, size_t size, zmq::send_flags flags = zmq::send_flags::none) {
                if (!data && size != 0) return false;
                
                zmq::message_t msg(size);
                if (size > 0) std::memcpy(msg.data(), data, size);
                
                auto res = sock_.send(msg, flags);
                return res.has_value();
            }
            
            bool llrecvpart(std::vector<std::byte>& data, bool& more, zmq::recv_flags flags = zmq::recv_flags::none) {
                zmq::message_t msg;
                auto res = sock_.recv(msg, flags);
                
                if (!res.has_value()) return false;
                
                data.resize(msg.size());
                if (msg.size() > 0) std::memcpy(data.data(), msg.data(), msg.size());
                
                more = sock_.get(zmq::sockopt::rcvmore);
                return true;
            }
    };
    
    class BrokerTransport {
        private:
            m::Serial ser_;
            Context lctx_;
            LowLTransport llt_;
        
        public:
            const char* socket_;
            
            explicit BrokerTransport(const char* socket = "ipc:///var/run/minbus.sock") : llt_(lctx_, zmq::socket_type::router) {
                llt_.llbind(socket);
                socket_ = socket;
            }
            
            ~BrokerTransport() {
                llt_.llclose();
                unlink(socket_);
            }
            
            bool send(const std::vector<std::byte>& routing_id, const p::Message& msg) {
                if (routing_id.empty()) return false;
                ser_.genpkt(msg);
                
                if (!llt_.llsendpart(routing_id.data(), routing_id.size(), zmq::send_flags::sndmore)) return false;
                if (!llt_.llsendpart(ser_.data(), ser_.size())) return false;

                return true;
            }
            
            bool recv(std::vector<std::byte>& routing_id, p::Message& msg) {
                p::Message temp;
                
                bool more = false;
                std::vector<std::byte> data;
                
                if (!llt_.llrecvpart(routing_id, more)) return false;
                if (!more) return false;
                
                if (!llt_.llrecvpart(data, more)) return false;
                if (more) return false;
                
                if (!ser_.degenpkt(reinterpret_cast<const char*>(data.data()), data.size(), temp)) return false;
                
                msg = std::move(temp);
                
                return true;
            }
    };

    class ClientTransport {
        private:
            m::Serial ser_;
            Context lctx_;
            LowLTransport llt_;

        public:
            explicit ClientTransport(const char* socket = "ipc:///var/run/minbus.sock") : llt_(lctx_, zmq::socket_type::dealer),socket_(socket) {
                llt_.llconnect(socket);
            }

            bool send(const p::Message& msg) {
                ser_.genpkt(msg);
                return llt_.llsendpart(ser_.data(), ser_.size());
            }

            bool recv(p::Message& msg) {
                std::vector<std::byte> data;
                if (!llt_.llrecv(data)) return false;

                p::Message temp;
                if (!ser_.degenpkt(
                        reinterpret_cast<const char*>(data.data()),
                        data.size(),
                        temp)) {
                    return false;
                }

                msg = std::move(temp);
                return true;
            }
    };
}
