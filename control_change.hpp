#pragma once

namespace midi
{
  enum class Control {};
  struct ControlChange
  {
    Control ctl;
    unsigned char val;
  };
} // namespace midi
