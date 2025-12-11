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

// ============================================================================
// SONG PRESETS - Famous chord progressions from classic songs
// ============================================================================

#define NUM_SONG_PRESETS 10

// Song preset info (name for display)
const char* songPresetNames[NUM_SONG_PRESETS] = {
  "LetItBe",    // Beatles - C G Am F
  "Hotel Ca",   // Eagles - Bm F# A E G D Em F#
  "Canon",      // Pachelbel - D A Bm F#m G D G A
  "DontStop",   // Journey - E B C#m A
  "Careless",   // George Michael - Am Dm G C
  "TakeOnMe",   // a-ha - A E F#m D
  "NothElse",   // Metallica - Em D C G B7 Em
  "Wonderwl",   // Oasis - F#m A E B7sus4
  "Clocks",     // Coldplay - Eb Bbm Fm
  "Africa"      // Toto - F#m D A E
};

// SONG 0: Let It Be - Beatles (C major) - I V vi IV
const ChordV2 songLetItBe[9] = {
  // C (I)
  {0, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G (V)
  {7, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am (vi)
  {9, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F (IV)
  {5, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // C/G (I with G bass feel)
  {0, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G (V repeat)
  {7, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F (IV repeat)
  {5, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Em (iii)
  {4, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Dm (ii)
  {2, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// SONG 1: Hotel California - Eagles (Bm) - i V VII IV VI III iv V
const ChordV2 songHotelCalifornia[9] = {
  // Bm (i)
  {11, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F# (V)
  {6, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // A (VII)
  {9, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // E (IV)
  {4, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G (VI)
  {7, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // D (III)
  {2, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Em (iv)
  {4, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F#7 (V7)
  {6, {0, 4, 7, 10, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -5, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Bm (i return)
  {11, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// SONG 2: Canon in D - Pachelbel - I V vi iii IV I IV V
const ChordV2 songCanon[9] = {
  // D (I)
  {2, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // A (V)
  {9, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Bm (vi)
  {11, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F#m (iii)
  {6, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G (IV)
  {7, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // D (I)
  {2, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G (IV)
  {7, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // A (V)
  {9, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Em (ii)
  {4, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// SONG 3: Don't Stop Believin' - Journey (E major) - I V vi IV
const ChordV2 songDontStop[9] = {
  // E (I)
  {4, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // B (V)
  {11, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // C#m (vi)
  {1, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // A (IV)
  {9, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // E (I)
  {4, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // B (V)
  {11, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G#m (iii)
  {8, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // A (IV)
  {9, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F#m (ii)
  {6, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// SONG 4: Careless Whisper - George Michael (Am) - i iv VII III
const ChordV2 songCareless[9] = {
  // Am (i)
  {9, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Dm (iv)
  {2, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G (VII)
  {7, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // C (III)
  {0, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F (VI)
  {5, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // E7 (V7)
  {4, {0, 4, 7, 10, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -5, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am7 (i7)
  {9, {0, 3, 7, 10, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -5, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Dm7 (iv7)
  {2, {0, 3, 7, 10, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -5, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Em (v)
  {4, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// SONG 5: Take On Me - a-ha (A major) - I V vi IV
const ChordV2 songTakeOnMe[9] = {
  // A (I)
  {9, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // E (V)
  {4, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F#m (vi)
  {6, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // D (IV)
  {2, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Bm (ii)
  {11, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // E (V)
  {4, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // C#m (iii)
  {1, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // D (IV)
  {2, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // A (I)
  {9, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// SONG 6: Nothing Else Matters - Metallica (Em) - i VII VI III V7 i
const ChordV2 songNothingElse[9] = {
  // Em (i)
  {4, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // D (VII)
  {2, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // C (VI)
  {0, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G (III)
  {7, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // B7 (V7)
  {11, {0, 4, 7, 10, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -5, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Em (i)
  {4, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am (iv)
  {9, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // C (VI)
  {0, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // D (VII)
  {2, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// SONG 7: Wonderwall - Oasis (F#m/Em capo) - vi I V IV
const ChordV2 songWonderwall[9] = {
  // Em (vi feel)
  {4, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G (I)
  {7, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // D (V)
  {2, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // A7sus4 (IV sus)
  {9, {0, 5, 7, 10, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -5, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // C (bVI)
  {0, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // D (V)
  {2, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Em7 (vi7)
  {4, {0, 3, 7, 10, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -5, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G (I)
  {7, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // A (II)
  {9, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// SONG 8: Clocks - Coldplay (Eb) - I v iv (piano riff pattern)
const ChordV2 songClocks[9] = {
  // Eb (I)
  {3, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Bbm (v)
  {10, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Fm (ii)
  {5, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Eb (I)
  {3, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Ab (IV)
  {8, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Eb (I)
  {3, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Bbm (v)
  {10, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Fm (ii)
  {5, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Cm (vi)
  {0, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// SONG 9: Africa - Toto (F#m/A) - vi IV I V
const ChordV2 songAfrica[9] = {
  // F#m (vi)
  {6, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // D (IV)
  {2, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // A (I)
  {9, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // E (V)
  {4, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F#m (vi)
  {6, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // D (IV)
  {2, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // A (I)
  {9, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // E (V)
  {4, {0, 4, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Bm (ii)
  {11, {0, 3, 7, 12, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {0, -5, -5, -10, 0, 0, 0, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// Array of pointers to all song presets
const ChordV2* songPresets[NUM_SONG_PRESETS] = {
  songLetItBe,
  songHotelCalifornia,
  songCanon,
  songDontStop,
  songCareless,
  songTakeOnMe,
  songNothingElse,
  songWonderwall,
  songClocks,
  songAfrica
};

// Total presets = style banks + song presets
#define NUM_TOTAL_PRESETS (NUM_PRESET_BANKS + NUM_SONG_PRESETS)

// Combined preset names for display
const char* allPresetNames[NUM_TOTAL_PRESETS] = {
  "DEFAULT", "JAZZ", "POP", "LOFI", "EDM", "SAD",  // Style presets
  "LetItBe", "Hotel Ca", "Canon", "DontStop", "Careless", "TakeOnMe", "NothElse", "Wonderwl", "Clocks", "Africa"  // Song presets
};

#endif // PRESETS_V2_H
