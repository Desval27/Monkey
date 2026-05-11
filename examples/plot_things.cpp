#include <music/music.hpp>
#include <music/pattern_generator.hpp>
#include <unistd.h>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

using namespace music;

constexpr int EVENT_POOL_SIZE = 5000;
constexpr ScaleType SCALE_TYPE = ScaleType::HEPATONIC;

#define PALETTE IONIAN_D15
#define PALETTE_LEN ArrayLen(PALETTE)

Setup<DEF_MAX_DEGREES, SCALE_TYPE> setup;
ChordEventSet<DEF_MAX_DEGREES, SCALE_TYPE, DEF_MAX_EVENTS> chords;

void
DoThing()
{
  const float density = randomRange(0.0F, 1.0F); // 0.50f;
  const NoteValue g =
    get_random_granularity(NoteValue::Sixteenth, NoteValue::Whole);
  PatternEventSet<> pattern;
  NoteEventSet<> noteEvents;

  EuclidianPatternGenerator<>::generate_pattern(
    setup.time_signature, setup.bars, density, g, pattern);

  StyleANoteGenerator<DEF_MAX_DEGREES, SCALE_TYPE, DEF_MAX_EVENTS>::
    generate_events(
      setup, pattern, chords, g, SCALE_WEIGHTS_7_UNIFORM, noteEvents);
  NoteEventScore score = ScoreNoteEvents(setup.temperament, noteEvents);

  float v1 = density;
  float v2 = static_cast<float>(score.rests) / 100.0F;

  std::cout << score.overall << '\t' << v1 << '\t' << v2 << '\n';
}

int
main(int argc, char* argv[])
{
  setup.temperament.attach_note_labels(NOTE_NAMES_12);
  setup.temperament.attach_interval_labels(INTERVAL_NAMES_12);
  setup.scale_map.set_degrees(IONIAN_D12);

  std::cout << "set title 'Density/Granularity vs Score'" << '\n';
  std::cout << "set xlabel 'Density/Granularity'" << '\n';
  std::cout << "set ylabel 'Score'" << '\n';
  std::cout << "set grid" << '\n';

  std::cout << "$MyData << EOD" << '\n';
  // std::cout << "plot '-' using 2:1 with points" << std::endl;

  GenerateStandardChordEvents(setup, NoteValue::Whole, chords);
  for (int i = 0; i < EVENT_POOL_SIZE; i++) {
    DoThing();
  }
  std::cout << "EOD" << '\n';
  std::cout << "plot $MyData using 2:1 with points title 'Density', "
            << "$MyData using 3:1 with points title 'Granularity'" << '\n';
  // std::cout << "e" << std::endl;

  return 0;
}
