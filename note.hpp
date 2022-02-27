#pragma once
namespace midi
{
  struct NoteOn
  {
    unsigned char note;
    unsigned char vel;
  };

  struct NoteOff
  {
    unsigned char note;
    unsigned char vel;
  };
} // namespace midi
