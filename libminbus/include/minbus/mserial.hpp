#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <cstddef>
#include <msgpack.hpp>
#include <exception>
#include <minbus/mproto.hpp>
#include <utility>

namespace m { // minbus namespace
  class Serial {
    private:
      msgpack::sbuffer sbuf_;

    public:
      void genpkt(const p::Message& msg) {
          sbuf_.clear();

          msgpack::packer<msgpack::sbuffer> pk(&sbuf_);

          pk.pack_map(4);

          pk.pack("magic");
          pk.pack("MINBUS");

          pk.pack("sender");
          pk.pack(msg.meta.wk_sbusn);

          pk.pack("receiver");
          pk.pack(msg.meta.wk_rbusn);

          pk.pack("payload");
          pk.pack_bin(static_cast<uint32_t>(msg.payload.size()));
          pk.pack_bin_body(reinterpret_cast<const char*>(msg.payload.data()), static_cast<uint32_t>(msg.payload.size()));
      }

      size_t size() { return sbuf_.size(); }
      const char* data() { return sbuf_.data(); }

      bool degenpkt(const char* data, std::size_t size, p::Message& out) {
        if (!data || size == 0) return false;

        p::Message temp;
        try {
            msgpack::object_handle oh = msgpack::unpack(data, size);
            msgpack::object obj = oh.get();
            if (obj.type != msgpack::type::MAP) return false;

            bool magic_found = false;
            bool sender_found = false;
            bool recv_found = false;
            bool payl_found = false;

            for (size_t i = 0; i < obj.via.map.size; i++) {
              if (obj.via.map.ptr[i].key.type == msgpack::type::STR) {
                if (obj.via.map.ptr[i].key.as<std::string>() == "magic") {
                  if (magic_found) return false;
                  if (obj.via.map.ptr[i].val.type == msgpack::type::STR) {
                    if (obj.via.map.ptr[i].val.as<std::string>() == "MINBUS") {
                      magic_found = true;
                    } else {
                      std::cerr << "Magic Header is bad!" << std::endl;
                      magic_found = false;
                      return false;
                    }
                  } else {
                    std::cerr << "magic key has invalid type!!" << std::endl;
                    magic_found = false;
                    return false;
                  }
                }

                if (obj.via.map.ptr[i].key.as<std::string>() == "sender") {
                  if (sender_found) return false;
                  if (obj.via.map.ptr[i].val.type == msgpack::type::STR) {
                    temp.meta.wk_sbusn = obj.via.map.ptr[i].val.as<std::string>();
                    sender_found = true;
                  } else {
                    sender_found = false;
                    return false;
                  }
                }

                if (obj.via.map.ptr[i].key.as<std::string>() == "receiver") {
                  if (recv_found) return false;
                  if (obj.via.map.ptr[i].val.type == msgpack::type::STR) {
                    temp.meta.wk_rbusn = obj.via.map.ptr[i].val.as<std::string>();
                    recv_found = true;
                  } else {
                    recv_found = false;
                    return false;
                  }
                }

                if (obj.via.map.ptr[i].key.as<std::string>() == "payload") {
                  if (payl_found) return false;
                  if (obj.via.map.ptr[i].val.type == msgpack::type::BIN) {
                    temp.payload.resize(obj.via.map.ptr[i].val.via.bin.size);
                    std::memcpy(temp.payload.data(), obj.via.map.ptr[i].val.via.bin.ptr, obj.via.map.ptr[i].val.via.bin.size);
                    payl_found = true;
                  } else {
                    payl_found = false;
                    return false;
                  }
                }
              } else {
                continue;
              }
            }

            if (magic_found == false) return false;
            if (sender_found == false) return false;
            if (recv_found == false) return false;
            if (payl_found == false) return false;

        } catch (const std::exception& e){
            std::cout << "MsgPack Exception:" << e.what() << std::endl;
            return false;
        }

        out = std::move(temp);
        return true;
      }
  };
};
