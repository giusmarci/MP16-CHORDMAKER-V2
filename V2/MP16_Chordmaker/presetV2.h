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

// Preset bank info
struct PresetBank {
  const char* name;                // Bank name (max 8 chars for display)
  const char* chordNames[9];       // Names for each chord
};

// Number of preset banks
#define NUM_PRESET_BANKS 6

// Bank names and chord names
const PresetBank presetBankInfo[NUM_PRESET_BANKS] = {
  // 0: DEFAULT - Diatonic progression
  {
    "DEFAULT",
    {"C", "Dm", "Em", "F", "G", "Am", "Bdim", "G7", "Cmaj7"}
  },
  // 1: JAZZ - ii-V-I voicings with extensions
  {
    "JAZZ",
    {"Dm9", "G13", "Cmaj9", "Fmaj7", "Bm7b5", "E7alt", "Am9", "D7#11", "Gm7"}
  },
  // 2: POP - Modern pop progressions
  {
    "POP",
    {"C", "G", "Am", "F", "Csus4", "Em", "Dm7", "Gsus4", "Fadd9"}
  },
  // 3: LOFI - Chill jazzy hip-hop chords
  {
    "LOFI",
    {"Cmaj9", "Am7", "Dm9", "G7", "Em7", "Fmaj7", "Bm7b5", "E7", "Am9"}
  },
  // 4: EDM - Big synth chords & power
  {
    "EDM",
    {"C5", "G5", "Am5", "F5", "Cstab", "Gsus", "Fmaj", "Am", "Em"}
  },
  // 5: SAD - Melancholic minor progressions
  {
    "SAD",
    {"Am", "F", "C", "G", "Dm", "E", "Am7", "Fmaj7", "Em"}
  }
};

// ============================================================================
// PRESET BANK 0: DEFAULT - Diatonic (your original - sounds amazing!)
// ============================================================================
const ChordV2 presetDefault[9] = {
  // C Major (I)
  {0, {0, 4, 7, 12, 16, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 0, -10, -10, -15, 10}, {true, true, true, true, false, false, false, true}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // D Minor (ii)
  {2, {0, 3, 7, 12, 15, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 0, -10, -10, -15, 10}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // E Minor (iii)
  {4, {0, 3, 7, 12, 15, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 0, -10, -10, -15, 10}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F Major (IV)
  {5, {0, 4, 7, 12, 16, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 0, -10, -10, -15, 10}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G Major (V)
  {7, {0, 4, 7, 12, 16, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 0, -10, -10, -15, 10}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // A Minor (vi)
  {9, {0, 3, 7, 12, 15, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 0, -10, -10, -15, 10}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // B Diminished (vii°)
  {11, {0, 3, 6, 12, 15, 18, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 0, -10, -10, -15, 10}, {true, true, true, false, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G7 (V7)
  {7, {0, 4, 7, 10, 12, 16, 19, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, -5, 0, -10, -10, 10}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Cmaj7 (Imaj7)
  {0, {0, 4, 7, 11, 14, 16, 19, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, -5, -10, -10, -10, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// ============================================================================
// PRESET BANK 1: JAZZ - Rich extended voicings
// ============================================================================
const ChordV2 presetJazz[9] = {
  // Dm9 (ii) - Root, b3, 5, b7, 9
  {2, {0, 3, 7, 10, 14, 17, 21, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, -5, 0, -10, -15, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G13 (V) - Root, 3, b7, 9, 13
  {7, {0, 4, 10, 14, 21, 7, 17, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, -5, -5, -10, -15, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Cmaj9 (I) - Root, 3, 5, 7, 9
  {0, {0, 4, 7, 11, 14, 16, 19, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, -5, 0, -10, -15, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Fmaj7 (IV)
  {5, {0, 4, 7, 11, 12, 16, 19, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, -5, 0, -10, -15, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Bm7b5 (vii half-dim)
  {11, {0, 3, 6, 10, 12, 15, 18, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, -5, 0, -10, -15, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // E7alt (V/vi) - altered dominant
  {4, {0, 4, 8, 10, 13, 15, 18, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, -5, -5, -10, -10, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am9 (vi)
  {9, {0, 3, 7, 10, 14, 15, 19, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, -5, 0, -10, -15, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // D7#11 - Lydian dominant
  {2, {0, 4, 7, 10, 18, 14, 21, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, -5, -5, 0, -15, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Gm7 (iv borrowed)
  {7, {0, 3, 7, 10, 12, 15, 19, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, -5, 0, -10, -15, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// ============================================================================
// PRESET BANK 2: POP - Modern pop/rock progressions
// ============================================================================
const ChordV2 presetPop[9] = {
  // C (I)
  {0, {0, 4, 7, 12, 16, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 5, -5, -10, -10, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G (V)
  {7, {0, 4, 7, 12, 16, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 5, -5, -10, -10, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am (vi)
  {9, {0, 3, 7, 12, 15, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 5, -5, -10, -10, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F (IV)
  {5, {0, 4, 7, 12, 16, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 5, -5, -10, -10, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Csus4
  {0, {0, 5, 7, 12, 17, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 5, -5, -10, -10, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Em (iii)
  {4, {0, 3, 7, 12, 15, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 5, -5, -10, -10, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Dm7 (ii7)
  {2, {0, 3, 7, 10, 12, 15, 19, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 0, 5, -10, -10, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Gsus4
  {7, {0, 5, 7, 12, 17, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 5, -5, -10, -10, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Fadd9
  {5, {0, 4, 7, 14, 12, 16, 19, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 0, 5, -10, -10, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// ============================================================================
// PRESET BANK 3: LOFI - Chill hip-hop jazz chords
// ============================================================================
const ChordV2 presetLofi[9] = {
  // Cmaj9 - dreamy
  {0, {0, 4, 7, 11, 14, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -15, -5, 0, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am7 - melancholic
  {9, {0, 3, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -15, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Dm9 - smooth
  {2, {0, 3, 7, 10, 14, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -15, -5, 0, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G7 - tension
  {7, {0, 4, 7, 10, 14, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -15, -5, 0, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Em7
  {4, {0, 3, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -15, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Fmaj7 - warm
  {5, {0, 4, 7, 11, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -15, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Bm7b5 - dark
  {11, {0, 3, 6, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -15, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // E7 - bluesy
  {4, {0, 4, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -15, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am9 - nostalgic
  {9, {0, 3, 7, 10, 14, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -15, -5, 0, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// ============================================================================
// PRESET BANK 4: EDM - Big powerful chords
// ============================================================================
const ChordV2 presetEDM[9] = {
  // C5 Power + octave
  {0, {0, 7, 12, 19, 24, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, 0, 0, 0, -5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G5 Power
  {7, {0, 7, 12, 19, 24, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, 0, 0, 0, -5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am5 Power
  {9, {0, 7, 12, 19, 24, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, 0, 0, 0, -5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F5 Power
  {5, {0, 7, 12, 19, 24, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, 0, 0, 0, -5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // C Stab (big major with octaves)
  {0, {0, 4, 7, 12, 16, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, 1}, {5, 0, 0, 5, 0, 0, -5, 10}, {true, true, true, true, true, true, true, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Gsus Big
  {7, {0, 5, 7, 12, 17, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {5, 0, 0, 5, 0, 0, -5, 0}, {true, true, true, true, true, true, true, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F Major Big
  {5, {0, 4, 7, 12, 16, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, 1}, {5, 0, 0, 5, 0, 0, -5, 10}, {true, true, true, true, true, true, true, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am Big
  {9, {0, 3, 7, 12, 15, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, 1}, {5, 0, 0, 5, 0, 0, -5, 10}, {true, true, true, true, true, true, true, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Em Big
  {4, {0, 3, 7, 12, 15, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, 1}, {5, 0, 0, 5, 0, 0, -5, 10}, {true, true, true, true, true, true, true, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// ============================================================================
// PRESET BANK 5: SAD - Melancholic minor progressions
// ============================================================================
const ChordV2 presetSad[9] = {
  // Am - root
  {9, {0, 3, 7, 12, 15, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, 5, -5, -10, -15, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F - bVI
  {5, {0, 4, 7, 12, 16, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, 5, -5, -10, -15, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // C - bIII
  {0, {0, 4, 7, 12, 16, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, 5, -5, -10, -15, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G - bVII
  {7, {0, 4, 7, 12, 16, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, 5, -5, -10, -15, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Dm - iv
  {2, {0, 3, 7, 12, 15, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, 5, -5, -10, -15, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // E (V of Am) - tension/hope
  {4, {0, 4, 7, 12, 16, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, 5, -5, -10, -15, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am7 - deeper sadness
  {9, {0, 3, 7, 10, 12, 15, 19, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, -5, 5, -10, -15, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Fmaj7 - bittersweet
  {5, {0, 4, 7, 11, 12, 16, 19, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, -5, 5, -10, -15, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Em - relative minor feel
  {4, {0, 3, 7, 12, 15, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, 5, -5, -10, -15, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// Array of pointers to all preset banks
const ChordV2* presetBanks[NUM_PRESET_BANKS] = {
  presetDefault,
  presetJazz,
  presetPop,
  presetLofi,
  presetEDM,
  presetSad
};

#endif // PRESETS_V2_H
