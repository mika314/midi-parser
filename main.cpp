#include "file.hpp"
#include <filesystem>
#include <fstream>
#include <log/log.hpp>

auto main() -> int
{
  auto test = midi::File{};
  using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
  auto good = 0;
  auto bad = 0;
  for (const auto &m : recursive_directory_iterator("/home/mika/Music/midi/"))
  {
    //    try
    //    {
    LOG("");
    LOG(m);

    auto fs = std::ifstream{m.path(), std::ios::binary};
    auto f = midi::File{fs};

    // LOG("format", midi::toStr(f.header.format));
    // LOG("ntrks", f.header.ntrks);
    // LOG("division", toStr(f.header.division));

    {

      auto ofs = std::ofstream{"tmp.mid", std::ios::binary};
      f.write(ofs);
    }
    {
      auto fs = std::ifstream{"tmp.mid", std::ios::binary};
      auto f2 = midi::File{fs};
      LOG("tracks", f.tracks.size(), f2.tracks.size());
      for (auto trk = 0U; trk < std::min(f.tracks.size(), f2.tracks.size()); ++trk)
      {
        const auto &trk1 = f.tracks[trk];
        const auto &trk2 = f2.tracks[trk];
        LOG("track", trk, "events:", trk1.events.size(), trk2.events.size());
        for (auto evnt = 0U; evnt < std::min(trk1.events.size(), trk2.events.size()); ++evnt)
        {
          const auto &evnt1 = trk1.events[evnt];
          const auto &evnt2 = trk2.events[evnt];
          auto toStr = [](auto evnt) {
            return std::to_string(std::get<0>(evnt)) + " " + std::to_string(std::get<1>(evnt)) + " " +
                   std::visit(
                     [](auto &&v) -> std::string {
                       using T = std::decay_t<decltype(v)>;

                       if constexpr (std::is_same_v<T, midi::NoteOn>)
                         return "NoteOn " + std::to_string(v.note) + " " + std::to_string(v.vel);
                       else if constexpr (std::is_same_v<T, midi::NoteOff>)
                         return "NoteOff " + std::to_string(v.note) + " " + std::to_string(v.vel);
                       else if constexpr (std::is_same_v<T, midi::ProgramChange>)
                         return "ProgramChange " + std::to_string(static_cast<int>(v));
                       else if constexpr (std::is_same_v<T, midi::ControlChange>)
                         return "ControlChange " + std::to_string(static_cast<int>(v.ctl)) + " " + std::to_string(v.val);
                       else if constexpr (std::is_same_v<T, midi::PitchBend>)
                         return "PitchBend " + std::to_string(v.val);
                       else if constexpr (std::is_same_v<T, midi::ChannelPressure>)
                         return "ChannelPressure " + std::to_string(static_cast<int>(v));
                       else if constexpr (std::is_same_v<T, midi::PolyKeyPressure>)
                         return "PolyKeyPressure " + std::to_string(v.note) + " " + std::to_string(v.val);
                       else if constexpr (std::is_same_v<T, midi::MetaEvent>)
                         return "MetaEvent";
                       else if constexpr (std::is_same_v<T, midi::MidiChannelPrefix>)
                         return "MidiChannelPrefix " + std::to_string(static_cast<int>(v));
                       else if constexpr (std::is_same_v<T, midi::EndOfTrack>)
                         return "EndOfTrack";
                       else if constexpr (std::is_same_v<T, midi::SetTempo>)
                         return "SetTempo " + std::to_string(static_cast<int>(v)) + " " + std::to_string(60 * 1'000'000 / static_cast<int>(v));
                       else if constexpr (std::is_same_v<T, midi::SmpteOffset>)
                         return "SmpteOffset";
                       else if constexpr (std::is_same_v<T, midi::TimeSignature>)
                         return "TimeSignature";
                       else if constexpr (std::is_same_v<T, midi::KeySignature>)
                         return "KeySignature";
                       else if constexpr (std::is_same_v<T, midi::SysEx>)
                         return "SysEx";
                       else if constexpr (std::is_same_v<T, midi::SysExF7>)
                         return "SysExF7";
                     },
                     std::get<2>(evnt));
          };
          LOG(toStr(evnt1), "|", toStr(evnt2));
        }
      }

      // for (const auto &track : f.tracks)
      // {
      //   for (auto e : track.events)
      //   {
      //     if (std::holds_alternative<midi::NoteOn>(std::get<2>(e)))
      //     {
      //       LOG(std::get<0>(e), static_cast<int>(std::get<midi::NoteOn>(std::get<2>(e)).note));
      //     }
      //   }
      // }

      // for (const auto &track : f.tracks)
      // {
      //   for (auto e : track.events)
      //   {
      //     if (std::holds_alternative<midi::NoteOn>(std::get<2>(e)))
      //     {
      //       LOG(std::get<0>(e), static_cast<int>(std::get<midi::NoteOn>(std::get<2>(e)).note));
      //     }
      //   }
      // }
    }
    break;
    ++good;
    //    }
    //    catch (const std::runtime_error &e)
    //    {
    //      LOG("Invalid file:", m.path(), "Error:", e.what());
    //      ++bad;
    //    }
  }
  LOG("good", good, "bad", bad);
}
