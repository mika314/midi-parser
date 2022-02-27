#pragma once

namespace midi
{
  enum class Control {};
  struct ControlChange
  {
    Control ctl;
    uint8_t val;
  };
} // namespace midi
