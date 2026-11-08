#include <cstring>
#include <iostream>
#include <minbus.hpp>
#include <minbusd.hpp>
#include <mcommon.hpp>
#include <vector>

using namespace std;

int main() {
  if (setup_signals() < 0) {
        std::cerr << "Failed to setup signal handlers!" << std::endl;
        return 1;
  }

  t::BrokerTransport trs;

  std::vector<std::byte> routing_id;
  p::Message msg;
  
  while (g_running) {
    routing_id.clear();
    msg.meta.wk_rbusn.clear();
    msg.meta.wk_sbusn.clear();
    msg.payload.clear();

    try {
      trs.recv(routing_id, msg);
    } catch (const zmq::error_t& e) {
      if (!g_running) {
        break;
      }
      std::cerr << "ZMQ error: " << e.what() << std::endl;
    };
    
    md::Action act;

    cout << "sender: " << msg.meta.wk_sbusn << " recv: " << msg.meta.wk_rbusn << " payload: " << std::string(reinterpret_cast<const char*>(msg.payload.data()), msg.payload.size()) << endl;

    msg.meta.wk_sbusn.swap(msg.meta.wk_rbusn);

    trs.send(routing_id, msg);
  }

  return -1;
}

