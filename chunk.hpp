#pragma once
#include "error.hpp"
#include "raw.hpp"
#include <iostream>

namespace midi
{
  class Chunk
  {
  public:
    Chunk(Raw = Raw{});

  private:
    Raw raw;
    size_t pos = 0;

  protected:
    auto eof() const -> bool;
    auto putBack() -> void;
    auto readBytes(int len) -> std::string;
    auto readU16() -> uint16_t;
    auto readU32() -> uint32_t;
    auto readU8() -> uint8_t;
    auto readVlq() -> uint32_t;
    static auto writeU16(std::ostream &, uint16_t) -> void;
    static auto writeU32(std::ostream &, uint32_t) -> void;
    static auto writeU8(std::ostream &, uint8_t) -> void;
    static auto writeVlq(std::ostream &, uint32_t) -> void;
  };
} // namespace midi
