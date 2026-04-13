#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <unistd.h>
#include <string>
#include <iostream>

#include <Monkey.h>
#include <Music/Music.h>
#include <Music/Temperament.h>
#include <Music/Tables.h>
#include <Music/ScaleMaps.h>

using namespace Music;

#define PALETTE Music::AEOLIAN_D12
#define PALETTE_LEN ArrayLen(PALETTE)

TimeSignature      ts;
Temperament        t;
ScaleMap           scale;
PitchEngine        pe;
const HarmonicMode mode = HarmonicMode::Minor;

void doDebug(const char *format, va_list args)
{ std::vprintf(format, args); }

void testThing()
{
    std::cout << std::string(80, '-') << std::endl;
    bool            pattern[128];
    ChordEvent      cEvents[32];
    const int       bars    = 8;
    const float     density = randomRange(0.4f, 0.6f); //0.50f;
    const NoteValue g       = NoteValue::Eighth;

    size_t patternSize
        = generatePattern(ts, bars, density, g, pattern, ArrayLen(pattern));
    // size_t cEventSize = generateChordEventsFromPattern(
    //     pattern, patternSize, g, cEvents, ArrayLen(cEvents));
    size_t cEventSize
        = generateStandardChordEvents(ts, scale, bars, mode, NoteValue::Whole, cEvents, ArrayLen(cEvents));

    // const float density = randomRange(0.2f, 0.9f);
    // const NoteValue g = NoteValue::Eighth;
    // size_t count = Music::generatePattern(ts, bars, density, g, pattern, ArrayLen(pattern));
    debugPattern(ts, g, pattern, patternSize);
    debugChordEvents(t, ts, cEvents, cEventSize);

    for(size_t i = 0; i < cEventSize; i++)
    {
        char noteName[6];
        int periodOffset;

        Degree root = cEvents[i].root;
        t.getNoteLabel(root, noteName, sizeof(noteName));
        DPRINTF("Chord Event %zu: Root %s (Degree %d)", i, noteName);       
        for (int j = 2; j < 5; j++)
        {
            Degree d = scale.mappedDegree(scale.indexOfDegree(root) + j, periodOffset);        
            t.getNoteLabel(d, noteName, sizeof(noteName));
            DPRINTF(", Interval %d: %s (Degree %d)", j, noteName, d);
        }
        DPRINTF("\n");
    }
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

    t.makeEqualDivision(12, 2.0f);
    t.attachNoteLabels(Music::NOTE_NAMES_12, ArrayLen(Music::NOTE_NAMES_12));
    t.attachIntervalLabels(Music::INTERVAL_NAMES_12,
                              ArrayLen(Music::INTERVAL_NAMES_12));

    std::rand();

    scale.setDegrees(PALETTE, PALETTE_LEN);

    pe.setTemperament(&t);
    pe.setScaleMap(&scale);
    pe.setRootHz(261.6256f); // C4

    for(;;)
    {
        testThing();
        sleep(5);
    }
    return 0;
}
