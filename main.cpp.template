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
  for (const auto &path : recursive_directory_iterator("/home/mika/Music/midi/"))
  {
    const auto m = path.path();
    // const auto m = "/home/mika/Music/midi/11966.mid";
    try
    {
      LOG("");
      LOG(m);

      auto fs = std::ifstream{m, std::ios::binary};

      auto f = midi::File{fs};

      LOG("format", midi::toStr(f.header.format));
      LOG("ntrks", f.header.ntrks);
      LOG("division", toStr(f.header.division));

      for (const auto &trk : f.tracks)
      {
        LOG("events:", trk.events.size());
        for (const auto &evnt : trk.events)
        {
          std::visit(
            [&](auto &&e) {
              using T = std::decay_t<decltype(e)>;

              if constexpr (std::is_same_v<T, midi::NoteOn>) {}
              else if constexpr (std::is_same_v<T, midi::NoteOff>)
              {
              }
              else
                LOG(std::to_string(getDeltaTime(evnt)), std::to_string(getChannel(evnt)), midi::toStr(e));
            },
            getEvent(evnt));
        }
      }
      ++good;
    }
    catch (const std::runtime_error &e)
    {
      LOG("Invalid file:", m, "Error:", e.what());
      ++bad;
    }
  }
  LOG("good", good, "bad", bad);
}
