#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <unistd.h>

#include <Music/EventSetManager.h>
#include <Music/Music.h>
#include <TTY.h>

using namespace Music;

constexpr std::size_t MAX_DEGREES = 64;
constexpr std::size_t MAX_EVENTS = 128;
constexpr std::size_t SCALE_DEGREES = HEPATONIC;
constexpr int LOOP_SECONDS = 5;
constexpr float HALF = 0.5F;

#define TEMPERAMENT_DEGREES 12
#define MY_SCALE HEPATONIC_D12_SCALES[0]
#define WEIGHT_MAP SCALE_WEIGHTS_7_UNIFORM

using FRange = Range<float>;
using MySetup = Music::Setup<MAX_DEGREES, SCALE_DEGREES>;
using MyPitchEngine = Music::PitchEngine<MAX_DEGREES, SCALE_DEGREES>;
using MyScaleTable = Music::ScaleTable<TEMPERAMENT_DEGREES, SCALE_DEGREES>;
template <typename TRole>
using MyPersona = Music::Persona<TRole, MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;
using MyNoteManager =
    Music::EventSetManager<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;
using MyChordEventSet =
    Music::ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;
using MyPatternEventSet = Music::PatternEventSet<MAX_EVENTS>;
using MyNoteEventSet = Music::NoteEventSet<MAX_EVENTS>;

// Testing Setup 4/4 time 12-EDO
MySetup setup(4, NoteValue::Quarter, TWELVE_TONE, OCTAVE_DOUBLE);
MyPitchEngine pitchEngine;

// Different Test Personas
struct RoleTypeA {
  const MString<7> Name = "Role A";
  //  static constexpr const char Name[] = "Role A";
  const int octaveOffset;
  const FRange density;
  const Music::NoteValue granularity = Music::NoteValue::Quarter;

  RoleTypeA(int octaveOffset, FRange density)
      : octaveOffset(octaveOffset), density(density) {};
};

struct RoleTypeB {
  const MString<7> Name = "Role B";
  // static constexpr const char Name[] = "Role B";
  const int octaveOffset;
  const FRange density;
  const Music::NoteValue granularity = Music::NoteValue::Eighth;

  RoleTypeB(int octaveOffset, FRange density)
      : octaveOffset(octaveOffset), density(density) {};
};

RoleTypeA role1(0, FRange(0.0F, HALF));
RoleTypeB role2(0, FRange(HALF, 1.0F));

MyPersona<RoleTypeA> bob("BOB", setup, role1);
MyPersona<RoleTypeB> mary("MARY", setup, role2);

void doDebug(const char *format, va_list args) { std::vprintf(format, args); }

void testThing() {
  static MyChordEventSet chords;
  static MyNoteManager noteManager;
  const char *pName;
  const char *rName;
  Music::NoteValue g;

  if (randomRange(0.0F, 1.0F) < HALF) {
    noteManager.SetPersona(bob);
    pName = bob.GetPersonaName();
    rName = bob.GetRoleName();
    g = bob.GetGranularity();
  } else {
    noteManager.SetPersona(mary);
    pName = mary.GetPersonaName();
    rName = mary.GetRoleName();
    g = mary.GetGranularity();
  }

  const std::size_t scaleIdx = randomRange(static_cast<std::size_t>(0),
                                           ArrayLen(HEPATONIC_D12_SCALES) - 1);
  setup.scaleMap.SetScale(HEPATONIC_D12_SCALES[scaleIdx]);

  // std::cout << std::string(80, '-') << '\n';
  std::cout << TTY_CLEAR << "PERSONA: " << pName << ":" << rName
            << "\t\tSCALE: " << HEPATONIC_D12_SCALES[scaleIdx].name
            << "\t\tGRANULARITY: " << static_cast<unsigned int>(g) << " "
            << GetNoteValueText(g) << '\n';

  // Make Chord Progression
  GenerateStandardChordEvents<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>(
      setup, NoteValue::Whole, chords);

  DebugChordEvents<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>(setup, chords);
  std::cout << '\n';

  noteManager.MakeNoteEventsFromChords(chords);
  DebugNoteEvents<MAX_DEGREES, MAX_EVENTS>(
      setup.temperament, setup.timeSignature, noteManager.GetEvents());

  std::cout << '\n';

  NoteEventScore score =
      ScoreNoteEvents(setup.temperament, noteManager.GetEvents());
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

auto main(int argc, char *argv[]) -> int {
  SET_DEBUG(doDebug);

#if TEMPERAMENT_DEGREES == 12
  setup.temperament.AttachNoteLabels(Music::NOTE_NAMES_12);
  setup.temperament.AttachIntervalLabels(Music::INTERVAL_NAMES_12);
#elif TEMPERAMENT_DEGREES == 15
  setup.temperament.AttachNoteLabels(Music::NOTE_NAMES_15);
  setup.temperament.AttachIntervalLabels(Music::INTERVAL_NAMES_15);
#elif TEMPERAMENT_DEGREES == 17
  setup.temperament.AttachNoteLabels(Music::NOTE_NAMES_17);
  setup.temperament.AttachIntervalLabels(Music::INTERVAL_NAMES_17);
#elif TEMPERAMENT_DEGREES == 19
  setup.temperament.AttachNoteLabels(Music::NOTE_NAMES_19);
  setup.temperament.AttachIntervalLabels(Music::INTERVAL_NAMES_19);
#elif TEMPERAMENT_DEGREES == 22
  setup.temperament.AttachNoteLabels(Music::NOTE_NAMES_22);
  setup.temperament.AttachIntervalLabels(Music::INTERVAL_NAMES_22);
#else
#error Unsupported TEMPERAMENT_DEGREES for music_demo
#endif

  pitchEngine.SetTemperament(&setup.temperament);
  pitchEngine.SetScaleMap(&setup.scaleMap);
  pitchEngine.SetRootHz(C4FREQ); // C4

  for (;;) {
    testThing();
    sleep(LOOP_SECONDS);
  }
  return 0;
}
