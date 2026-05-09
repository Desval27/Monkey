
#include <music/music.hpp>

#include "test_framework.hpp"

using music::NoteEventSet;
using music::TimeSignature;
using MyNoteEventSet = NoteEventSet<>;

TEST_CASE("Event size")
{
  TimeSignature ts(4, music::NoteValue::Quarter);
  MyNoteEventSet events;
  CHECK_EQ(events.size(), 0UL); // Should always start out empty
  CHECK_EQ(events.is_empty(), true);

  events.emplace(music::Note_P1, 0, music::NoteValue::Quarter);
  events.emplace(music::Note_P1, 0, music::NoteValue::Quarter);
  events.emplace(music::Note_P1, 0, music::NoteValue::Quarter);
  events.emplace(music::Note_P1, 0, music::NoteValue::Quarter);
  CHECK_EQ(events.size(), 4UL);

  events.clear();
  CHECK_EQ(events.size(), 0UL);
  CHECK_EQ(events.is_empty(), true);
}

TEST_CASE("Bars For Events")
{
  TimeSignature ts(4, music::NoteValue::Quarter);
  MyNoteEventSet events;
  events.emplace(music::Note_P1, 0, music::NoteValue::Whole);
  events.emplace(music::Note_P1, 0, music::NoteValue::Whole);
  events.emplace(music::Note_P1, 0, music::NoteValue::Whole);
  events.emplace(music::Note_P1, 0, music::NoteValue::Whole);
  CHECK_EQ(events.get_bars_for_time_signature(ts), 4);

  TimeSignature ts2(6, music::NoteValue::Eighth);
  events.clear();
  for (int i = 0; i < 12; i++)
    events.emplace(music::Note_P1, 0, music::NoteValue::Eighth);
  CHECK_EQ(events.get_bars_for_time_signature(ts2), 2);
}

TEST_CASE("Total Pulse size")
{
  TimeSignature ts(4, music::NoteValue::Quarter);
  MyNoteEventSet events;
}

TEST_MAIN();
