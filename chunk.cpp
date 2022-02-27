#include "chunk.hpp"

namespace midi
{
  Chunk::Chunk(Raw raw) : raw(raw) {}

  auto Chunk::eof() const -> bool { return pos >= raw.size(); }
  auto Chunk::putBack() -> void { --pos; }
  auto Chunk::readBytes(int len) -> std::string
  {
    pos += len;
    if (pos > raw.size())
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

  auto Chunk::writeU16(std::ostream &st, uint16_t v) -> void
  {
    for (auto i = 0U; i < sizeof(uint16_t); ++i)
    {
      const auto ch = static_cast<char>((v >> ((sizeof(uint16_t) - i - 1) * 8)) & 0xff);
      st.write(&ch, 1);
    }
  }

  auto Chunk::writeU32(std::ostream &st, uint32_t v) -> void
  {
    for (auto i = 0U; i < sizeof(uint32_t); ++i)
    {
      const auto ch = static_cast<char>((v >> ((sizeof(uint32_t) - i - 1) * 8)) & 0xff);
      st.write(&ch, 1);
    }
  }

  auto Chunk::writeU8(std::ostream &st, uint8_t v) -> void
  {
    const auto ch = static_cast<char>(v);
    st.write(&ch, 1);
  }

  auto Chunk::writeVlq(std::ostream &st, uint32_t v) -> void
  {
    auto ret = std::string{};
    while (v != 0)
    {
      ret = static_cast<char>(v & 0x7fU) + ret;
      v >>= 7;
    }
    if (ret.empty())
    {
      const auto ch = char{0};
      st.write(&ch, 1);
      return;
    }
    for (auto i = 0U; i < ret.size() - 1; ++i)
      ret[i] |= 0x80;
    st << ret;
  }
} // namespace midi
