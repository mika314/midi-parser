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
    auto write(std::ostream &) const -> void;
    std::vector<std::tuple<uint32_t, uint8_t, Event>> events;
  };
} // namespace midi
