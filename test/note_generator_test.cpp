#include <music/note_generator.hpp>

#include "test_framework.hpp"

using MySetup = music::Setup<>;
using MyNoteEventSet = music::NoteEventSet<>;
using MyPatternModifier = music::PatternModifier<>;

TEST_CASE("Repeat two replaces second half with first half")
{
  MySetup setup;
  setup.bars = 2;

  MyNoteEventSet events;
  events.emplace(music::Note_P1, 0, music::NoteValue::Quarter);
  events.emplace(music::Note_M2, 0, music::NoteValue::Quarter);
  events.emplace(music::Note_M3, 0, music::NoteValue::Half);
  events.emplace(music::Note_P4, 0, music::NoteValue::Whole);

  CHECK_EQ(MyPatternModifier::repeat_two(setup, events), 6UL);
  CHECK_EQ(events.get_total_event_pulses(),
           setup.bars * setup.timeSignature.get_pulses_per_bar());

  CHECK_EQ(events[0].note, music::Note_P1);
  CHECK_EQ(events[0].value, music::NoteValue::Quarter);
  CHECK_EQ(events[1].note, music::Note_M2);
  CHECK_EQ(events[1].value, music::NoteValue::Quarter);
  CHECK_EQ(events[2].note, music::Note_M3);
  CHECK_EQ(events[2].value, music::NoteValue::Half);
  CHECK_EQ(events[3].note, music::Note_P1);
  CHECK_EQ(events[3].value, music::NoteValue::Quarter);
  CHECK_EQ(events[4].note, music::Note_M2);
  CHECK_EQ(events[4].value, music::NoteValue::Quarter);
  CHECK_EQ(events[5].note, music::Note_M3);
  CHECK_EQ(events[5].value, music::NoteValue::Half);
}

TEST_CASE("Repeat two splits event crossing middle bar line")
{
  MySetup setup;
  setup.bars = 2;

  MyNoteEventSet events;
  events.emplace(music::Note_P1, 0, music::NoteValue::Half);
  events.emplace(music::Note_M2, 0, music::NoteValue::Whole);
  events.emplace(music::Note_M3, 0, music::NoteValue::Half);

  CHECK_EQ(MyPatternModifier::repeat_two(setup, events), 4UL);
  CHECK_EQ(events.get_total_event_pulses(),
           setup.bars * setup.timeSignature.get_pulses_per_bar());

  CHECK_EQ(events[0].note, music::Note_P1);
  CHECK_EQ(events[0].value, music::NoteValue::Half);
  CHECK_EQ(events[1].note, music::Note_M2);
  CHECK_EQ(events[1].value, music::NoteValue::Half);
  CHECK_EQ(events[2].note, music::Note_P1);
  CHECK_EQ(events[2].value, music::NoteValue::Half);
  CHECK_EQ(events[3].note, music::Note_M2);
  CHECK_EQ(events[3].value, music::NoteValue::Half);
}

TEST_MAIN();
