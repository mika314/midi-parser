#include "chunk.hpp"

namespace midi
{
  Chunk::Chunk(Raw raw) : raw(raw) {}

  auto Chunk::eof() const -> bool { return pos >= static_cast<int>(raw.size()); }
  auto Chunk::putBack() -> void { --pos; }
  auto Chunk::readBytes(int len) -> std::string
  {
    pos += len;
    if (pos > static_cast<int>(raw.size()))
      throw Error{"unexpected end of file"};
    return std::string{raw.data() + pos - len, static_cast<size_t>(len)};
  }

  auto Chunk::readVlq() -> uint32_t
  {
    auto ret = uint32_t{0};
    for (auto i = 0U; i < sizeof(uint32_t); ++i)
    {
      const auto b = readU8();
      ret = (ret << 7) | (b & 0x7fU);
      if ((b & 0x80) == 0)
        break;
    }
    return ret;
  }

  auto Chunk::readU8() -> uint8_t
  {
    if (eof())
      throw Error{"unexpected end of file"};
    return static_cast<uint8_t>(raw[pos++]);
  }

  auto Chunk::readU32() -> uint32_t
  {
    auto ret = uint32_t{};
    for (auto i = 0U; i < sizeof(uint32_t); ++i)
    {
      const auto b = readU8();
      ret = (ret << 8) | b;
    }
    return ret;
  }

  auto Chunk::readU16() -> uint16_t
  {
    auto ret = uint16_t{};
    for (auto i = 0U; i < sizeof(uint16_t); ++i)
    {
      const auto b = readU8();
      ret = (ret << 8) | b;
    }
    return ret;
  }

} // namespace midi
