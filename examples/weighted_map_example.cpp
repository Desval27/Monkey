#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <Monkey.h>
#include <Music/Music.h>
#include <Music/Tables.h>
#include <Music/ScaleMaps.h>

int main()
{
    using TempT  = Music::Temperament;
    using ScaleT = Music::ScaleMap;
    using PitchT = Music::PitchEngine;

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    TempT temperament;
    temperament.MakeEqualDivision(12, 2.0f);
    temperament.AttachNoteLabels(Music::NOTE_NAMES_12,
                                 ArrayLen(Music::NOTE_NAMES_12));

    ScaleT scale;
    scale.SetDegrees(Music::MAJOR_PENTATONIC_D12, ArrayLen(Music::MAJOR_PENTATONIC_D12));
    // scale.setWeightedDegrees(Music::MAJOR_PENTATONIC_DEGREES_12,
    //                         //  Music::SCALE_WEIGHTS_5_CHORD_TONE_HEAVY,
    //                          ArrayLen(Music::MAJOR_PENTATONIC_DEGREES_12));

    // A4 = 440 Hz in 12-EDO where C is degree 0 and A is degree 9.
    const Music::TuningReference refA4(Music::BASE_HZ, 9, 0);
    const float                  rootC4Hz
        = temperament.FrequencyFromReference(Music::TemperedPitch(0, 0), refA4);

    PitchT engine;
    engine.SetTemperament(&temperament);
    engine.SetScaleMap(&scale);
    engine.SetRootHz(rootC4Hz);

    std::printf("Root derived from A4=440Hz: C4=%.4f Hz\n", rootC4Hz);

    for(int step = 0; step < 16; ++step)
    {
        const float   unitRandom = randomRange(0.0f, 0.999999f);
        int           period     = 0;
        const int16_t degree     = scale.GetWeightedMappedDegree(
            step,
            unitRandom,
            period,
            Music::SCALE_WEIGHTS_5_CHORD_TONE_HEAVY,
            ArrayLen(Music::SCALE_WEIGHTS_5_CHORD_TONE_HEAVY));
        const float freq = engine.FrequencyFromWeightedScaleIndex(
            step,
            unitRandom,
            Music::SCALE_WEIGHTS_5_CHORD_TONE_HEAVY,
            ArrayLen(Music::SCALE_WEIGHTS_5_CHORD_TONE_HEAVY));

        char note[8];
        note[0] = '\0';
        temperament.GetNoteLabel(
            static_cast<uint16_t>(degree), note, sizeof(note));

        std::printf(
            "Step %2d  r=%.3f  degree=%2d  period=%2d  note=%-2s  freq=%8.3f "
            "Hz\n",
            step,
            unitRandom,
            static_cast<int>(degree),
            period,
            note,
            freq);
    }

    return 0;
}
