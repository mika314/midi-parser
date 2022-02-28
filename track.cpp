#include "track.hpp"
#include <log/log.hpp>
#include <sstream>

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
      switch (static_cast<Status>((lastStatusByte & 0x70) >> 4))
      {
      case Status::NoteOn: {
        const auto n = readU8();
        const auto v = readU8();
        events.emplace_back(deltaTime, channel, NoteOn{n, v});
        break;
      }
      case Status::NoteOff: {
        const auto n = readU8();
        const auto v = readU8();
        events.emplace_back(deltaTime, channel, NoteOff{n, v});
        break;
      }
      case Status::PolyKeyPressure: {
        const auto n = readU8();
        const auto v = readU8();
        events.emplace_back(deltaTime, channel, PolyKeyPressure{n, v});
        break;
      }
      case Status::ControlChange: {
        const auto n = readU8();
        const auto v = readU8();
        events.emplace_back(deltaTime, channel, ControlChange{static_cast<Control>(n), v});
        break;
      }
      case Status::ProgramChange: {
        events.emplace_back(deltaTime, channel, static_cast<ProgramChange>(readU8()));
        break;
      }
      case Status::ChannelPressure: {
        events.emplace_back(deltaTime, channel, static_cast<ChannelPressure>(readU8()));
        break;
      }
      case Status::PitchBend: {
        const auto b1 = readU8();
        const auto b2 = readU8();
        events.emplace_back(deltaTime, channel, PitchBend{(((b2 << 7) | b1) - 0x2000) / (1.f * 0x2000)});
        break;
      }
      case Status::System: {
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
  template <class>
  inline constexpr bool always_false_v = false;
  auto Track::write(std::ostream &st) const -> void
  {
    std::ostringstream ss;
    auto lastStatusByte = uint8_t{};
    for (const auto &e : events)
    {
      const auto ch = std::get<1>(e);
      std::visit(
        [&](auto &&arg) {
          using T = std::decay_t<decltype(arg)>;

          if constexpr (std::is_same_v<T, NoteOn>)
          {
            writeVlq(ss, std::get<0>(e));
            const auto statusByte = 0x80 | (static_cast<uint8_t>(Status::NoteOn) << 4) | ch;
            if (lastStatusByte != statusByte)
            {
              writeU8(ss, statusByte);
              lastStatusByte = statusByte;
            }
            writeU8(ss, arg.note);
            writeU8(ss, arg.vel);
          }
          else if constexpr (std::is_same_v<T, NoteOff>)
          {
            writeVlq(ss, std::get<0>(e));
            const auto statusByte = 0x80 | (static_cast<uint8_t>(Status::NoteOff) << 4) | ch;
            if (lastStatusByte != statusByte)
            {
              writeU8(ss, statusByte);
              lastStatusByte = statusByte;
            }
            writeU8(ss, arg.note);
            writeU8(ss, arg.vel);
          }
          else if constexpr (std::is_same_v<T, ProgramChange>)
          {
            writeVlq(ss, std::get<0>(e));
            const auto statusByte = 0x80 | (static_cast<uint8_t>(Status::ProgramChange) << 4) | ch;
            if (lastStatusByte != statusByte)
            {
              writeU8(ss, statusByte);
              lastStatusByte = statusByte;
            }
            writeU8(ss, static_cast<uint8_t>(arg));
          }
          else if constexpr (std::is_same_v<T, ControlChange>)
          {
            writeVlq(ss, std::get<0>(e));
            const auto statusByte = 0x80 | (static_cast<uint8_t>(Status::ControlChange) << 4) | ch;
            if (lastStatusByte != statusByte)
            {
              writeU8(ss, statusByte);
              lastStatusByte = statusByte;
            }
            writeU8(ss, static_cast<uint8_t>(arg.ctl));
            writeU8(ss, arg.val);
          }
          else if constexpr (std::is_same_v<T, PitchBend>)
          {
            writeVlq(ss, std::get<0>(e));
            const auto statusByte = 0x80 | (static_cast<uint8_t>(Status::PitchBend) << 4) | ch;
            if (lastStatusByte != statusByte)
            {
              writeU8(ss, statusByte);
              lastStatusByte = statusByte;
            }
            const auto v = static_cast<uint16_t>(arg.val * 0x2000 + 0x2000);
            writeU8(ss, v & 0x7f);
            writeU8(ss, (v >> 7) & 0x7f);
          }
          else if constexpr (std::is_same_v<T, ChannelPressure>)
          {
            writeVlq(ss, std::get<0>(e));
            const auto statusByte = 0x80 | (static_cast<uint8_t>(Status::ChannelPressure) << 4) | ch;
            if (lastStatusByte != statusByte)
            {
              writeU8(ss, statusByte);
              lastStatusByte = statusByte;
            }
            writeU8(ss, static_cast<uint8_t>(arg));
          }
          else if constexpr (std::is_same_v<T, PolyKeyPressure>)
          {
            writeVlq(ss, std::get<0>(e));
            const auto statusByte = 0x80 | (static_cast<uint8_t>(Status::PolyKeyPressure) << 4) | ch;
            if (lastStatusByte != statusByte)
            {
              writeU8(ss, statusByte);
              lastStatusByte = statusByte;
            }
            writeU8(ss, arg.note);
            writeU8(ss, arg.val);
          }
          else if constexpr (std::is_same_v<T, MetaEvent>)
          {
            writeVlq(ss, std::get<0>(e));
            lastStatusByte = 0;
            writeU8(ss, 0xff);
            writeU8(ss, static_cast<uint8_t>(arg.type));
            writeVlq(ss, arg.val.size());
            ss.write(arg.val.data(), arg.val.size());
          }
          else if constexpr (std::is_same_v<T, MidiChannelPrefix>)
          {
            writeVlq(ss, std::get<0>(e));
            lastStatusByte = 0;
            writeU8(ss, 0xff);
            writeU8(ss, static_cast<uint8_t>(MetaEventType::MidiChannelPrefix));
            writeVlq(ss, 1);
            writeU8(ss, static_cast<uint8_t>(arg));
          }
          else if constexpr (std::is_same_v<T, EndOfTrack>)
          {
            writeVlq(ss, std::get<0>(e));
            lastStatusByte = 0;
            writeU8(ss, 0xff);
            writeU8(ss, static_cast<uint8_t>(MetaEventType::EndOfTrack));
            writeVlq(ss, 0);
          }
          else if constexpr (std::is_same_v<T, SetTempo>)
          {
            writeVlq(ss, std::get<0>(e));
            lastStatusByte = 0;
            writeU8(ss, 0xff);
            writeU8(ss, static_cast<uint8_t>(MetaEventType::SetTempo));
            writeVlq(ss, 3);
            {
              const auto ch = (static_cast<uint32_t>(arg) >> 16) & 0xff;
              writeU8(ss, ch);
            }
            {
              const auto ch = (static_cast<uint32_t>(arg) >> 8) & 0xff;
              writeU8(ss, ch);
            }
            {
              const auto ch = (static_cast<uint32_t>(arg) >> 0) & 0xff;
              writeU8(ss, ch);
            }
          }
          else if constexpr (std::is_same_v<T, SmpteOffset>)
          {
            writeVlq(ss, std::get<0>(e));
            lastStatusByte = 0;
            writeU8(ss, 0xff);
            writeU8(ss, static_cast<uint8_t>(MetaEventType::SmpteOffset));
            writeVlq(ss, 5);
            writeU8(ss, arg.hr);
            writeU8(ss, arg.mn);
            writeU8(ss, arg.se);
            writeU8(ss, arg.fr);
            writeU8(ss, arg.ff);
          }
          else if constexpr (std::is_same_v<T, TimeSignature>)
          {
            writeVlq(ss, std::get<0>(e));
            lastStatusByte = 0;
            writeU8(ss, 0xff);
            writeU8(ss, static_cast<uint8_t>(MetaEventType::TimeSignature));
            writeVlq(ss, 4);
            writeU8(ss, arg.nn);
            writeU8(ss, arg.dd);
            writeU8(ss, arg.cc);
            writeU8(ss, arg.bb);
          }
          else if constexpr (std::is_same_v<T, KeySignature>)
          {
            writeVlq(ss, std::get<0>(e));
            lastStatusByte = 0;
            writeU8(ss, 0xff);
            writeU8(ss, static_cast<uint8_t>(MetaEventType::KeySignature));
            writeVlq(ss, 2);
            writeU8(ss, arg.sf);
            writeU8(ss, arg.mi);
          }
          else if constexpr (std::is_same_v<T, SysEx>)
          {
            writeVlq(ss, std::get<0>(e));
            lastStatusByte = 0;
            writeU8(ss, 0xf0);
            writeVlq(ss, arg.val.size());
            ss << arg.val;
          }
          else if constexpr (std::is_same_v<T, SysExF7>)
          {
            writeVlq(ss, std::get<0>(e));
            lastStatusByte = 0;
            writeU8(ss, 0xf7);
            writeVlq(ss, arg.val.size());
            ss << arg.val;
          }
          else
            static_assert(always_false_v<T>, "non-exhaustive visitor!");
        },
        std::get<2>(e));
    }

    writeU32(st, 'MTrk');
    writeU32(st, ss.str().size());
    st << ss.str();
  }
} // namespace midi
