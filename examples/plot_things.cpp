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

void DoThing()
{
    const int bars = 8;
    const float density = randomRange(0.0f, 1.0f); // 0.50f;
    const NoteValue g = NoteValue::Eighth;
    ChordEventSet<> chords;
    PatternEventSet<> pattern;
    NoteEventSet<> noteEvents;
    GenerateStandardChordEvents(ts, scale, bars, mode, NoteValue::Whole, chords);
    GeneratePattern(ts, bars, density, g, pattern);
    size_t eventSize = GenerateEventsFromPattern2(pattern, chords, ts, scale, bars, g, noteEvents);
    NoteEventScore score = ScoreNoteEvents(t, noteEvents);

    std::cout << density << " " << score.overall << std::endl;


    // std::cout << "Event Score: Overall    " << score.overall << std::endl;
    // std::cout << "           : Density    " << score.density << std::endl;
    // std::cout << "           : Rests      " << score.rests << std::endl;
    // std::cout << "           : Cadence    " << score.cadence << std::endl;
    // std::cout << "           : Repetition " << score.repetition << std::endl;
    // std::cout << "           : Contour    " << score.contour << std::endl;
    // std::cout << "           : Phrase     " << score.phrase << std::endl;
    // std::cout << "           : Rhythm     " << score.rhythm << std::endl;
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

    std::cout << "plot '-' using 1:2 with points" << std::endl;

    for (int i = 0; i < 1000; i++)
    {
        DoThing();
    }

    std::cout << "e" << std::endl;

    return 0;
}
