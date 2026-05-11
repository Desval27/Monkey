#include <Music/event_set_manager.hpp>
#include <Music/music.hpp>
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

using TSetup = Setup<MAX_DEGREES, SCALE_DEGREES>;
using TPitchEngine = PitchEngine<MAX_DEGREES, SCALE_DEGREES>;
using TScaleTable = ScaleTable<TEMPERAMENT_DEGREES, SCALE_DEGREES>;

template<typename TRole>
using TPersona = Persona<TRole, MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;
using TPatternEventSet = PatternEventSet<MAX_EVENTS>;
using TChordEventSet = ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;

using TNoteEventSet = NoteEventSet<MAX_EVENTS>;
using TChordManager =
  EventSetManager<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS, TChordEventSet>;
using MyNoteManager = EventSetManager<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;
using TStockPersonaRole = StockPersonaRole<SCALE_DEGREES>;

// Testing Setup 4/4 time 12-EDO
TSetup setup(4, NoteValue::Quarter, TWELVE_TONE, OCTAVE_DOUBLE);
TPitchEngine pitchEngine;

TStockPersonaRole roleChords;
TStockPersonaRole roleSimple(NoteValue::Half,
                             0,
                             SCALE_WEIGHTS_7_TONIC_HEAVY,
                             0.4F,
                             0.8F);
TStockPersonaRole roleBusy(NoteValue::Eighth, 0, 0.1F, 0.9F, 0.3F, 0.6F);

TPersona<TStockPersonaRole> clyde("CLYDE", setup, roleChords);
TPersona<TStockPersonaRole> bob("BOB", setup, roleSimple);
TPersona<TStockPersonaRole> mary("MARY", setup, roleBusy);

void
doDebug(const char* format, va_list args)
{
  std::vprintf(format, args);
}

void
testThing()
{
  // static TChordEventSet chords;
  static MyNoteManager noteManager(setup);
  static TChordManager chordManager(setup);
  const char* pName;
  const char* rName;
  NoteValue g;

  chordManager.set_persona(clyde);

  if (randomRange(0.0F, 1.0F) < HALF) {
    noteManager.set_persona(bob);
    pName = bob.GetPersonaName();
    rName = bob.GetRoleName();
    g = bob.get_granularity();
  } else {
    noteManager.set_persona(mary);
    pName = mary.GetPersonaName();
    rName = mary.GetRoleName();
    g = mary.get_granularity();
  }

  const std::size_t scaleIdx = randomRange(static_cast<std::size_t>(0),
                                           ArrayLen(HEPATONIC_D12_SCALES) - 1);
  setup.scale_map.set_scale(HEPATONIC_D12_SCALES[scaleIdx]);

  // std::cout << std::string(80, '-') << '\n';
  std::cout << TTY_CLEAR << "PERSONA: " << pName << ":" << rName
            << "\t\tSCALE: " << HEPATONIC_D12_SCALES[scaleIdx].name
            << "\t\tGRANULARITY: " << static_cast<unsigned int>(g) << " "
            << get_note_value_text(g) << '\n';

  // Make Chord Progression
  // GenerateStandardChordEvents<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>(
  //   setup, NoteValue::Whole, chords);
  chordManager.make_chord_events();
  DebugChordEvents<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>(
    setup, chordManager.get_events());
  std::cout << '\n';

  noteManager.make_note_events_from_chords(chordManager.get_events());
  DebugNoteEvents<MAX_DEGREES, MAX_EVENTS>(
    setup.temperament, setup.time_signature, noteManager.get_events());

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
    std::cout << "Name: " << t.name << "\tDegrees: " << t.degrees_in_period
              << "\n";
    for (int i = 0; i < t.degrees_in_period; i++) {
      std::cout << "\tInterval: " << t.interval_labels[i]
                << "\tNote: " << t.note_labels[i] << "\n";
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
  pitchEngine.set_scale_map(&setup.scale_map);
  pitchEngine.set_root_hz(C4FREQ); // C4

  for (;;) {
    testThing();
    sleep(LOOP_SECONDS);
  }
  return 0;
}
