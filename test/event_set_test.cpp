
#include <Music/Music.h>

#include "TestFramework.h"

using Music::TimeSignature;
using Music::NoteEventSet;
using MyNotEventSet = NoteEventSet<>;

TEST_CASE("Event Count") {
    TimeSignature ts(4, Music::NoteValue::Quarter);
    MyNotEventSet events;
    CHECK_EQ(events.Count(), 0UL);   // Should always start out empty
    CHECK_EQ(events.IsEmpty(), true);

    events.Emplace(Music::Note_P1, 0, Music::NoteValue::Quarter);
    events.Emplace(Music::Note_P1, 0, Music::NoteValue::Quarter);
    events.Emplace(Music::Note_P1, 0, Music::NoteValue::Quarter);
    events.Emplace(Music::Note_P1, 0, Music::NoteValue::Quarter);
    CHECK_EQ(events.Count(), 4UL); 

    events.Clear();
    CHECK_EQ(events.Count(), 0UL); 
    CHECK_EQ(events.IsEmpty(), true);
}

TEST_CASE("Bars For Events")
{
    TimeSignature ts(4, Music::NoteValue::Quarter);
    MyNotEventSet events;
    events.Emplace(Music::Note_P1, 0, Music::NoteValue::Whole);
    events.Emplace(Music::Note_P1, 0, Music::NoteValue::Whole);
    events.Emplace(Music::Note_P1, 0, Music::NoteValue::Whole);
    events.Emplace(Music::Note_P1, 0, Music::NoteValue::Whole);
    CHECK_EQ(events.GetBarsForTimeSignature(ts), 4);

    TimeSignature ts2(6, Music::NoteValue::Eighth);
    events.Clear();
    for (int i = 0; i < 12; i++)
        events.Emplace(Music::Note_P1, 0, Music::NoteValue::Eighth);
    CHECK_EQ(events.GetBarsForTimeSignature(ts2), 2);
}

TEST_CASE("Total Pulse Count") {
    TimeSignature ts(4, Music::NoteValue::Quarter);
    MyNotEventSet events;
    
}

TEST_MAIN();
