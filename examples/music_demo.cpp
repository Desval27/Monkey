#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <unistd.h>
#include <string>
#include <iostream>

#include <TTY.h>
#include <Music/Music.h>

#define DO_DEBUG

using namespace Music;

constexpr ScaleType SCALE_TYPE = ScaleType::HEPATONIC;

#define PALETTE PHRYGIAN_D19
#define TEMPERAMENT_DEGREES 19
const HarmonicMode mode = HarmonicMode::Minor;

TimeSignature ts(4,NoteValue::Quarter);
Temperament t;
ScaleMap<SCALE_TYPE> scale;
PitchEngine<SCALE_TYPE> pe;

void doDebug(const char *format, va_list args)
{
    std::vprintf(format, args);
}

void testThing()
{
    std::cout << std::string(80, '-') << std::endl;
    const int bars = 8;
    const float density = randomRange(0.6f, 0.9f); // 0.50f;
    const NoteValue g = GetRandomGranularity(NoteValue::Eighth, NoteValue::Whole);
    std::cout << "Granularity: " << static_cast<int>(g) << " " << GetNoteValueText(g) << std::endl;
    ChordEventSet<SCALE_TYPE> chords;
    PatternEventSet<> pattern;
    NoteEventSet<> noteEvents;

    // Make Chord Progression
    GenerateStandardChordEvents(ts, scale, bars, mode, NoteValue::Whole, chords);
    DebugChordEvents(ts, t, scale, chords);
    std::cout << std::endl;

    // Make "Hit" Pattern
    if (randomRange(0.0f, 1.0f) < 0.5f)
        EuclidianPatternGenerator<>::GeneratePattern(ts, bars, density, g, pattern);
    else
        SimpleRandomPatternGenerator<>::GeneratePattern(ts, bars, density, g, pattern);
    DebugPattern(ts, g, pattern);
    std::cout << std::endl;

    // Make Notes from "Hit" Pattern
    StyleANoteGenerator<SCALE_TYPE, DEFAULT_MAX_EVENTS>::GenerateEvents(pattern, chords, ts, t, scale, bars, g, noteEvents);
    DebugNoteEvents(t, ts, noteEvents);
    std::cout << std::endl;

    NoteEventScore score = ScoreNoteEvents(t, noteEvents);
    std::cout << TTY_FG_MAGENTA << "Event Score" << TTY_RESET << ": Overall    " << score.overall << std::endl;
    std::cout << "           : Density    " << score.density << std::endl;
    std::cout << "           : Rests      " << score.rests << std::endl;
    std::cout << "           : Cadence    " << score.cadence << std::endl;
    std::cout << "           : Repetition " << score.repetition << std::endl;
    std::cout << "           : Contour    " << score.contour << std::endl;
    std::cout << "           : Phrase     " << score.phrase << std::endl;
    std::cout << "           : Rhythm     " << score.rhythm << std::endl;
  std::cout << std::endl;
    
}

int main(int argc, char *argv[])
{
    // Random number seed
    // std::srand(std::time(nullptr));

    SET_DEBUG(doDebug);

    t.MakeEqualDivision(TEMPERAMENT_DEGREES, 2.0f);
#if TEMPERAMENT_DEGREES == 12
    t.AttachNoteLabels(Music::NOTE_NAMES_12);
    t.AttachIntervalLabels(Music::INTERVAL_NAMES_12);
#elif TEMPERAMENT_DEGREES == 15
    t.AttachNoteLabels(Music::NOTE_NAMES_15);
    t.AttachIntervalLabels(Music::INTERVAL_NAMES_15);
#elif TEMPERAMENT_DEGREES == 17
    t.AttachNoteLabels(Music::NOTE_NAMES_17);
    t.AttachIntervalLabels(Music::INTERVAL_NAMES_17);
#elif TEMPERAMENT_DEGREES == 19
    t.AttachNoteLabels(Music::NOTE_NAMES_19);
    t.AttachIntervalLabels(Music::INTERVAL_NAMES_19);
#elif TEMPERAMENT_DEGREES == 22
    t.AttachNoteLabels(Music::NOTE_NAMES_22);
    t.AttachIntervalLabels(Music::INTERVAL_NAMES_22);
#else
#error Unsupported TEMPERAMENT_DEGREES for music_demo
#endif

    scale.SetDegrees(PALETTE);

    pe.SetTemperament(&t);
    pe.SetScaleMap(&scale);
    pe.SetRootHz(C4FREQ); // C4

    for (;;)
    {
        testThing();
        sleep(5);
    }
    return 0;
}
