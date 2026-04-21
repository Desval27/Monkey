#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <unistd.h>
#include <string>
#include <iostream>

#include <Music/Music.h>

#define DO_DEBUG

using namespace Music;

#define PALETTE IONIAN_D15
#define PALETTE_LEN ArrayLen(PALETTE)

TimeSignature ts;
Temperament t;
ScaleMap scale;
PitchEngine pe;
const HarmonicMode mode = HarmonicMode::Major;

void doDebug(const char *format, va_list args)
{
    std::vprintf(format, args);
}

void testThing()
{
    std::cout << std::string(80, '-') << std::endl;
    const int bars = 8;
    const float density = randomRange(0.2f, 0.8f); // 0.50f;
    const NoteValue g = NoteValue::Eighth;
    ChordEventSet<> chords;
    PatternEventSet<> pattern;
    NoteEventSet<> noteEvents;

    GenerateStandardChordEvents(ts, scale, bars, mode, NoteValue::Whole, chords);
    std::cout << "Chord Events" << std::endl;
    DebugChordEvents(t, ts, chords);
    std::cout << std::endl;

    GeneratePattern(ts, bars, density, g, pattern);
    std::cout << "Pattern Events" << std::endl;
    DebugPattern(ts, g, pattern);
    std::cout << std::endl;

    size_t eventSize = GenerateEventsFromPattern(pattern, chords, ts, scale, bars, g, noteEvents);
    std::cout << "Note Events" << std::endl;
    DebugNoteEvents(t, ts, noteEvents);
    std::cout << std::endl;
}

int main(int argc, char *argv[])
{
    // Random number seed
    std::srand(std::time(nullptr));

    SET_DEBUG(doDebug);

    // for (size_t i = 0; i < NUM_SCALES; i++)
    // {
    //     const auto& scaleTable = Music::SCALE_TABLES[i];
    //     std::cout << "Scale: " << scaleTable.name
    //               << " (Degrees in Period: " << scaleTable.degreesInPeriod
    //               << " (Degrees in Scale: " << scaleTable.degreesInScale
    //               << " (Minor: " << (scaleTable.isMinor() ? "Yes" : "No") << ")"
    //               << std::endl;
    // }

    t.MakeEqualDivision(15, 2.0f);
    t.AttachNoteLabels(Music::NOTE_NAMES_15, ArrayLen(Music::NOTE_NAMES_15));
    t.AttachIntervalLabels(Music::INTERVAL_NAMES_15,
                           ArrayLen(Music::INTERVAL_NAMES_15));

    scale.SetDegrees(PALETTE, PALETTE_LEN);

    std::srand(std::time(nullptr));

    pe.SetTemperament(&t);
    pe.SetScaleMap(&scale);
    pe.SetRootHz(261.6256f); // C4

    for (;;)
    {
        testThing();
        sleep(5);
    }
    return 0;
}
