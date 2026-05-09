#include <music/gnome.hpp>

#include "test_framework.hpp"

using music::Gnome;
using music::NoteValue;
using music::TimeSignature;

TEST_CASE("Gnome starts in pre-roll and enters beat zero on first pulse")
{
  TimeSignature ts(4, NoteValue::Quarter);
  Gnome gnome(ts, 2);

  CHECK_EQ(gnome.get_bar(), 0);
  CHECK_EQ(gnome.get_beat(), -1);
  CHECK_EQ(gnome.get_pulse(), -1);
  CHECK(!gnome.rising_beat_edge());
  CHECK(!gnome.rising_bar_edge());

  CHECK_EQ(gnome.do_pulse(), 0);
  CHECK_EQ(gnome.get_bar(), 0);
  CHECK_EQ(gnome.get_beat(), 0);
  CHECK_EQ(gnome.get_pulse(), 0);
  CHECK(gnome.rising_beat_edge());
  CHECK(gnome.rising_bar_edge());
}

TEST_CASE("Gnome beat edge fires once per beat")
{
  TimeSignature ts(4, NoteValue::Quarter);
  Gnome gnome(ts, 1);
  const int pulsesPerBeat = static_cast<int>(NoteValue::Quarter);

  CHECK_EQ(gnome.do_pulse(), 0);
  CHECK(gnome.rising_beat_edge());
  CHECK(gnome.rising_bar_edge());

  CHECK_EQ(gnome.do_pulse(), 1);
  CHECK(!gnome.rising_beat_edge());
  CHECK(!gnome.rising_bar_edge());
  CHECK_EQ(gnome.get_beat(), 0);

  for (int i = 2; i < pulsesPerBeat; ++i) {
    CHECK_EQ(gnome.do_pulse(), i);
    CHECK(!gnome.rising_beat_edge());
    CHECK(!gnome.rising_bar_edge());
  }

  CHECK_EQ(gnome.do_pulse(), pulsesPerBeat);
  CHECK(gnome.rising_beat_edge());
  CHECK(!gnome.rising_bar_edge());
  CHECK_EQ(gnome.get_beat(), 1);
}

TEST_CASE("Gnome wraps to the start without losing phase")
{
  TimeSignature ts(2, NoteValue::Eighth);
  Gnome gnome(ts, 2);
  const int pulsesPerBeat = static_cast<int>(NoteValue::Eighth);
  const int totalLoopPulses = 2 * 2 * pulsesPerBeat;

  CHECK_EQ(gnome.do_pulse(), 0);
  CHECK_EQ(gnome.get_bar(), 0);
  CHECK_EQ(gnome.get_beat(), 0);

  for (int i = 1; i < pulsesPerBeat; ++i) {
    CHECK_EQ(gnome.do_pulse(), i);
    CHECK_EQ(gnome.get_bar(), 0);
    CHECK_EQ(gnome.get_beat(), 0);
  }

  CHECK_EQ(gnome.do_pulse(), pulsesPerBeat);
  CHECK_EQ(gnome.get_bar(), 0);
  CHECK_EQ(gnome.get_beat(), 1);
  CHECK(gnome.rising_beat_edge());
  CHECK(!gnome.rising_bar_edge());

  for (int i = pulsesPerBeat + 1; i < totalLoopPulses; ++i) {
    CHECK_EQ(gnome.do_pulse(), i);
  }

  CHECK_EQ(gnome.do_pulse(), 0);
  CHECK_EQ(gnome.get_bar(), 0);
  CHECK_EQ(gnome.get_beat(), 0);
  CHECK_EQ(gnome.get_pulse(), 0);
  CHECK(gnome.rising_beat_edge());
  CHECK(gnome.rising_bar_edge());
}

TEST_CASE("Gnome reset restores the pre-roll state")
{
  TimeSignature ts(3, NoteValue::Quarter);
  Gnome gnome(ts, 2);

  CHECK_EQ(gnome.do_pulse(), 0);
  CHECK_EQ(gnome.do_pulse(), 1);

  gnome.reset();

  CHECK_EQ(gnome.get_bar(), 0);
  CHECK_EQ(gnome.get_beat(), -1);
  CHECK_EQ(gnome.get_pulse(), -1);
  CHECK(!gnome.rising_beat_edge());
  CHECK(!gnome.rising_bar_edge());

  CHECK_EQ(gnome.do_pulse(), 0);
  CHECK_EQ(gnome.get_bar(), 0);
  CHECK_EQ(gnome.get_beat(), 0);
  CHECK(gnome.rising_bar_edge());
}

TEST_CASE("Gnome invalid meter stays idle")
{
  TimeSignature zeroBeats(0, NoteValue::Quarter);
  Gnome noBeats(zeroBeats, 1);

  CHECK_EQ(noBeats.do_pulse(), -1);
  CHECK_EQ(noBeats.get_bar(), 0);
  CHECK_EQ(noBeats.get_beat(), -1);
  CHECK_EQ(noBeats.get_pulse(), -1);
  CHECK(!noBeats.rising_beat_edge());
  CHECK(!noBeats.rising_bar_edge());

  TimeSignature noValue(4, NoteValue::None);
  Gnome noBeatValue(noValue, 1);

  CHECK_EQ(noBeatValue.do_pulse(), -1);
  CHECK_EQ(noBeatValue.get_beat(), -1);

  TimeSignature validTs(4, NoteValue::Quarter);
  Gnome noBars(validTs, 0);

  CHECK_EQ(noBars.do_pulse(), -1);
  CHECK_EQ(noBars.get_pulse(), -1);
}

TEST_MAIN();
