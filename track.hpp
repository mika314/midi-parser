#pragma once
#include "chunk.hpp"
#include "event.hpp"
#include <tuple>
#include <vector>

namespace midi
{
  class Track : public Chunk
  {
  public:
    Track(Raw);
    std::vector<std::tuple<uint32_t, unsigned char, Event>> events;
  };
} // namespace midi
