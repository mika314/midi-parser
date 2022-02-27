#pragma once
#include "channel_pressure.hpp"
#include "control_change.hpp"
#include "meta_event.hpp"
#include "note.hpp"
#include "pitch_bend.hpp"
#include "poly_key_pressure.hpp"
#include "program_change.hpp"
#include "sysex.hpp"
#include "sysex_f7.hpp"
#include <variant>

namespace midi
{
  using Event = std::variant<NoteOn, NoteOff, ProgramChange, ControlChange, PitchBend, ChannelPressure, PolyKeyPressure, MetaEvent, SysEx, SysExF7>;
} // namespace midi
