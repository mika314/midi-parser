#pragma once

namespace midi
{
  enum class MetaEventType : unsigned char {
    SequenceNumber = 0x00,
    TextEvent = 0x01,
    CopyrightNotice = 0x02,
    SequenceTrackName = 0x03,
    InstrumentName = 0x04,
    Lyric = 0x05,
    Marker = 0x06,
    CuePoint = 0x07,
    IChannelPrefix = 0x20,
    EndOfTrack = 0x2F,
    SetTempo = 0x51,
    SmpteOffset = 0x54
  };
  struct MetaEvent
  {
    MetaEventType type;
    std::string val;
  };
} // namespace midi
