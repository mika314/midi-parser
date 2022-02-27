#include "file.hpp"
#include <filesystem>
#include <log/log.hpp>

auto main() -> int
{
  using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
  auto good = 0;
  auto bad = 0;
  for (const auto &m : recursive_directory_iterator("/home/mika/Music/midi/"))
  {
    try
    {
      // LOG("");
      // LOG(m);
      auto f = midi::File{m.path()};
      // LOG("format", midi::toStr(f.header.format));
      // LOG("ntrks", f.header.ntrks);
      // LOG("division", toStr(f.header.division));
      ++good;
    }
    catch (const std::runtime_error &e)
    {
      LOG("Invalid file:", m.path(), "Error:", e.what());
      ++bad;
    }
  }
  LOG("good", good, "bad", bad);
}
