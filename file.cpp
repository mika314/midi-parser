#include "file.hpp"
#include <fstream>
#include <log/log.hpp>

namespace midi
{
  File::File(const std::string &fileName)
    : raw([&]() {
        std::ifstream t(fileName, std::ios::binary);
        std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        return str;
      }()),
      header([&]() {
        const auto chunkType = readU32();
        if (chunkType != 'MThd')
        {
          throw Error("Expected MThd");
        }
        const auto len = readU32();
        pos += len;
        return Raw{raw.begin() + pos - len, raw.begin() + pos};
      }())
  {
    for (auto i = 0; i < header.ntrks; ++i)
    {
      const auto chunkType = readU32();
      if (chunkType != 'MTrk')
      {
        LOG("track", i, " corrupted MIDI expected MTrk");
        break;
      }
      const auto len = readU32();
      if (len >= raw.size())
      {
        LOG("track", i, "corrupted MIDI len is too big", len);
        break;
      }
      pos += len;
      if (pos > static_cast<int>(raw.size()))
      {
        LOG("track", i, "corrupted MIDI len is too big", len);
        break;
      }
      tracks.emplace_back(Raw{raw.begin() + pos - len, raw.begin() + pos});
    }
  }

  auto File::readU32() -> uint32_t
  {
    auto ret = uint32_t{};
    for (auto i = 0U; i < sizeof(uint32_t); ++i)
    {
      if (pos >= static_cast<int>(raw.size()))
      {
        throw Error{"unexpected end of file"};
      }
      const auto b = static_cast<uint8_t>(raw[pos++]);
      ret = (ret << 8) | b;
    }
    return ret;
  }
} // namespace midi
