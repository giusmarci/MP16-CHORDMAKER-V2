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
#define NUM_PRESET_BANKS 16

// Bank names and chord names
const PresetBank presetBankInfo[NUM_PRESET_BANKS] = {
  // 0: DEFAULT - Diatonic major scale
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
  // 3: LOFI - Chill hip-hop jazz chords
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
  },
  // 6: FUNK - Funky 9ths and 7ths
  {
    "FUNK",
    {"C9", "F9", "G9", "Dm7", "Em7", "Am7", "E7#9", "Bb9", "D9"}
  },
  // 7: RNB - Smooth R&B/Soul
  {
    "RNB",
    {"Cmaj7", "Am9", "Dm11", "G9", "Em7", "Fmaj9", "Bm7b5", "E7b9", "Abmaj7"}
  },
  // 8: GOSPEL - Soulful gospel voicings
  {
    "GOSPEL",
    {"Cadd9", "Am7", "Dm9", "G11", "Fsus2", "C/E", "Dm7", "Gsus4", "Cmaj9"}
  },
  // 9: AMBIENT - Lush atmospheric pads
  {
    "AMBIENT",
    {"Cmaj9", "Fsus2", "Gsus4", "Am11", "Emsus", "Dm9", "Fmaj7", "Gadd9", "Em9"}
  },
  // 10: NEOSOUL - Modern jazzy soul
  {
    "NEOSOUL",
    {"Dm9", "G13", "Cmaj9", "Fmaj7#11", "Bm7b5", "E7#9", "Am11", "D9sus", "Gm9"}
  },
  // 11: ROCK - Classic rock power chords
  {
    "ROCK",
    {"C5", "G5", "Am", "F5", "D5", "E5", "A5", "Bb5", "Fsus2"}
  },
  // 12: BLUES - 12-bar blues voicings
  {
    "BLUES",
    {"C7", "F7", "G7", "C9", "F9", "Dm7", "Em7", "Am7", "Bdim7"}
  },
  // 13: LATIN - Bossa nova / Latin jazz
  {
    "LATIN",
    {"Cmaj9", "A7b13", "Dm9", "G7b9", "Em7b5", "A7#5", "Dm7", "Db7", "Cmaj7"}
  },
  // 14: CINEMA - Epic cinematic chords
  {
    "CINEMA",
    {"Cm", "Ab", "Eb", "Bb", "Fm", "Gm", "Db", "Bbsus", "Cm9"}
  },
  // 15: TRAP - Dark trap/hip-hop
  {
    "TRAP",
    {"Cm7", "Abmaj7", "Ebmaj7", "Bb", "Fm7", "Gm7", "Dbmaj7", "Bb7", "Cm9"}
  }
};

// ============================================================================
// PRESET BANK 0: DEFAULT - Diatonic Major Scale (I ii iii IV V vi vii° V7 Imaj7)
// ============================================================================
const ChordV2 presetDefault[9] = {
  // C Major (I) - Full voiced with bass
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
// PRESET BANK 1: JAZZ - Rich ii-V-I voicings with extensions
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
  // E7alt (V/vi) - altered dominant with b9, #9, b13
  {4, {0, 4, 8, 10, 13, 15, 18, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, -5, -5, -10, -10, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am9 (vi)
  {9, {0, 3, 7, 10, 14, 15, 19, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, -5, 0, -10, -15, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // D7#11 - Lydian dominant
  {2, {0, 4, 7, 10, 18, 14, 21, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, -5, -5, 0, -15, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Gm7 (iv borrowed)
  {7, {0, 3, 7, 10, 12, 15, 19, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -10, -5, 0, -10, -15, 0}, {true, true, true, true, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// ============================================================================
// PRESET BANK 2: POP - Modern pop/rock progressions (I-V-vi-IV)
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
  // Csus4 - tension
  {0, {0, 5, 7, 12, 17, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 5, -5, -10, -10, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Em (iii)
  {4, {0, 3, 7, 12, 15, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 5, -5, -10, -10, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Dm7 (ii7)
  {2, {0, 3, 7, 10, 12, 15, 19, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 0, 5, -10, -10, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Gsus4 - pre-chorus tension
  {7, {0, 5, 7, 12, 17, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 5, -5, -10, -10, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Fadd9 - color chord
  {5, {0, 4, 7, 14, 12, 16, 19, 0}, {0, 0, 0, 0, 0, 0, 0, -1}, {0, -5, -5, 0, 5, -10, -10, 10}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// ============================================================================
// PRESET BANK 3: LOFI - Chill hip-hop jazzy chords
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
  // Em7 - floating
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
// PRESET BANK 4: EDM - Big powerful chords for drops
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
// PRESET BANK 5: SAD - Melancholic minor progressions (i VI III VII)
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

// ============================================================================
// PRESET BANK 6: FUNK - Funky 9ths and dominant 7ths
// ============================================================================
const ChordV2 presetFunk[9] = {
  // C9 - main groove chord
  {0, {0, 4, 7, 10, 14, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, 0, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F9 - IV groove
  {5, {0, 4, 7, 10, 14, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, 0, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G9 - V groove
  {7, {0, 4, 7, 10, 14, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, 0, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Dm7 - minor funk
  {2, {0, 3, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, 0, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Em7 - ii groove
  {4, {0, 3, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, 0, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am7 - vi funk
  {9, {0, 3, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, 0, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // E7#9 - Hendrix chord
  {4, {0, 4, 7, 10, 15, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -5, 0, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Bb9 - flat VII funk
  {10, {0, 4, 7, 10, 14, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, 0, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // D9 - chromatic approach
  {2, {0, 4, 7, 10, 14, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, 0, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// ============================================================================
// PRESET BANK 7: RNB - Smooth R&B/Soul voicings
// ============================================================================
const ChordV2 presetRnB[9] = {
  // Cmaj7 - smooth open
  {0, {0, 4, 7, 11, 14, 16, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 0, -5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am9 - silky
  {9, {0, 3, 7, 10, 14, 12, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 0, 5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Dm11 - deep
  {2, {0, 3, 7, 10, 14, 17, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 0, -5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G9 - buttery
  {7, {0, 4, 7, 10, 14, 12, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 0, 5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Em7 - velvet
  {4, {0, 3, 7, 10, 12, 14, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Fmaj9 - lush
  {5, {0, 4, 7, 11, 14, 16, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 0, -5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Bm7b5 - tension
  {11, {0, 3, 6, 10, 12, 15, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // E7b9 - drama
  {4, {0, 4, 7, 10, 13, 12, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -5, -5, 0, 5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Abmaj7 - borrowed beauty
  {8, {0, 4, 7, 11, 14, 16, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 0, -5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// ============================================================================
// PRESET BANK 8: GOSPEL - Soulful church voicings
// ============================================================================
const ChordV2 presetGospel[9] = {
  // Cadd9 - bright praise
  {0, {0, 4, 7, 14, 12, 16, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, 0, 5, -5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am7 - reflective
  {9, {0, 3, 7, 10, 12, 14, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Dm9 - soulful
  {2, {0, 3, 7, 10, 14, 17, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 0, -5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G11 - suspension glory
  {7, {0, 5, 7, 10, 12, 17, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 5, -5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Fsus2 - open air
  {5, {0, 2, 7, 12, 14, 19, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, 5, 0, -5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // C/E - walking bass
  {4, {0, 5, 8, 12, 17, 20, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, 5, -5, -5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Dm7 - testimony
  {2, {0, 3, 7, 10, 12, 15, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Gsus4 - anticipation
  {7, {0, 5, 7, 12, 17, 19, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, 5, -5, -5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Cmaj9 - resolution glory
  {0, {0, 4, 7, 11, 14, 16, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 0, -5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// ============================================================================
// PRESET BANK 9: AMBIENT - Lush atmospheric pads
// ============================================================================
const ChordV2 presetAmbient[9] = {
  // Cmaj9 - vast
  {0, {0, 7, 11, 14, 19, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -10, -5, -10, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Fsus2 - floating
  {5, {0, 2, 7, 12, 14, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -10, -5, -5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Gsus4 - suspended
  {7, {0, 5, 7, 12, 17, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -10, -5, -5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am11 - deep space
  {9, {0, 7, 10, 14, 17, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -10, -5, -5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Em add9 - ethereal
  {4, {0, 3, 7, 14, 19, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -10, -5, -10, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Dm9 - twilight
  {2, {0, 7, 10, 14, 19, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -10, -5, -10, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Fmaj7 - horizon
  {5, {0, 7, 11, 14, 19, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -10, -5, -10, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Gadd9 - clouds
  {7, {0, 4, 7, 14, 19, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -10, -5, -10, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Em9 - starlight
  {4, {0, 7, 10, 14, 19, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -10, -10, -5, -10, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// ============================================================================
// PRESET BANK 10: NEOSOUL - Modern jazzy soul (Erykah/D'Angelo vibes)
// ============================================================================
const ChordV2 presetNeoSoul[9] = {
  // Dm9 - pocket groove
  {2, {0, 3, 7, 10, 14, 17, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 0, -5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G13 - silky dominant
  {7, {0, 4, 10, 14, 21, 17, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -5, -5, -5, -10, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Cmaj9 - home base
  {0, {0, 4, 7, 11, 14, 16, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 0, -5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Fmaj7#11 - lydian color
  {5, {0, 4, 7, 11, 18, 14, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, -5, 0, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Bm7b5 - dark passing
  {11, {0, 3, 6, 10, 12, 15, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // E7#9 - spicy dominant
  {4, {0, 4, 7, 10, 15, 19, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -5, -5, 0, -5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am11 - deep minor
  {9, {0, 3, 7, 10, 14, 17, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 0, -5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // D9sus - floating tension
  {2, {0, 5, 7, 10, 14, 12, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 0, 5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Gm9 - borrowed mood
  {7, {0, 3, 7, 10, 14, 17, 0, 0}, {0, 0, 0, 0, 0, 0, -1, -1}, {0, -5, -10, -5, 0, -5, 0, 0}, {true, true, true, true, true, true, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// ============================================================================
// PRESET BANK 11: ROCK - Classic rock power chords
// ============================================================================
const ChordV2 presetRock[9] = {
  // C5 power
  {0, {0, 7, 12, 0, 0, 0, 0, 0}, {0, 0, 0, -1, -1, -1, -1, -1}, {0, 0, -5, 0, 0, 0, 0, 0}, {true, true, true, false, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G5 power
  {7, {0, 7, 12, 0, 0, 0, 0, 0}, {0, 0, 0, -1, -1, -1, -1, -1}, {0, 0, -5, 0, 0, 0, 0, 0}, {true, true, true, false, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am - minor rock
  {9, {0, 3, 7, 12, 15, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -5, 0, -5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F5 power
  {5, {0, 7, 12, 0, 0, 0, 0, 0}, {0, 0, 0, -1, -1, -1, -1, -1}, {0, 0, -5, 0, 0, 0, 0, 0}, {true, true, true, false, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // D5 power
  {2, {0, 7, 12, 0, 0, 0, 0, 0}, {0, 0, 0, -1, -1, -1, -1, -1}, {0, 0, -5, 0, 0, 0, 0, 0}, {true, true, true, false, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // E5 power
  {4, {0, 7, 12, 0, 0, 0, 0, 0}, {0, 0, 0, -1, -1, -1, -1, -1}, {0, 0, -5, 0, 0, 0, 0, 0}, {true, true, true, false, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // A5 power
  {9, {0, 7, 12, 0, 0, 0, 0, 0}, {0, 0, 0, -1, -1, -1, -1, -1}, {0, 0, -5, 0, 0, 0, 0, 0}, {true, true, true, false, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Bb5 power - bVII
  {10, {0, 7, 12, 0, 0, 0, 0, 0}, {0, 0, 0, -1, -1, -1, -1, -1}, {0, 0, -5, 0, 0, 0, 0, 0}, {true, true, true, false, false, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Fsus2 - rock ballad
  {5, {0, 2, 7, 12, 14, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -5, 0, -5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// ============================================================================
// PRESET BANK 12: BLUES - 12-bar blues voicings
// ============================================================================
const ChordV2 presetBlues[9] = {
  // C7 - I7
  {0, {0, 4, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F7 - IV7
  {5, {0, 4, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G7 - V7
  {7, {0, 4, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // C9 - I9 color
  {0, {0, 4, 7, 10, 14, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 0, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // F9 - IV9 color
  {5, {0, 4, 7, 10, 14, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 0, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Dm7 - ii7
  {2, {0, 3, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Em7 - iii7
  {4, {0, 3, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Am7 - vi7
  {9, {0, 3, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Bdim7 - passing diminished
  {11, {0, 3, 6, 9, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// ============================================================================
// PRESET BANK 13: LATIN - Bossa Nova / Latin Jazz
// ============================================================================
const ChordV2 presetLatin[9] = {
  // Cmaj9 - bossa home
  {0, {0, 4, 7, 11, 14, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 0, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // A7b13 - altered V/ii
  {9, {0, 4, 7, 10, 20, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -5, -5, -5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Dm9 - ii bossa
  {2, {0, 3, 7, 10, 14, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 0, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // G7b9 - V altered
  {7, {0, 4, 7, 10, 13, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -5, -5, 0, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Em7b5 - locrian
  {4, {0, 3, 6, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // A7#5 - augmented dominant
  {9, {0, 4, 8, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -5, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Dm7 - smooth ii
  {2, {0, 3, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Db7 - tritone sub
  {1, {0, 4, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -5, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Cmaj7 - resolution
  {0, {0, 4, 7, 11, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// ============================================================================
// PRESET BANK 14: CINEMA - Epic cinematic chords (Hans Zimmer style)
// ============================================================================
const ChordV2 presetCinema[9] = {
  // Cm - dark hero
  {0, {0, 3, 7, 12, 15, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, 1}, {5, 0, -5, 5, 0, -5, -10, 10}, {true, true, true, true, true, true, true, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Ab - hope rising
  {8, {0, 4, 7, 12, 16, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, 1}, {5, 0, -5, 5, 0, -5, -10, 10}, {true, true, true, true, true, true, true, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Eb - triumph
  {3, {0, 4, 7, 12, 16, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, 1}, {5, 0, -5, 5, 0, -5, -10, 10}, {true, true, true, true, true, true, true, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Bb - journey
  {10, {0, 4, 7, 12, 16, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, 1}, {5, 0, -5, 5, 0, -5, -10, 10}, {true, true, true, true, true, true, true, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Fm - tension
  {5, {0, 3, 7, 12, 15, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, 1}, {5, 0, -5, 5, 0, -5, -10, 10}, {true, true, true, true, true, true, true, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Gm - intensity
  {7, {0, 3, 7, 12, 15, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, 1}, {5, 0, -5, 5, 0, -5, -10, 10}, {true, true, true, true, true, true, true, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Db - mysterious
  {1, {0, 4, 7, 12, 16, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, 1}, {5, 0, -5, 5, 0, -5, -10, 10}, {true, true, true, true, true, true, true, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Bbsus - anticipation
  {10, {0, 5, 7, 12, 17, 19, 24, 0}, {0, 0, 0, 0, 0, 0, 0, 1}, {5, 0, -5, 5, 0, -5, -10, 10}, {true, true, true, true, true, true, true, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Cm9 - resolution depth
  {0, {0, 3, 7, 10, 14, 12, 19, 0}, {0, 0, 0, 0, 0, 0, 0, 1}, {5, 0, -5, -5, 0, 5, -10, 10}, {true, true, true, true, true, true, true, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// ============================================================================
// PRESET BANK 15: TRAP - Dark trap/hip-hop minor chords
// ============================================================================
const ChordV2 presetTrap[9] = {
  // Cm7 - dark base
  {0, {0, 3, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Abmaj7 - moody major
  {8, {0, 4, 7, 11, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Ebmaj7 - late night
  {3, {0, 4, 7, 11, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Bb - heavy
  {10, {0, 4, 7, 12, 16, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, 0, -5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Fm7 - melancholy
  {5, {0, 3, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Gm7 - haunting
  {7, {0, 3, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Dbmaj7 - dreamy dark
  {1, {0, 4, 7, 11, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Bb7 - tension
  {10, {0, 4, 7, 10, 12, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -5, -5, 5, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}},
  // Cm9 - resolution
  {0, {0, 3, 7, 10, 14, 0, 0, 0}, {0, 0, 0, 0, 0, -1, -1, -1}, {0, -5, -10, -5, 0, 0, 0, 0}, {true, true, true, true, true, false, false, false}, {0, 0, 0, 0, 0, 0, 0, 0}}
};

// Array of pointers to all preset banks
const ChordV2* presetBanks[NUM_PRESET_BANKS] = {
  presetDefault,
  presetJazz,
  presetPop,
  presetLofi,
  presetEDM,
  presetSad,
  presetFunk,
  presetRnB,
  presetGospel,
  presetAmbient,
  presetNeoSoul,
  presetRock,
  presetBlues,
  presetLatin,
  presetCinema,
  presetTrap
};

#endif // PRESETS_V2_H
