#pragma once
#include "error.hpp"
#include "raw.hpp"
#include "type.hpp"

namespace midi
{
  enum class Vlq {};
  enum class Byte {};
  enum class Bytes {};

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
    auto readByte() -> unsigned char;
    auto readBytes(int len) -> std::string;
    auto readU16() -> uint16_t;
    auto readU32() -> uint32_t;
    auto readVlq() -> uint32_t;
  };
} // namespace midi
