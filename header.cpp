#include "header.hpp"
#include <log/log.hpp>

namespace midi
{
  Header::Header(Raw raw)
    : Chunk(raw), format(static_cast<Format>(readU16())), ntrks(readU16()), division([&]() -> decltype(division) {
        const auto d = readU16();
        if ((d & 0x8000) == 0)
        {
          // ticks per quarter-note
          return static_cast<int>(d);
        }
        else
        {
          // SMPTE
          return Smpte{static_cast<SmpteFormat>(-static_cast<char>((d & 0Xff00) >> 8)), d & 0X00ff};
        }
      }())
  {
  }

  auto toStr(Format v) -> std::string
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
