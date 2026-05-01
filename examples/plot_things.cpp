#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <unistd.h>
#include <string>
#include <iostream>

#include <Music/Music.h>

using namespace Music;

constexpr ScaleType SCALE_TYPE = ScaleType::HEPATONIC;

#define PALETTE IONIAN_D15
#define PALETTE_LEN ArrayLen(PALETTE)

Setup<DEF_MAX_DEGREES, SCALE_TYPE> setup;
ChordEventSet<DEF_MAX_DEGREES, SCALE_TYPE, DEF_MAX_EVENTS> chords;

void DoThing()
{
    const float density = randomRange(0.0f, 1.0f); // 0.50f;
    const NoteValue g = GetRandomGranularity(NoteValue::Sixteenth, NoteValue::Whole);
    PatternEventSet<> pattern;
    NoteEventSet<> noteEvents;

    EuclidianPatternGenerator<>::GeneratePattern(setup.timeSignature, setup.bars, density, g, pattern);

    StyleANoteGenerator<DEF_MAX_DEGREES, SCALE_TYPE, DEF_MAX_EVENTS>::GenerateEvents(setup, pattern, chords, g, SCALE_WEIGHTS_7_UNIFORM, noteEvents);
    NoteEventScore score = ScoreNoteEvents(setup.temperament, noteEvents);

    float v1 = density;
    float v2 = score.rests / 100.0;

    std::cout << score.overall << '\t' << v1 << '\t' << v2 << std::endl;
}

int main(int argc, char *argv[])
{
    setup.temperament.AttachNoteLabels(Music::NOTE_NAMES_12); 
    setup.temperament.AttachIntervalLabels(Music::INTERVAL_NAMES_12);
    setup.scaleMap.SetDegrees(Music::IONIAN_D12);

    std::cout << "set title 'Density/Granularity vs Score'" << std::endl;
    std::cout << "set xlabel 'Density/Granularity'" << std::endl;
    std::cout << "set ylabel 'Score'" << std::endl;
    std::cout << "set grid" << std::endl;

    std::cout << "$MyData << EOD" << std::endl;
    //std::cout << "plot '-' using 2:1 with points" << std::endl;

    GenerateStandardChordEvents(setup, NoteValue::Whole, chords);
    for (int i = 0; i < 5000; i++)
    {
        DoThing();
    }
    std::cout << "EOD" << std::endl;
    std::cout << "plot $MyData using 2:1 with points title 'Density', "
              << "$MyData using 3:1 with points title 'Granularity'" << std::endl;
    //std::cout << "e" << std::endl;

    return 0;
}
