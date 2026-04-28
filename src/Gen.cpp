#include <Music/Music.h>

namespace Music
{
  ////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param ts
  /// @param scale
  /// @param bars
  /// @param mode
  /// @param granularity
  /// @param chords
  /// @return
  template <std::size_t SCALE_DEGREES, std::size_t MAX_EVENTS>
  std::size_t GenerateStandardChordEvents(const TimeSignature &ts,
                                     const ScaleMap<SCALE_DEGREES> &scale,
                                     int bars,
                                     HarmonicMode mode,
                                     NoteValue granularity,
                                     ChordEventSet<SCALE_DEGREES, MAX_EVENTS> &chords)
  {
    return GenerateWeightedChordEvents(ts, scale, bars, mode, 0, granularity, chords);
  }

  ////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param ts
  /// @param scale
  /// @param bars
  /// @param mode
  /// @param tonic
  /// @param value
  /// @param chords
  /// @return
  template <std::size_t SCALE_DEGREES, std::size_t MAX_EVENTS>
  std::size_t GenerateWeightedChordEvents(const TimeSignature &ts,
                                     const ScaleMap<SCALE_DEGREES> &scale,
                                     int bars,
                                     HarmonicMode mode,
                                     Note tonic,
                                     NoteValue value,
                                     ChordEventSet<SCALE_DEGREES, MAX_EVENTS> &chords)
  {
    (void)ts;
    if (chords.Capacity() == 0 || bars <= 0 || value == NoteValue::None)
      return 0;

    const std::size_t totalEvents = 
      (static_cast<std::size_t>(bars) * static_cast<std::size_t>(ts.beats) * static_cast<std::size_t>(ts.beatValue)) / static_cast<std::size_t>(value);
    const std::size_t chordsToEmit = min(totalEvents, chords.Capacity());
    ScaleDegree degree = GetWeightedStartingChord(mode);
    const bool hasScale = (scale.Count() >= SCALE_CHORD_COUNT);

    chords.Clear();
    for (std::size_t i = 0; i < chordsToEmit; ++i)
    {
      const int degreeIndex = ScaleDegreeIndex(degree, mode);
      Note rootOffset = ScaleDegreeToSemitone(degree);
      if (hasScale && degreeIndex >= 0)
      {
        int scalePeriodOffset = 0;
        rootOffset =
            static_cast<Note>(scale.GetMappedDegree(degreeIndex, scalePeriodOffset));
      }
      // Auto 7th V for testing
      ChordExtension extensions = (degree == ScaleDegree::V) ? ChordExtension::Seventh : Music::ChordExtension{}; 
      chords.Emplace(tonic + rootOffset, value, extensions);

      degree = GetWeightedNextChord(degree, mode);
    }

    return chords.Count();
  }

  ////////////////////////////////////////////////////////////////////////////
  /// @brief
  /// @param pattern
  /// @param chords
  /// @param ts
  /// @param bars
  /// @param granularity
  /// @param events
  /// @return
  template <std::size_t MAX_DEGREES, std::size_t SCALE_DEGREES, std::size_t MAX_EVENTS>
  std::size_t GenerateEventsFromPattern(const PatternEventSet<MAX_EVENTS> &pattern,
                                   const ChordEventSet<SCALE_DEGREES, MAX_EVENTS> &chords,
                                   const TimeSignature &ts,
                                   const Temperament<MAX_DEGREES> &temperament,
                                   const ScaleMap<SCALE_DEGREES> &scale,
                                   int bars,
                                   NoteValue granularity,
                                   NoteEventSet<MAX_EVENTS> &events)
  {
    events.Clear();
    if (chords.Count() == 0)
      return 0;

    // Direct one-to-one copy of events to notes
    int pulses = 0;
    int chordPulses = 0;
    int ppb = ts.GetPulsesPerBar();
    std::size_t chordIdx = 0;

    Note tones[20];
    std::size_t toneCount = chords[chordIdx].GetChordTones(
        scale, static_cast<int>(temperament.DegreesPerPeriod()), tones,
        ArrayLen(tones));

    for (std::size_t i = 0; i < pattern.Count() && !events.AtCapacity(); i++, pulses = pulses + granularity, chordPulses = chordPulses + granularity)
    {
      if (chordPulses >= static_cast<int>(chords[chordIdx].value) && chordIdx < chords.Count() - 1)
      {
        chordIdx++;
        chordPulses = 0;

        toneCount = chords[chordIdx].GetChordTones(
            scale, static_cast<int>(temperament.DegreesPerPeriod()), tones,
            ArrayLen(tones));
      }

      if (pattern[i])
      {
        // Is this at the start of a bar?  Then start with a chord
        if (pulses % ppb == 0)
          events.Emplace(tones[0], 0, granularity);
        else
        {
          int idx = randomRange(0, static_cast<int>(toneCount)-1);
          events.Emplace(tones[idx], 0, granularity);
        }
      }
      else
      {
        // If our last event was also a rest then just add to its value
        // Unless the new event is at the start of a bar
        if (pulses % ppb != 0 && events.Count() > 1 && events[events.Count() - 1].note == REST)
          events[events.Count() - 1].value += granularity;
        else
          events.Emplace(REST, 0, granularity);
      }
    }
    return events.Count();
  }

  template std::size_t GenerateStandardChordEvents<DEF_SCALE_DEGREES, DEF_MAX_EVENTS>(
      const TimeSignature &ts,
      const ScaleMap<DEF_SCALE_DEGREES> &scale,
      int bars,
      HarmonicMode mode,
      NoteValue granularity,
      ChordEventSet<DEF_SCALE_DEGREES, DEF_MAX_EVENTS> &chords);

  template std::size_t GenerateWeightedChordEvents<DEF_SCALE_DEGREES, DEF_MAX_EVENTS>(
      const TimeSignature &ts,
      const ScaleMap<DEF_SCALE_DEGREES> &scale,
      int bars,
      HarmonicMode mode,
      Note tonic,
      NoteValue value,
      ChordEventSet<DEF_SCALE_DEGREES, DEF_MAX_EVENTS> &chords);

} // namespace Music
