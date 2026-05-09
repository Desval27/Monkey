#include <music/event_set_manager.hpp>
#include <music/music.hpp>
#include <tty.h>
#include <unistd.h>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

using namespace music;

constexpr std::size_t MAX_DEGREES = 64;
constexpr std::size_t MAX_EVENTS = 128;
constexpr std::size_t SCALE_DEGREES = HEPATONIC;
constexpr int LOOP_SECONDS = 5;
constexpr float HALF = 0.5F;

#define TEMPERAMENT_DEGREES 12
#define MY_SCALE HEPATONIC_D12_SCALES[0]
#define WEIGHT_MAP SCALE_WEIGHTS_7_UNIFORM

using FRange = Range<float>;
using MySetup = Setup<MAX_DEGREES, SCALE_DEGREES>;
using MyPitchEngine = PitchEngine<MAX_DEGREES, SCALE_DEGREES>;
using MyScaleTable = ScaleTable<TEMPERAMENT_DEGREES, SCALE_DEGREES>;
template<typename TRole>
using MyPersona = Persona<TRole, MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;
using MyNoteManager = EventSetManager<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;
using MyChordEventSet = ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;
using MyPatternEventSet = PatternEventSet<MAX_EVENTS>;
using MyNoteEventSet = NoteEventSet<MAX_EVENTS>;

// Testing Setup 4/4 time 12-EDO
MySetup setup(4, NoteValue::Quarter, TWELVE_TONE, OCTAVE_DOUBLE);
MyPitchEngine pitchEngine;

// Different Test Personas
struct RoleTypeA
{
  const MString<7> Name = "Role A";
  //  static constexpr const char Name[] = "Role A";
  const int octaveOffset;
  const FRange density;
  const NoteValue granularity = NoteValue::Quarter;

  RoleTypeA(int octaveOffset, FRange density)
    : octaveOffset(octaveOffset)
    , density(density) {};
};

struct RoleTypeB
{
  const MString<7> Name = "Role B";
  // static constexpr const char Name[] = "Role B";
  const int octaveOffset;
  const FRange density;
  const NoteValue granularity = NoteValue::Eighth;

  RoleTypeB(int octaveOffset, FRange density)
    : octaveOffset(octaveOffset)
    , density(density) {};
};

RoleTypeA role1(0, FRange(0.0F, HALF));
RoleTypeB role2(0, FRange(HALF, 1.0F));

MyPersona<RoleTypeA> bob("BOB", setup, role1);
MyPersona<RoleTypeB> mary("MARY", setup, role2);

void
doDebug(const char* format, va_list args)
{
  std::vprintf(format, args);
}

void
testThing()
{
  static MyChordEventSet chords;
  static MyNoteManager noteManager;
  const char* pName;
  const char* rName;
  NoteValue g;

  if (randomRange(0.0F, 1.0F) < HALF) {
    noteManager.set_persona(bob);
    pName = bob.GetPersonaName();
    rName = bob.GetRoleName();
    g = bob.GetGranularity();
  } else {
    noteManager.set_persona(mary);
    pName = mary.GetPersonaName();
    rName = mary.GetRoleName();
    g = mary.GetGranularity();
  }

  const std::size_t scaleIdx = randomRange(static_cast<std::size_t>(0),
                                           ArrayLen(HEPATONIC_D12_SCALES) - 1);
  setup.scaleMap.set_scale(HEPATONIC_D12_SCALES[scaleIdx]);

  // std::cout << std::string(80, '-') << '\n';
  std::cout << TTY_CLEAR << "PERSONA: " << pName << ":" << rName
            << "\t\tSCALE: " << HEPATONIC_D12_SCALES[scaleIdx].name
            << "\t\tGRANULARITY: " << static_cast<unsigned int>(g) << " "
            << get_note_value_text(g) << '\n';

  // Make Chord Progression
  GenerateStandardChordEvents<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>(
    setup, NoteValue::Whole, chords);

  DebugChordEvents<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>(setup, chords);
  std::cout << '\n';

  noteManager.make_note_events_from_chords(chords);
  DebugNoteEvents<MAX_DEGREES, MAX_EVENTS>(
    setup.temperament, setup.timeSignature, noteManager.get_events());

  std::cout << '\n';

  NoteEventScore score =
    ScoreNoteEvents(setup.temperament, noteManager.get_events());
  std::cout << TTY_FG_MAGENTA << "Event Score" << TTY_RESET << ": Overall    "
            << score.overall << '\n';
  std::cout << "           : Density    " << score.density << '\n';
  std::cout << "           : Rests      " << score.rests << '\n';
  std::cout << "           : Cadence    " << score.cadence << '\n';
  std::cout << "           : Repetition " << score.repetition << '\n';
  std::cout << "           : Contour    " << score.contour << '\n';
  std::cout << "           : Phrase     " << score.phrase << '\n';
  std::cout << "           : Rhythm     " << score.rhythm << '\n';
  std::cout << '\n';
}

auto
main(int argc, char* argv[]) -> int
{
  SET_DEBUG(doDebug);

#if false
  for (const auto& t : EQUAL_TEMPERAMENT_TABLE) {
    std::cout << "Name: " << t.name << "\tDegrees: " << t.degreesInPeriod
              << "\n";
    for (int i = 0; i < t.degreesInPeriod; i++) {
      std::cout << "\tInterval: " << t.intervalLabels[i]
                << "\tNote: " << t.noteLabels[i] << "\n";
    }
  }
  return 0;
#endif

#if TEMPERAMENT_DEGREES == 12
  setup.temperament.attach_note_labels(NOTE_NAMES_12);
  setup.temperament.attach_interval_labels(INTERVAL_NAMES_12);
#elif TEMPERAMENT_DEGREES == 15
  setup.temperament.attach_note_labels(NOTE_NAMES_15);
  setup.temperament.attach_interval_labels(INTERVAL_NAMES_15);
#elif TEMPERAMENT_DEGREES == 17
  setup.temperament.attach_note_labels(NOTE_NAMES_17);
  setup.temperament.attach_interval_labels(INTERVAL_NAMES_17);
#elif TEMPERAMENT_DEGREES == 19
  setup.temperament.attach_note_labels(NOTE_NAMES_19);
  setup.temperament.attach_interval_labels(INTERVAL_NAMES_19);
#elif TEMPERAMENT_DEGREES == 22
  setup.temperament.attach_note_labels(NOTE_NAMES_22);
  setup.temperament.attach_interval_labels(INTERVAL_NAMES_22);
#else
#error Unsupported TEMPERAMENT_DEGREES for music_demo
#endif

  pitchEngine.set_temperament(&setup.temperament);
  pitchEngine.set_scale_map(&setup.scaleMap);
  pitchEngine.set_root_hz(C4FREQ); // C4

  for (;;) {
    testThing();
    sleep(LOOP_SECONDS);
  }
  return 0;
}
