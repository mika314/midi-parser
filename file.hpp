#pragma once
#include "error.hpp"
#include "header.hpp"
#include "track.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace midi
{
  class File
  {
  public:
    File() = default;
    File(std::istream &);
    auto write(std::ostream &) const -> void;

  private:
    std::string raw;
    size_t pos = 0;

  public:
    Header header;
    std::vector<Track> tracks;

  private:
    auto readU32() -> uint32_t;
  };
} // namespace midi
