#pragma once
#include "chunk.hpp"
#include "event.hpp"
#include <tuple>
#include <vector>

namespace midi
{
  using TrackEvent = std::tuple<uint32_t, uint8_t, Event>;
  class Track : public Chunk
  {
  public:
    Track() = default;
    Track(Raw);
    auto write(std::ostream &) const -> void;
    std::vector<TrackEvent> events;
  };
  auto getDeltaTime(const TrackEvent &) -> uint32_t;
  auto getChannel(const TrackEvent &) -> uint8_t;
  auto getEvent(const TrackEvent &) -> Event;
} // namespace midi
