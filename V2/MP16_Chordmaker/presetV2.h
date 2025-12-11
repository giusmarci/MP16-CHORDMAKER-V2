#ifndef PRESETS_V2_H
#define PRESETS_V2_H

// V2 Chord Structure - 8 notes per chord
struct ChordV2 {
  int rootOffset = 0;              // Semitones from global root
  int intervals[8] = {0};          // Note intervals in semitones
  int octaveModifiers[8] = {0};    // Octave shift per note (-3 to +3)
  int velocityModifiers[8] = {0};  // Velocity offset per note
  bool isActive[8] = {false};      // Which notes are enabled
  int channel[8] = {0};            // MIDI channel per note (0-3 = A-D)
};

// V2 Pad Structure
struct PadV2 {
  uint32_t color = 0xFFFFFF;
  int triggerNote = 60;            // MIDI note that triggers this pad
  int velocity = 100;              // Base velocity
  int velocityVariation = 0;       // Random velocity range
  ChordV2 chord;
};

// 9 Default Chord Presets for V2
// Musical progression: I - ii - iii - IV - V - vi - vii° - I(8ve) - V/V
// In C Major: C - Dm - Em - F - G - Am - Bdim - C(8ve) - D

ChordV2 presetV2[9] = {
  // Chord 1: C Major (I) - Rich voicing
  {
    .rootOffset = 0,
    .intervals = {0, 4, 7, 12, 16, 19, 24, 0},
    .octaveModifiers = {0, 0, 0, 0, 0, 0, 0, -1},
    .velocityModifiers = {0, -5, -5, 0, -10, -10, -15, 10},
    .isActive = {true, true, true, true, false, false, false, true},
    .channel = {0, 0, 0, 0, 0, 0, 0, 0}
  },

  // Chord 2: D Minor (ii)
  {
    .rootOffset = 2,
    .intervals = {0, 3, 7, 12, 15, 19, 24, 0},
    .octaveModifiers = {0, 0, 0, 0, 0, 0, 0, -1},
    .velocityModifiers = {0, -5, -5, 0, -10, -10, -15, 10},
    .isActive = {true, true, true, true, false, false, false, false},
    .channel = {0, 0, 0, 0, 0, 0, 0, 0}
  },

  // Chord 3: E Minor (iii)
  {
    .rootOffset = 4,
    .intervals = {0, 3, 7, 12, 15, 19, 24, 0},
    .octaveModifiers = {0, 0, 0, 0, 0, 0, 0, -1},
    .velocityModifiers = {0, -5, -5, 0, -10, -10, -15, 10},
    .isActive = {true, true, true, true, false, false, false, false},
    .channel = {0, 0, 0, 0, 0, 0, 0, 0}
  },

  // Chord 4: F Major (IV)
  {
    .rootOffset = 5,
    .intervals = {0, 4, 7, 12, 16, 19, 24, 0},
    .octaveModifiers = {0, 0, 0, 0, 0, 0, 0, -1},
    .velocityModifiers = {0, -5, -5, 0, -10, -10, -15, 10},
    .isActive = {true, true, true, true, false, false, false, false},
    .channel = {0, 0, 0, 0, 0, 0, 0, 0}
  },

  // Chord 5: G Major (V) - Dominant
  {
    .rootOffset = 7,
    .intervals = {0, 4, 7, 12, 16, 19, 24, 0},
    .octaveModifiers = {0, 0, 0, 0, 0, 0, 0, -1},
    .velocityModifiers = {0, -5, -5, 0, -10, -10, -15, 10},
    .isActive = {true, true, true, true, false, false, false, false},
    .channel = {0, 0, 0, 0, 0, 0, 0, 0}
  },

  // Chord 6: A Minor (vi)
  {
    .rootOffset = 9,
    .intervals = {0, 3, 7, 12, 15, 19, 24, 0},
    .octaveModifiers = {0, 0, 0, 0, 0, 0, 0, -1},
    .velocityModifiers = {0, -5, -5, 0, -10, -10, -15, 10},
    .isActive = {true, true, true, true, false, false, false, false},
    .channel = {0, 0, 0, 0, 0, 0, 0, 0}
  },

  // Chord 7: B Diminished (vii°)
  {
    .rootOffset = 11,
    .intervals = {0, 3, 6, 12, 15, 18, 24, 0},
    .octaveModifiers = {0, 0, 0, 0, 0, 0, 0, -1},
    .velocityModifiers = {0, -5, -5, 0, -10, -10, -15, 10},
    .isActive = {true, true, true, false, false, false, false, false},
    .channel = {0, 0, 0, 0, 0, 0, 0, 0}
  },

  // Chord 8: G7 (V7) - Dominant 7th for resolution
  {
    .rootOffset = 7,
    .intervals = {0, 4, 7, 10, 12, 16, 19, 0},
    .octaveModifiers = {0, 0, 0, 0, 0, 0, 0, -1},
    .velocityModifiers = {0, -5, -5, -5, 0, -10, -10, 10},
    .isActive = {true, true, true, true, false, false, false, false},
    .channel = {0, 0, 0, 0, 0, 0, 0, 0}
  },

  // Chord 9: C Major 7 (Imaj7) - Jazz voicing
  {
    .rootOffset = 0,
    .intervals = {0, 4, 7, 11, 14, 16, 19, 0},
    .octaveModifiers = {0, 0, 0, 0, 0, 0, 0, -1},
    .velocityModifiers = {0, -5, -5, -5, -10, -10, -10, 10},
    .isActive = {true, true, true, true, true, false, false, false},
    .channel = {0, 0, 0, 0, 0, 0, 0, 0}
  }
};

// Alternative preset banks (can be selected later)
// Pop Progression: I - V - vi - IV (C - G - Am - F)
const int popProgression[4] = {0, 4, 5, 3};  // Pad indices

// Jazz ii-V-I progression
const int jazzProgression[3] = {1, 4, 0};    // Dm7 - G7 - Cmaj7

// Sad progression: vi - IV - I - V (Am - F - C - G)
const int sadProgression[4] = {5, 3, 0, 4};

#endif // PRESETS_V2_H
