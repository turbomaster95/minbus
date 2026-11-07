#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <cstddef>
#include <msgpack.hpp>
#include <exception>
#include <minbusd.hpp>
#include <utility>

namespace md {
  enum class Actions {
    ACK = 0x00,
    HEAL,
    REG,
    UREG
  };

  struct Action {
    std::uint32_t act_id;
    Actions action;
  };

  class Serial {
    private:
      msgpack::sbuffer sbuf_;

    public:
      void genpkt(const Action& amsg) {
        sbuf_.clear();

        msgpack::packer<msgpack::sbuffer> pk(&sbuf_);

        pk.pack_map(2);

        pk.pack("act_id");
        pk.pack_uint32(amsg.act_id);

        pk.pack("action");
        pk.pack(amsg.action);
      }

      size_t size() { return sbuf_.size(); }
      const char* data() { return sbuf_.data(); }

      bool degenpkt(const char* data, size_t size, Action& out) {
        if (!data || size == 0) return false;

        Action tmp;

        try {
          msgpack::object_handle oh = msgpack::unpack(data, size);
          msgpack::object obj = oh.get();

          if (obj.type != msgpack::type::MAP) return false;

          bool actid_found = false;
          bool action_found = false;

          for (size_t i = 0; i < obj.via.map.size; i++) {
              auto& key_obj = obj.via.map.ptr[i].key;
              auto& val_obj = obj.via.map.ptr[i].val;

              if (key_obj.type != msgpack::type::STR) return false;

              std::string key = key_obj.as<std::string>();

              if (key == "act_id") {
                if (actid_found) return false;
                if (val_obj.type != msgpack::type::POSITIVE_INTEGER) return false;

                tmp.act_id = val_obj.as<uint32_t>();
                actid_found = true;
              }

              if (key == "action") {
                if (action_found) return false;
                if (val_obj.type != msgpack::type::POSITIVE_INTEGER) return false;

                bool wrong_action = false;
                
                switch (static_cast<Actions>(val_obj.as<uint8_t>())) {
                  case Actions::ACK:
                    break;
                  case Actions::HEAL:
                    break;
                  case Actions::REG:
                    break;
                  case Actions::UREG:
                    break;
                  default:
                    std::cerr << "Unknown Action type Found!" << std::endl;
                    wrong_action = true;    
                }

                if (wrong_action) {
                  return false;
                } else {
                  tmp.action = static_cast<Actions>(val_obj.as<uint8_t>());
                }
                
                action_found = true;
              }
          }

          if (actid_found == false) return false;
          if (action_found == false) return false;
          
        } catch(const std::exception& Exception) {
          std::cerr << "Action Message Decoding Exception:" << Exception.what() << std::endl;
          return false;
        };

        out = std::move(tmp);
        return true;
      }
  };
}
