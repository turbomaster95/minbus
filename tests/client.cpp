#include <iostream>
#include <minbus.hpp>
#include <zmq.hpp>

int main() {
    p::Message msg;

    msg.meta.wk_sbusn = "dev.deva.client1";
    msg.meta.wk_rbusn = "dev.deva.minbuscore";
    std::string hello = "Hello!!";
    msg.payload.assign(
        reinterpret_cast<const std::byte*>(hello.data()),
        reinterpret_cast<const std::byte*>(hello.data() + hello.size())
    );

    t::ClientTransport cts;    

    cts.send(msg);
    
    p::Message out;

    cts.recv(out);

    std::cout << "sender: " << out.meta.wk_sbusn << " recv: " << out.meta.wk_rbusn << " payload: " << std::string(reinterpret_cast<const char*>(out.payload.data()), out.payload.size()) << std::endl;

    return 0;
}
