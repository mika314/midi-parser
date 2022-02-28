#include "event.hpp"
#include <cmath>

namespace midi
{

  auto toStr(Status v) -> const char *
  {
    switch (v)
    {
    case Status::NoteOff: return "NoteOff";
    case Status::NoteOn: return "NoteOn";
    case Status::PolyKeyPressure: return "PolyKeyPressure";
    case Status::ControlChange: return "ControlChange";
    case Status::ProgramChange: return "ProgramChange";
    case Status::ChannelPressure: return "ChannelPressure";
    case Status::PitchBend: return "PitchBend";
    case Status::System: return "System";
    }
    return "Unknown";
  }

  auto toStr(ChannelPressure v) -> std::string { return std::to_string(val(v)); }
  auto val(ChannelPressure v) -> int { return static_cast<int>(v); }

  auto toStr(Control v) -> const char *
  {
    switch (v)
    {
    case Control::Legato: return "Legato";
    case Control::SoundVariation: return "SoundVariation";
    case Control::TimbreHarmonicIntensity: return "TimbreHarmonicIntensity";
    case Control::Brightness: return "Brightness";
    case Control::ReleaseTime: return "ReleaseTime";
    case Control::AttackTime: return "AttackTime";
    case Control::PortamentoController: return "PortamentoController";
    case Control::AllSoundOff: return "AllSoundOff";
    case Control::ResetAllControllers: return "ResetAllControllers";
    case Control::LocalControl: return "LocalControl";
    case Control::AllNotesOff: return "AllNotesOff";
    case Control::OmniOff: return "OmniOff";
    case Control::OmniOn: return "OmniOn";
    case Control::MonoOn: return "MonoOn";
    case Control::PolyOn: return "PolyOn";
    case Control::ChangingTuningPrograms: return "ChangingTuningPrograms";
    }
    return "Unknown";
  }

  auto toStr(ControlChange v) -> std::string
  {
    return std::string{"ControlChange "} + toStr(v.ctl) + "(" + std::to_string(static_cast<int>(v.ctl)) + ")" + ":" + std::to_string(v.val);
  }
  auto toStr(MetaEventType v) -> const char *
  {
    switch (v)
    {
    case MetaEventType::SequenceNumber: return "SequenceNumber";
    case MetaEventType::TextEvent: return "TextEvent";
    case MetaEventType::CopyrightNotice: return "CopyrightNotice";
    case MetaEventType::SequenceTrackName: return "SequenceTrackName";
    case MetaEventType::InstrumentName: return "InstrumentName";
    case MetaEventType::Lyric: return "Lyric";
    case MetaEventType::Marker: return "Marker";
    case MetaEventType::CuePoint: return "CuePoint";
    case MetaEventType::MidiChannelPrefix: return "MidiChannelPrefix";
    case MetaEventType::EndOfTrack: return "EndOfTrack";
    case MetaEventType::SetTempo: return "SetTempo";
    case MetaEventType::SmpteOffset: return "SmpteOffset";
    case MetaEventType::TimeSignature: return "TimeSignature";
    case MetaEventType::KeySignature: return "KeySignature";
    case MetaEventType::SequencerSpecificMetaEvent: return "SequencerSpecificMetaEvent";
    }
    return "Unknown";
  }
  auto val(MidiChannelPrefix v) -> int { return static_cast<int>(v); }
  auto toStr(MidiChannelPrefix v) -> std::string { return "MidiChannelPrefix " + std::to_string(val(v)); }
  auto toStr(EndOfTrack) -> const char * { return "EndOfTrack"; }
  auto toStr(SetTempo v) -> std::string { return "SetTempo " + std::to_string(val(v)); }
  auto val(SetTempo v) -> uint32_t { return static_cast<uint32_t>(v); }
  auto getBpm(SetTempo v) -> float { return 60.f * 1'000'000 / val(v); }
  auto toStr(SmpteOffset v) -> std::string
  {
    return "SmpteOffset " + std::to_string(v.hr) + ":" + std::to_string(v.mn) + ":" + std::to_string(v.se) + ":" + std::to_string(v.fr) + ":" +
           std::to_string(v.ff);
  }

  auto toStr(TimeSignature v) -> std::string
  {
    return "TimeSignature " + std::to_string(v.nn) + "/" + std::to_string(std::pow(2, v.dd)) + " " + std::to_string(v.cc) + " " +
           std::to_string(v.bb);
  }

  auto toStr(KeySignature v) -> const char *
  {
    switch (v.sf)
    {
    case -7: return v.mi ? "KeySignature Abm" : "KeySignature B";
    case -6: return v.mi ? "KeySignature Ebm" : "KeySignature Gb";
    case -5: return v.mi ? "KeySignature Bbm" : "KeySignature Db";
    case -4: return v.mi ? "KeySignature Fm" : "KeySignature Ab";
    case -3: return v.mi ? "KeySignature Cm" : "KeySignature Eb";
    case -2: return v.mi ? "KeySignature Gm" : "KeySignature Bb";
    case -1: return v.mi ? "KeySignature Dm" : "KeySignature F";
    case 0: return v.mi ? "KeySignature Am" : "KeySignature C";
    case 1: return v.mi ? "KeySignature Em" : "KeySignature G";
    case 2: return v.mi ? "KeySignature Bm" : "KeySignature D";
    case 3: return v.mi ? "KeySignature F#m" : "KeySignature A";
    case 4: return v.mi ? "KeySignature C#m" : "KeySignature E";
    case 5: return v.mi ? "KeySignature G#m" : "KeySignature B";
    case 6: return v.mi ? "KeySignature D#m" : "KeySignature F#";
    case 7: return v.mi ? "KeySignature A#m" : "KeySignature C#";
    }
    return "Unknown";
  }

  auto toStr(const MetaEvent &v) -> std::string
  {
    return std::string{"MetaEvent "} + toStr(v.type) + "(" + std::to_string(static_cast<int>(v.type)) + "):" + v.val;
  }
  auto toStr(NoteOn v) -> std::string { return "NoteOn  " + std::to_string(v.note) + ":" + std::to_string(v.vel); }
  auto toStr(NoteOff v) -> std::string { return "NoteOff " + std::to_string(v.note) + ":" + std::to_string(v.vel); }
  auto toStr(PitchBend v) -> std::string { return "PitchBend " + std::to_string(val(v)); }
  auto val(PitchBend v) -> float { return v.val; }
  auto toStr(PolyKeyPressure v) -> std::string { return "PolyKeyPressure " + std::to_string(v.note) + ":" + std::to_string(v.val); }
  auto toStr(ProgramChange v) -> std::string { return "ProgramChange " + std::to_string(val(v)); }
  auto val(ProgramChange v) -> int { return static_cast<int>(v); }
  auto toStr(SysEx v) -> std::string { return "SysEx " + val(v); }
  auto val(SysEx v) -> std::string { return v.val; }
  auto toStr(SysExF7 v) -> std::string { return "SysExF7 " + val(v); }
  auto val(SysExF7 v) -> std::string { return v.val; }
  auto toStr(const Event &v) -> std::string
  {
    return std::visit([](auto &&arg) -> std::string { return toStr(arg); }, v);
  }
} // namespace midi
