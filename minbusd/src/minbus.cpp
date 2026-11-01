#include <cstring>
#include <iostream>
#include <minbus.hpp>

using namespace std;

int main(int argc, char* argv[]) {
  p::Message msg;
  msg.meta.wk_sbusn = "deva";
  msg.meta.wk_rbusn = "example.service";

  msg.payload.resize(6);

  memcpy(msg.payload.data(), "hello", 6);

  m::Serial ser;

  ser.genpkt(msg);

  cout << "generated msg!" << endl;

  p::Message dmsg;
  ser.degenpkt(ser.data(), ser.size(), dmsg);

  cout << dmsg.meta.wk_rbusn << endl;
}
