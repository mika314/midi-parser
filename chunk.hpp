#pragma once
#include "error.hpp"
#include "raw.hpp"
#include "type.hpp"

namespace midi
{
  class Chunk
  {
  public:
    Chunk(Raw);

  private:
    Raw raw;
    int pos = 0;

  protected:
    auto eof() const -> bool;
    auto putBack() -> void;
    auto readBytes(int len) -> std::string;
    auto readU16() -> uint16_t;
    auto readU32() -> uint32_t;
    auto readU8() -> uint8_t;
    auto readVlq() -> uint32_t;
  };
} // namespace midi
