#pragma once
#include "error.hpp"
#include "header.hpp"
#include "track.hpp"
#include <string>
#include <vector>

namespace midi
{
  class File
  {
  public:
    File(const std::string &fileName);

  private:
    std::string raw;
    int pos = 0;

  public:
    Header header;
    std::vector<Track> tracks;

  private:
    auto readU32() -> uint32_t;
  };
} // namespace midi
