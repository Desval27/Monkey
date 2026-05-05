#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <unistd.h>

#include <Music/Music.h>
#include <TTY.h>

#define DO_DEBUG

using namespace Music;

constexpr std::size_t MAX_DEGREES = 64;
constexpr std::size_t MAX_EVENTS = 128;

#define TEMPERAMENT_DEGREES 12
#define MY_SCALE HEPATONIC_D12_SCALES[0]
#define WEIGHT_MAP SCALE_WEIGHTS_7_UNIFORM
constexpr std::size_t SCALE_DEGREES = HEPATONIC;

using MySetup = Music::Setup<MAX_DEGREES, SCALE_DEGREES>;
using MyPitchEngine = Music::PitchEngine<MAX_DEGREES, SCALE_DEGREES>;
using MyScaleTable = Music::ScaleTable<TEMPERAMENT_DEGREES, SCALE_DEGREES>;

using MyChordEventSet =
    Music::ChordEventSet<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;
using MyPatternEventSet = Music::PatternEventSet<MAX_EVENTS>;
using MyNoteEventSet = Music::NoteEventSet<MAX_EVENTS>;
using MySimpleRandomPatternGenerator =
    Music::SimpleRandomPatternGenerator<MAX_EVENTS>;
using MyInversionPatternGenerator =
    Music::InversionPatternGenerator<MAX_EVENTS>;
using MyEuclidianPatternGenerator =
    Music::EuclidianPatternGenerator<MAX_EVENTS>;
using MyStyleANoteGenerator =
    Music::StyleANoteGenerator<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>;

// Testing Setup 3/4 time 12-EDO
MySetup setup(4, NoteValue::Quarter, 12, 2.0f);
MyPitchEngine pitchEngine;

void doDebug(const char *format, va_list args) { std::vprintf(format, args); }

void testThing() {
  static MyChordEventSet chords;
  static MyPatternEventSet pattern;
  static MyNoteEventSet noteEvents;
  static float density;
  static NoteValue g;

  const std::size_t scaleIdx = randomRange(static_cast<std::size_t>(0), ArrayLen(HEPATONIC_D12_SCALES)-1);
  setup.scaleMap.SetScale(HEPATONIC_D12_SCALES[scaleIdx]);

  // std::cout << std::string(80, '-') << std::endl;
  std::cout << TTY_CLEAR;
  std::cout << "SCALE: " << HEPATONIC_D12_SCALES[scaleIdx].name << std::endl;

  // Make Chord Progression
  GenerateStandardChordEvents<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>(
      setup, NoteValue::Whole, chords);

  // We have a 25% chance of simply inverting a prior run
  // But there will still be chord changes from above
  // If that's not the case then we will regenerate our parameters
  if (randomRange(0.0, 1.0) <= 0.25 && pattern.Count() > 0) {
    MyInversionPatternGenerator::GeneratePattern(
        setup.timeSignature, setup.bars, density, g, pattern);
  } else {
    density = randomRange(0.6f, 0.9f); // 0.50f;
    g = GetRandomGranularity(NoteValue::Eighth, NoteValue::Whole);
    pattern.Clear();

    if (randomRange(0.0, 1.0) > 0.50) {
      MySimpleRandomPatternGenerator::GeneratePattern(
          setup.timeSignature, setup.bars, density, g, pattern);
    } else {
      MyEuclidianPatternGenerator::GeneratePattern(
          setup.timeSignature, setup.bars, density, g, pattern);
    }
  }

  std::cout << "Granularity: " << static_cast<int>(g) << " "
            << GetNoteValueText(g) << std::endl;

  DebugChordEvents<MAX_DEGREES, SCALE_DEGREES, MAX_EVENTS>(setup, chords);
  std::cout << std::endl;

  DebugPattern<MAX_EVENTS>(setup.timeSignature, g, pattern);
  std::cout << std::endl;

  // Make Notes from "Hit" Pattern
  MyStyleANoteGenerator::GenerateEvents(setup, pattern, chords, g, WEIGHT_MAP,
                                        noteEvents);
  DebugNoteEvents<MAX_DEGREES, MAX_EVENTS>(setup.temperament,
                                           setup.timeSignature, noteEvents);
  std::cout << std::endl;

  NoteEventScore score = ScoreNoteEvents(setup.temperament, noteEvents);
  std::cout << TTY_FG_MAGENTA << "Event Score" << TTY_RESET << ": Overall    "
            << score.overall << std::endl;
  std::cout << "           : Density    " << score.density << std::endl;
  std::cout << "           : Rests      " << score.rests << std::endl;
  std::cout << "           : Cadence    " << score.cadence << std::endl;
  std::cout << "           : Repetition " << score.repetition << std::endl;
  std::cout << "           : Contour    " << score.contour << std::endl;
  std::cout << "           : Phrase     " << score.phrase << std::endl;
  std::cout << "           : Rhythm     " << score.rhythm << std::endl;
  std::cout << std::endl;
}

int main(int argc, char *argv[]) {
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
    sleep(5);
  }
  return 0;
}
