#include <cstring>
#include <iostream>
#include <minbus.hpp>

using namespace std;

int main() {
  t::BrokerTransport trs;

  std::vector<std::byte> routing_id;
  p::Message msg;

  while (true) {
    routing_id.clear();
    msg.meta.wk_rbusn.clear();
    msg.meta.wk_sbusn.clear();
    msg.payload.clear();
    
    trs.recv(routing_id, msg);

    cout << "sender: " << msg.meta.wk_sbusn << " recv: " << msg.meta.wk_rbusn << " payload: " << std::string(reinterpret_cast<const char*>(msg.payload.data()), msg.payload.size()) << endl;
  
    msg.meta.wk_sbusn.swap(msg.meta.wk_rbusn);

    trs.send(routing_id, msg);
  }
  
  return -1;
}
