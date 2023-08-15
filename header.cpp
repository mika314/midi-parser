#include "header.hpp"
#include <sstream>

namespace midi
{
  Header::Header(Raw raw)
    : Chunk(raw), format(static_cast<Format>(readU16())), ntrks(readU16()), division([&]() -> decltype(division) {
        const auto d = readU16();
        if ((d & 0x8000) == 0)
        {
          // ticks per quarter-note
          return d;
        }
        else
        {
          // SMPTE
          return Smpte{static_cast<SmpteFormat>(-static_cast<char>((d & 0xff00) >> 8)), static_cast<Ticks>(d & 0x00ff)};
        }
      }())
  {
  }

  auto Header::write(std::ostream &st) const -> void
  {
    std::ostringstream ss;
    writeU16(ss, static_cast<uint16_t>(format));
    writeU16(ss, ntrks);
    if (std::holds_alternative<Ticks>(division))
      writeU16(ss, std::get<Ticks>(division));
    else
      writeU16(ss,
               uint16_t{0x8000} | ((-static_cast<int8_t>(std::get<Smpte>(division).fmt)) << 8) | static_cast<int8_t>(std::get<Smpte>(division).tpf));
    writeU32(st, 'MThd');
    writeU32(st, ss.str().size());
    st << ss.str();
  }

  auto toStr(Format v) -> const char *
  {
    switch (v)
    {
    case Format::zero: return "format 0";
    case Format::one: return "format 1";
    case Format::two: return "format 2";
    }
  }
  auto toStr(Smpte v) -> std::string { return toStr(v.fmt) + ":" + std::to_string(v.tpf); }
  auto toStr(SmpteFormat v) -> std::string { return std::to_string(static_cast<int>(v)); }
  auto toStr(std::variant<Ticks, Smpte> v) -> std::string
  {
    if (std::holds_alternative<Ticks>(v))
      return "ticks: " + std::to_string(std::get<Ticks>(v));
    else
      return "SMPTE: " + toStr(std::get<Smpte>(v));
  }
} // namespace midi
