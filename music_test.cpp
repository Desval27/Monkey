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
#include <Music/Labels.h>
#include <Music/ScaleMaps.h>

using namespace Music;

#define PALETTE Music::MIXOLYDIAN_DEGREES_12
#define PALETTE_LEN ArrayLen(PALETTE)

TimeSignature      ts;
Temperament        t;
ScaleMap           scale;
PitchEngine        pe;
const HarmonicMode mode = HarmonicMode::Major;

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
        Note chordTones[10];
        size_t chordToneLen;

        chordToneLen = cEvents[i].getChordTones(scale, chordTones, ArrayLen(chordTones));
        for (size_t j = 0; j < chordToneLen; j++)
        {
            t.getNoteLabel(chordTones[j], noteName, sizeof(noteName));
            DPRINTF("%s ", noteName);
        }
        DPRINTF("\n");
    }
}

int main(int argc, char *argv[])
{
    // Random number seed
    std::srand(std::time(nullptr));

    SET_DEBUG(doDebug);

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
