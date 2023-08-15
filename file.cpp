#include "file.hpp"

namespace midi
{
  File::File(std::istream &is)
    : raw((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>()), header([&]() {
        const auto chunkType = readU32();
        if (chunkType != 'MThd')
        {
          throw Error("Expected MThd");
        }
        const auto len = readU32();
        pos += len;
        return Raw{raw.data() + pos - len, len};
      }())
  {
    for (auto i = 0U; i < header.ntrks; ++i)
    {
      const auto chunkType = readU32();
      if (chunkType != 'MTrk')
      {
        break;
      }
      const auto len = readU32();
      if (len >= raw.size())
      {
        break;
      }
      pos += len;
      if (pos > raw.size())
      {
        break;
      }
      tracks.emplace_back(Raw{raw.data() + pos - len, len});
    }
    raw.clear();
  }

  auto File::readU32() -> uint32_t
  {
    auto ret = uint32_t{};
    for (auto i = 0U; i < sizeof(uint32_t); ++i)
    {
      if (pos >= raw.size())
      {
        throw Error{"unexpected end of file"};
      }
      const auto b = static_cast<uint8_t>(raw[pos++]);
      ret = (ret << 8) | b;
    }
    return ret;
  }

  auto File::write(std::ostream &st) const -> void
  {
    auto h = header;
    h.ntrks = tracks.size();
    h.write(st);
    for (const auto &track : tracks)
      track.write(st);
  }
} // namespace midi
