#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <unistd.h>
#include <string>
#include <iostream>

#include <Music/Music.h>

using namespace Music;

#define PALETTE IONIAN_D15
#define PALETTE_LEN ArrayLen(PALETTE)

TimeSignature ts;
Temperament t;
ScaleMap scale;
PitchEngine pe;
const HarmonicMode mode = HarmonicMode::Major;

const int bars = 8;
ChordEventSet<> chords;

void DoThing()
{
    const float density = randomRange(0.0f, 1.0f); // 0.50f;
    const NoteValue g = GetRandomGranularity(NoteValue::Sixteenth, NoteValue::Whole);
    PatternEventSet<> pattern;
    NoteEventSet<> noteEvents;

    EuclidianPatternGenerator<>::GeneratePattern(ts, bars, density, g, pattern);

    StyleANoteGenerator<>::GenerateEvents(pattern, chords, ts, t, scale, bars, g, noteEvents);
    NoteEventScore score = ScoreNoteEvents(t, noteEvents);

    float v1 = density;
    float v2 = score.rests / 100.0;

    std::cout << score.overall << '\t' << v1 << '\t' << v2 << std::endl;
}

int main(int argc, char *argv[])
{
    // Random number seed
    std::srand(std::time(nullptr));

    t.MakeEqualDivision(12, 2.0f);
    t.AttachNoteLabels(Music::NOTE_NAMES_12, ArrayLen(Music::NOTE_NAMES_12));
    t.AttachIntervalLabels(Music::INTERVAL_NAMES_12,
                           ArrayLen(Music::INTERVAL_NAMES_12));

    scale.SetDegrees(PALETTE, PALETTE_LEN);

    std::srand(std::time(nullptr));

    pe.SetTemperament(&t);
    pe.SetScaleMap(&scale);
    pe.SetRootHz(C4FREQ);

    std::cout << "set title 'Density vs Score'" << std::endl;
    std::cout << "set xlabel 'Density'" << std::endl;
    std::cout << "set ylabel 'Score'" << std::endl;
    std::cout << "set grid" << std::endl;

    std::cout << "$MyData << EOD" << std::endl;
    //std::cout << "plot '-' using 2:1 with points" << std::endl;

    GenerateStandardChordEvents(ts, scale, bars, mode, NoteValue::Whole, chords);
    for (int i = 0; i < 1000; i++)
    {
        DoThing();
    }
    std::cout << "EOD" << std::endl;
    std::cout << "plot $MyData using 2:1 with points title 'Density', "
              << "$MyData using 3:1 with points title 'Granularity'" << std::endl;
    //std::cout << "e" << std::endl;

    return 0;
}
