#pragma once
#include <variant>

namespace midi
{
  enum class ChannelPressure : uint8_t {};
  enum class Control : uint8_t {};
  struct ControlChange
  {
    Control ctl;
    uint8_t val;
  };

  enum class MetaEventType : uint8_t {
    SequenceNumber = 0x00,
    TextEvent = 0x01,
    CopyrightNotice = 0x02,
    SequenceTrackName = 0x03,
    InstrumentName = 0x04,
    Lyric = 0x05,
    Marker = 0x06,
    CuePoint = 0x07,
    MidiChannelPrefix = 0x20,
    EndOfTrack = 0x2F,
    SetTempo = 0x51,
    SmpteOffset = 0x54,
    TimeSignature = 0x58,
    KeySignature = 0x59,
    SequencerSpecificMetaEvent = 0x7f
  };

  enum class MidiChannelPrefix : uint8_t {}; // 0-15
  enum class EndOfTrack : uint8_t {};
  enum class SetTempo : uint32_t {}; // microseconds per MIDI quarter-note
  struct SmpteOffset
  {
    uint8_t hr;
    uint8_t mn;
    uint8_t se;
    uint8_t fr;
    uint8_t ff;
  };

  struct TimeSignature
  {
    uint8_t nn; // numerator
    uint8_t dd; // The denominator is a negative power of two: 2 represents a quarter-note, 3 represents an eighth-note, etc.
    uint8_t cc; // expresses the number of MIDI clocks in a metronome click
    uint8_t bb; // expresses the number of notated 32nd-notes in what MIDI thinks of as a quarter-note (24 MIDI Clocks)
  };

  struct KeySignature
  {
    int8_t sf;
    // sf = -7: 7 flats
    // sf = -1: 1 flat
    // sf = 0: key of C/Am
    // sf = 1: 1 sharp
    // sf = 7: 7 sharps
    bool mi;
    // mi = false: major key
    // mi = true: minor key
  };

  struct MetaEvent
  {
    MetaEventType type;
    std::string val;
  };

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

  struct PitchBend
  {
    float val;
  };

  struct PolyKeyPressure
  {
    uint8_t note;
    uint8_t val;
  };

  enum class ProgramChange : uint8_t {};

  struct SysEx
  {
    std::string val;
  };

  struct SysExF7
  {
    std::string val;
  };

  using Event = std::variant<NoteOn,
                             NoteOff,
                             ProgramChange,
                             ControlChange,
                             PitchBend,
                             ChannelPressure,
                             PolyKeyPressure,
                             MetaEvent,
                             MidiChannelPrefix,
                             EndOfTrack,
                             SetTempo,
                             SmpteOffset,
                             TimeSignature,
                             KeySignature,
                             SysEx,
                             SysExF7>;
} // namespace midi
