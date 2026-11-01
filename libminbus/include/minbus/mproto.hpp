#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <cstddef>

namespace p { // minbus protocol namespace
  struct Meta {
    std::string wk_sbusn; // sender well-known name
    std::string wk_rbusn; // receiver well-known name
  };
  
  struct Header {
    char magic[6] = {'M', 'I', 'N', 'B', 'U', 'S'}; // MINBUS
    std::array<std::byte, 32> hash{}; // sha256 hash of the entire message/packet after the reserved slot
    uint32_t reserved[2] = {};
  };
  
  struct Message {
    Header hdr;
    Meta meta;
    std::vector<std::byte> payload;
  };
};
