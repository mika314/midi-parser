#pragma once
namespace midi
{
  struct NoteOn
  {
    uint8_t note;
    uint8_t vel;
  };

  struct NoteOff
  {
    uint8_t note;
    uint8_t vel;
  };
} // namespace midi
