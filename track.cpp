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
          const auto type = readU8();
          const auto len = readVlq();
          const auto bytes = readBytes(len);
          events.emplace_back(deltaTime, 0xff, MetaEvent{static_cast<MetaEventType>(type), bytes});
          break;
        }
        }
        break;
      }
      }
    }
  }
} // namespace midi
