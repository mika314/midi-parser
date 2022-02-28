#pragma once
#include "chunk.hpp"
#include "error.hpp"
#include <string_view>
#include <variant>

namespace midi
{
  enum class Format : uint16_t { zero = 0, one = 1, two = 2 };

  using Ticks = uint16_t;
  enum class SmpteFormat : uint8_t { f24 = 24, f25 = 25, f29 = 29, f30 = 30 };

  struct Smpte
  {
    SmpteFormat fmt;
    Ticks tpf; // ticks per frame
  };

  class Header : public Chunk
  {
  public:
    Header() = default;
    Header(Raw);
    auto write(std::ostream &) const -> void;

  public:
    Format format;
    size_t ntrks;
    std::variant<Ticks, Smpte> division;
  };
  auto toStr(Format) -> std::string;
  auto toStr(Smpte) -> std::string;
  auto toStr(SmpteFormat) -> std::string;
  auto toStr(std::variant<Ticks, Smpte>) -> std::string;
} // namespace midi
