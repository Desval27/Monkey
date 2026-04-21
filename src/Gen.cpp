#include <Music/Music.h>

namespace Music
{
  //////////////////////////////////////////////////////////////////////////////////////////////////
  /// @brief 
  /// @param ts 
  /// @param scale 
  /// @param bars 
  /// @param mode 
  /// @param granularity 
  /// @param chords 
  /// @return 
  size_t GenerateStandardChordEvents(const TimeSignature &ts,
                                     const ScaleMap &scale,
                                     int bars,
                                     HarmonicMode mode,
                                     NoteValue granularity,
                                     ChordEventSet<> &chords)
  {
    return GenerateWeightedChordEvents(ts, scale, bars, mode, 0, granularity, chords);
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  /// @brief 
  /// @param ts 
  /// @param scale 
  /// @param bars 
  /// @param mode 
  /// @param tonic 
  /// @param value 
  /// @param chords 
  /// @return 
  size_t GenerateWeightedChordEvents(const TimeSignature &ts,
                                     const ScaleMap &scale,
                                     int bars,
                                     HarmonicMode mode,
                                     Note tonic,
                                     NoteValue value,
                                     ChordEventSet<> &chords)
  {
    (void)ts;
    if (chords.Capacity() == 0 || bars <= 0 || value == NoteValue::None)
      return 0;

    const size_t barsToEmit = min(static_cast<size_t>(bars), chords.Capacity());
    ScaleDegree degree = GetWeightedStartingChord(mode);
    const bool hasScale = (scale.Count() >= SCALE_CHORD_COUNT);

    chords.Clear();
    for (size_t i = 0; i < barsToEmit; ++i)
    {
      const int degreeIndex = ScaleDegreeIndex(degree, mode);
      Note rootOffset = ScaleDegreeToSemitone(degree);
      if (hasScale && degreeIndex >= 0)
      {
        int scalePeriodOffset = 0;
        rootOffset =
            static_cast<Note>(scale.GetMappedDegree(degreeIndex, scalePeriodOffset));
      }
      chords.Emplace(tonic + rootOffset, value);

      degree = GetWeightedNextChord(degree, mode);
    }

    return barsToEmit;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////
  /// @brief 
  /// @param pattern 
  /// @param granularity 
  /// @param events 
  /// @return 
  size_t GenerateChordEventsFromPattern(const PatternEventSet<> &pattern,
                                        NoteValue granularity,
                                        ChordEventSet<> &events)
  {

    if (pattern.Count() || granularity == NoteValue::None)
      return 0;

    size_t maxSize = min(pattern.Count(), events.Count());

    size_t eventCount = 0;
    size_t idx = 0;
    // eventsOut[eventCount].root =
    //     REST; // By default; normal would be first event is a hit but this allows
    //           // for a "gap" event if not.
    // eventsOut[eventCount].value = NoteValue::None;
    // while (idx < maxSize)
    // {
    //   if (pattern[idx])
    //   {
    //     if (idx > 0)
    //       eventCount++;

    //     switch (eventCount % 8)
    //     {
    //     case 0:
    //       eventsOut[eventCount].root = 0;
    //       break;
    //     case 1:
    //       eventsOut[eventCount].root = 5;
    //       break;
    //     case 2:
    //       eventsOut[eventCount].root = 7;
    //       break;
    //     case 3:
    //       eventsOut[eventCount].root = 5;
    //       break;
    //     case 4:
    //       eventsOut[eventCount].root = 0;
    //       break;
    //     case 5:
    //       eventsOut[eventCount].root = 7;
    //       break;
    //     case 6:
    //       eventsOut[eventCount].root = 5;
    //       break;
    //     case 7:
    //       eventsOut[eventCount].root = 7;
    //       break;
    //     default:
    //       eventsOut[eventCount].root = REST;
    //       break;
    //     }
    //     eventsOut[eventCount].value = granularity;
    //   }
    //   else
    //   {
    //     eventsOut[eventCount].value = eventsOut[eventCount].value + granularity;
    //   }
    //   idx++;
    // }
    return events.Count();
  }


  //////////////////////////////////////////////////////////////////////////////////////////////////
  /// @brief 
  /// @param pattern 
  /// @param chords 
  /// @param ts 
  /// @param bars 
  /// @param granularity 
  /// @param events 
  /// @return 
  size_t GenerateEventsFromPattern(const PatternEventSet<> &pattern,                                  
                                   const ChordEventSet<> &chords,
                                   const TimeSignature &ts,
                                   const ScaleMap &scale,
                                   int bars,
                                   NoteValue granularity,
                                   NoteEventSet<> &events)
  {
    events.Clear();
    if (chords.Count() == 0)
      return 0;

    // Direct one-to-one copy of events to notes
    int pulses = 0;
    int chordPulses = 0;
    int ppb = ts.PulsesPerBar();
    size_t chordIdx = 0;

    Note tones[20];
    size_t toneCount = chords[chordIdx].GetChordTones(scale, tones, sizeof(tones));
    
    for (size_t i = 0; i < pattern.Count() && !events.AtCapacity(); i++, pulses = pulses + granularity, chordPulses = chordPulses + granularity)
    {
      if (chordPulses >= static_cast<int>(chords[chordIdx].value) && chordIdx < chords.Count() - 1)
      {
        chordIdx++;
        chordPulses = 0;

        toneCount = chords[chordIdx].GetChordTones(scale, tones, sizeof(tones));
      }

      if (pattern[i])
      {      
        // Is this at the start of a bar?
        if (pulses % ppb == 0)
          events.Emplace(tones[0], 0, granularity);
        else
        {
          size_t idx = randomRange((size_t)0, toneCount);
          events.Emplace(tones[idx], 0, granularity);
        }
      }
      else
      {
        // If our last event was also a rest then just add to its value
        // Unless the new event is at the start of a bar
        if (pulses % ppb != 0 && events.Count() > 1 && events[events.Count()-1].note == REST)
          events[events.Count()-1].value += granularity;
        else
          events.Emplace(REST, 0, granularity);
      }
    }
    return events.Count();
  }

} // namespace Music
