#include <Music/Gnome.h>

#include "TestFramework.h"

using Music::Gnome;
using Music::NoteValue;
using Music::TimeSignature;

TEST_CASE("Gnome starts in pre-roll and enters beat zero on first pulse")
{
  TimeSignature ts(4, NoteValue::Quarter);
  Gnome gnome(ts, 2);

  CHECK_EQ(gnome.GetBar(), 0);
  CHECK_EQ(gnome.GetBeat(), -1);
  CHECK_EQ(gnome.GetPulse(), -1);
  CHECK(!gnome.RisingBeatEdge());
  CHECK(!gnome.RisingBarEdge());

  CHECK_EQ(gnome.DoPulse(), 0);
  CHECK_EQ(gnome.GetBar(), 0);
  CHECK_EQ(gnome.GetBeat(), 0);
  CHECK_EQ(gnome.GetPulse(), 0);
  CHECK(gnome.RisingBeatEdge());
  CHECK(gnome.RisingBarEdge());
}

TEST_CASE("Gnome beat edge fires once per beat")
{
  TimeSignature ts(4, NoteValue::Quarter);
  Gnome gnome(ts, 1);
  const int pulsesPerBeat = static_cast<int>(NoteValue::Quarter);

  CHECK_EQ(gnome.DoPulse(), 0);
  CHECK(gnome.RisingBeatEdge());
  CHECK(gnome.RisingBarEdge());

  CHECK_EQ(gnome.DoPulse(), 1);
  CHECK(!gnome.RisingBeatEdge());
  CHECK(!gnome.RisingBarEdge());
  CHECK_EQ(gnome.GetBeat(), 0);

  for (int i = 2; i < pulsesPerBeat; ++i) {
    CHECK_EQ(gnome.DoPulse(), i);
    CHECK(!gnome.RisingBeatEdge());
    CHECK(!gnome.RisingBarEdge());
  }

  CHECK_EQ(gnome.DoPulse(), pulsesPerBeat);
  CHECK(gnome.RisingBeatEdge());
  CHECK(!gnome.RisingBarEdge());
  CHECK_EQ(gnome.GetBeat(), 1);
}

TEST_CASE("Gnome wraps to the start without losing phase")
{
  TimeSignature ts(2, NoteValue::Eighth);
  Gnome gnome(ts, 2);
  const int pulsesPerBeat = static_cast<int>(NoteValue::Eighth);
  const int totalLoopPulses = 2 * 2 * pulsesPerBeat;

  CHECK_EQ(gnome.DoPulse(), 0);
  CHECK_EQ(gnome.GetBar(), 0);
  CHECK_EQ(gnome.GetBeat(), 0);

  for (int i = 1; i < pulsesPerBeat; ++i) {
    CHECK_EQ(gnome.DoPulse(), i);
    CHECK_EQ(gnome.GetBar(), 0);
    CHECK_EQ(gnome.GetBeat(), 0);
  }

  CHECK_EQ(gnome.DoPulse(), pulsesPerBeat);
  CHECK_EQ(gnome.GetBar(), 0);
  CHECK_EQ(gnome.GetBeat(), 1);
  CHECK(gnome.RisingBeatEdge());
  CHECK(!gnome.RisingBarEdge());

  for (int i = pulsesPerBeat + 1; i < totalLoopPulses; ++i) {
    CHECK_EQ(gnome.DoPulse(), i);
  }

  CHECK_EQ(gnome.DoPulse(), 0);
  CHECK_EQ(gnome.GetBar(), 0);
  CHECK_EQ(gnome.GetBeat(), 0);
  CHECK_EQ(gnome.GetPulse(), 0);
  CHECK(gnome.RisingBeatEdge());
  CHECK(gnome.RisingBarEdge());
}

TEST_CASE("Gnome Reset restores the pre-roll state")
{
  TimeSignature ts(3, NoteValue::Quarter);
  Gnome gnome(ts, 2);

  CHECK_EQ(gnome.DoPulse(), 0);
  CHECK_EQ(gnome.DoPulse(), 1);

  gnome.Reset();

  CHECK_EQ(gnome.GetBar(), 0);
  CHECK_EQ(gnome.GetBeat(), -1);
  CHECK_EQ(gnome.GetPulse(), -1);
  CHECK(!gnome.RisingBeatEdge());
  CHECK(!gnome.RisingBarEdge());

  CHECK_EQ(gnome.DoPulse(), 0);
  CHECK_EQ(gnome.GetBar(), 0);
  CHECK_EQ(gnome.GetBeat(), 0);
  CHECK(gnome.RisingBarEdge());
}

TEST_CASE("Gnome invalid meter stays idle")
{
  TimeSignature zeroBeats(0, NoteValue::Quarter);
  Gnome noBeats(zeroBeats, 1);

  CHECK_EQ(noBeats.DoPulse(), -1);
  CHECK_EQ(noBeats.GetBar(), 0);
  CHECK_EQ(noBeats.GetBeat(), -1);
  CHECK_EQ(noBeats.GetPulse(), -1);
  CHECK(!noBeats.RisingBeatEdge());
  CHECK(!noBeats.RisingBarEdge());

  TimeSignature noValue(4, NoteValue::None);
  Gnome noBeatValue(noValue, 1);

  CHECK_EQ(noBeatValue.DoPulse(), -1);
  CHECK_EQ(noBeatValue.GetBeat(), -1);

  TimeSignature validTs(4, NoteValue::Quarter);
  Gnome noBars(validTs, 0);

  CHECK_EQ(noBars.DoPulse(), -1);
  CHECK_EQ(noBars.GetPulse(), -1);
}

TEST_MAIN();
