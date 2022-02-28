#pragma once
#include <cstdint>
#include <string>
#include <variant>

namespace midi
{
  enum class Status {
    NoteOff = 0,         // 0x80
    NoteOn = 1,          // 0x90
    PolyKeyPressure = 2, // 0xa0
    ControlChange = 3,   // 0xb0
    ProgramChange = 4,   // 0xc0
    ChannelPressure = 5, // 0xd0
    PitchBend = 6,       // 0xe0
    System = 7           // 0xf.
  };

  auto toStr(Status) -> const char *;

  enum class ChannelPressure : uint8_t {};
  auto toStr(ChannelPressure) -> std::string;
  auto val(ChannelPressure) -> int;

  enum class Control : uint8_t {
    Legato = 0x44,
    SoundVariation = 0x46,
    TimbreHarmonicIntensity = 0x47,
    Brightness = 0x4a,
    ReleaseTime = 0x48,
    AttackTime = 0x49,
    PortamentoController = 0x54,
    AllSoundOff = 120,
    ResetAllControllers = 121,
    LocalControl = 122,
    AllNotesOff = 123,
    OmniOff = 124,
    OmniOn = 125,
    MonoOn = 126,
    PolyOn = 127,
    ChangingTuningPrograms = 0x64
  };
  auto toStr(Control) -> const char *;

  struct ControlChange
  {
    Control ctl;
    uint8_t val;
  };
  auto toStr(ControlChange) -> std::string;

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
  auto toStr(MetaEventType) -> const char *;

  enum class MidiChannelPrefix : uint8_t {}; // 0-15
  auto val(MidiChannelPrefix) -> int;
  auto toStr(MidiChannelPrefix) -> std::string;
  enum class EndOfTrack : uint8_t {};
  auto toStr(EndOfTrack) -> const char *;
  enum class SetTempo : uint32_t {}; // microseconds per MIDI quarter-note
  auto toStr(SetTempo) -> std::string;
  auto val(SetTempo) -> uint32_t;
  auto getBpm(SetTempo) -> float;
  struct SmpteOffset
  {
    uint8_t hr;
    uint8_t mn;
    uint8_t se;
    uint8_t fr;
    uint8_t ff;
  };
  auto toStr(SmpteOffset) -> std::string;

  struct TimeSignature
  {
    uint8_t nn; // numerator
    uint8_t dd; // The denominator is a negative power of two: 2 represents a quarter-note, 3 represents an eighth-note, etc.
    uint8_t cc; // expresses the number of MIDI clocks in a metronome click
    uint8_t bb; // expresses the number of notated 32nd-notes in what MIDI thinks of as a quarter-note (24 MIDI Clocks)
  };
  auto toStr(TimeSignature) -> std::string;

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
  auto toStr(KeySignature) -> const char *;

  struct MetaEvent
  {
    MetaEventType type;
    std::string val;
  };

  auto toStr(const MetaEvent &) -> std::string;

  struct NoteOn
  {
    uint8_t note;
    uint8_t vel;
  };

  auto toStr(NoteOn) -> std::string;

  struct NoteOff
  {
    uint8_t note;
    uint8_t vel;
  };

  auto toStr(NoteOff) -> std::string;

  struct PitchBend
  {
    float val;
  };

  auto toStr(PitchBend) -> std::string;
  auto val(PitchBend) -> float;

  struct PolyKeyPressure
  {
    uint8_t note;
    uint8_t val;
  };

  auto toStr(PolyKeyPressure) -> std::string;

  enum class ProgramChange : uint8_t {};
  auto toStr(ProgramChange) -> std::string;
  auto val(ProgramChange) -> int;

  struct SysEx
  {
    std::string val;
  };
  auto toStr(SysEx) -> std::string;
  auto val(SysEx) -> std::string;

  struct SysExF7
  {
    std::string val;
  };
  auto toStr(SysExF7) -> std::string;
  auto val(SysExF7) -> std::string;

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

  auto toStr(const Event &) -> std::string;
} // namespace midi
