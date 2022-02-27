#include "track.hpp"
#include <log/log.hpp>

namespace midi
{
  Track::Track(Raw raw) : Chunk(raw)
  {
    uint8_t lastStatusByte = 0;
    while (!eof())
    {
      const auto deltaTime = readVlq();
      const auto b = readU8();

      if ((b & 0x80) == 0)
      {
        // data byte
        putBack();
      }
      else
      {
        // status byte
        lastStatusByte = b;
      }
      const auto channel = static_cast<uint8_t>(lastStatusByte & 0x0f);
      switch (lastStatusByte & 0xf0)
      {
      case 0x80: {
        const auto n = readU8();
        const auto v = readU8();
        events.emplace_back(deltaTime, channel, NoteOff{n, v});
        break;
      }
      case 0x90: {
        const auto n = readU8();
        const auto v = readU8();
        events.emplace_back(deltaTime, channel, NoteOn{n, v});
        break;
      }
      case 0xa0: {
        const auto n = readU8();
        const auto v = readU8();
        events.emplace_back(deltaTime, channel, PolyKeyPressure{n, v});
        break;
      }
      case 0xb0: {
        const auto n = readU8();
        const auto v = readU8();
        events.emplace_back(deltaTime, channel, ControlChange{static_cast<Control>(n), v});
        break;
      }
      case 0xc0: {
        events.emplace_back(deltaTime, channel, static_cast<ProgramChange>(readU8()));
        break;
      }
      case 0xd0: {
        events.emplace_back(deltaTime, channel, static_cast<ChannelPressure>(readU8()));
        break;
      }
      case 0xe0: {
        const auto b1 = readU8();
        const auto b2 = readU8();
        events.emplace_back(deltaTime, channel, PitchBend{(((b2 << 7) | b1) - 0x2000) / (1.f * 0x2000)});
        break;
      }
      case 0xf0: {
        switch (lastStatusByte)
        {
        case 0xf0: {
          const auto len = readVlq();
          events.emplace_back(deltaTime, 0xff, SysEx{readBytes(len)});
          break;
        }
        case 0xf7: {
          const auto len = readVlq();
          events.emplace_back(deltaTime, 0xff, SysExF7{readBytes(len)});
          break;
        }
        case 0xff: {
          const auto type = static_cast<MetaEventType>(readU8());
          const auto len = readVlq();
          const auto bytes = readBytes(len);
          switch (type)
          {
          case MetaEventType::TextEvent: events.emplace_back(deltaTime, 0xff, MetaEvent{type, bytes}); break;
          case MetaEventType::CopyrightNotice: events.emplace_back(deltaTime, 0xff, MetaEvent{type, bytes}); break;
          case MetaEventType::SequenceTrackName: events.emplace_back(deltaTime, 0xff, MetaEvent{type, bytes}); break;
          case MetaEventType::InstrumentName: events.emplace_back(deltaTime, 0xff, MetaEvent{type, bytes}); break;
          case MetaEventType::Lyric: events.emplace_back(deltaTime, 0xff, MetaEvent{type, bytes}); break;
          case MetaEventType::Marker: events.emplace_back(deltaTime, 0xff, MetaEvent{type, bytes}); break;
          case MetaEventType::CuePoint: events.emplace_back(deltaTime, 0xff, MetaEvent{type, bytes}); break;
          case MetaEventType::MidiChannelPrefix:
            if (len != 1)
              throw Error("Expected one byte for MidiChannelPrefix");
            events.emplace_back(deltaTime, 0xff, static_cast<MidiChannelPrefix>(bytes[0]));
            break;
          case MetaEventType::EndOfTrack:
            if (len != 0)
              throw Error("Expected zero bytes for EndOfTrack");
            events.emplace_back(deltaTime, 0xff, EndOfTrack{});
            break;
          case MetaEventType::SetTempo: {
            if (len != 3)
              throw Error("Expected 3 bytes for SetTempo");
            const auto tempo =
              static_cast<unsigned char>(bytes[0]) << 16 | static_cast<unsigned char>(bytes[1]) << 8 | static_cast<unsigned char>(bytes[2]);
            events.emplace_back(deltaTime, 0xff, static_cast<SetTempo>(tempo));
            break;
          }
          case MetaEventType::SmpteOffset: {
            if (len == 5)
              events.emplace_back(deltaTime,
                                  0xff,
                                  SmpteOffset{static_cast<uint8_t>(bytes[0]),
                                              static_cast<uint8_t>(bytes[1]),
                                              static_cast<uint8_t>(bytes[2]),
                                              static_cast<uint8_t>(bytes[3]),
                                              static_cast<uint8_t>(bytes[4])});
            else
              LOG("Expected 5 bytes for SmpteOffset but have got", len);
            break;
          }
          case MetaEventType::TimeSignature: {
            if (len != 4)
              throw Error("Expected 4 bytes for TimeSignature");
            events.emplace_back(
              deltaTime,
              0xff,
              TimeSignature{
                static_cast<uint8_t>(bytes[0]), static_cast<uint8_t>(bytes[1]), static_cast<uint8_t>(bytes[2]), static_cast<uint8_t>(bytes[3])});
            break;
          }
          case MetaEventType::KeySignature: {
            if (len != 2)
              throw Error("Expected 2 bytes for KeySignature");
            events.emplace_back(deltaTime, 0xff, KeySignature{static_cast<int8_t>(bytes[0]), static_cast<bool>(bytes[1])});
            break;
          }
          case MetaEventType::SequencerSpecificMetaEvent: events.emplace_back(deltaTime, 0xff, MetaEvent{type, bytes}); break;
          default: events.emplace_back(deltaTime, 0xff, MetaEvent{type, bytes}); break;
          }
          break;
        }
        }
        break;
      }
      }
    }
  }
} // namespace midi
