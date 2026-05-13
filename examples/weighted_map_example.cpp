#include <monkey.hpp>
#include <music/music.hpp>
#include <music/music_tables.hpp>

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace music;

int
main()
{
  using TempT = Temperament<DEF_MAX_DEGREES>;
  using ScaleT = ScaleMap<HEPATONIC>;
  using PitchT = PitchEngine<DEF_MAX_DEGREES, HEPATONIC>;

  TempT temperament;
  temperament.make_equal_division(12, 2.0F);
  temperament.attach_note_labels(NOTE_NAMES_12);

  ScaleT scale;
  scale.set_degrees(IONIAN_D12);

  // A4 = 440 Hz in 12-EDO where C is degree 0 and A is degree 9.
  const TuningReference refA4(BASE_HZ, 9, 0);
  const float rootC4Hz =
    temperament.frequency_from_reference(TemperedPitch(0, 0), refA4);

  PitchT engine;
  engine.set_temperament(&temperament);
  engine.set_scale_map(&scale);
  engine.set_root_hz(rootC4Hz);

  std::printf("Root derived from A4=440Hz: C4=%.4f Hz\n", rootC4Hz);

  for (int step = 0; step < 16; ++step) {
    const float unitRandom = random_range(0.0f, 0.999999f);
    int period = 0;
    const int16_t degree = scale.get_weighted_mapped_degree(
      step, unitRandom, period, SCALE_WEIGHTS_7_CHORD_TONE_HEAVY);
    const float freq = engine.frequency_from_weighted_scale_index(
      step, unitRandom, SCALE_WEIGHTS_7_CHORD_TONE_HEAVY);

    char note[8];
    note[0] = '\0';
    temperament.get_note_label(
      static_cast<uint16_t>(degree), note, sizeof(note));

    std::cout << "Step " << std::setw(2) << step
              << ": unitRandom=" << std::fixed << std::setprecision(4)
              << unitRandom << " degree=" << static_cast<int>(degree)
              << " period=" << period << " note=" << note
              << " freq=" << std::setw(10) << std::fixed << std::setprecision(4)
              << freq << " Hz" << std::endl;
  }

  return 0;
}
