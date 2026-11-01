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
    };

    class BrokerTransport {
        private:
            m::Serial ser_;
            Context lctx_;
            LowLTransport llt_;

        public:
            BrokerTransport(const char* socket = "ipc:///var/run/minbus.sock") : llt_(lctx_, zmq::socket_type::router) {
                llt_.llbind(socket);
            }

            ~BrokerTransport() {
                llt_.llclose();
            }

            bool send(const p::Message& msg) {
                ser_.genpkt(msg);
                
                llt_.llsend(ser_.data(), ser_.size());

                return true;
            }

            bool recv(p::Message& msg) {
                p::Message temp;

                std::vector<std::byte> data;

                llt_.llrecv(data);

                ser_.degenpkt(reinterpret_cast<const char*>(data.data()), data.size(), temp);

                msg = std::move(temp);

                return true;
            }
    };
}