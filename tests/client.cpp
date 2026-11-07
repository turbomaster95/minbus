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

    m::Serial ser;
    ser.genpkt(msg);

    lts.llconnect();

    lts.llsend(ser.data(), ser.size());

    std::vector<std::byte> data;

    lts.llrecv(data);

    p::Message out;

    ser.degenpkt(reinterpret_cast<const char*>(data.data()), data.size(), out);

    std::cout << "sender: " << out.meta.wk_sbusn << " recv: " << out.meta.wk_rbusn << " payload: " << std::string(reinterpret_cast<const char*>(out.payload.data()), out.payload.size()) << std::endl;

    return 0;
}
