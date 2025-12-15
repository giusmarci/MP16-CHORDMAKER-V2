#ifndef MUSICTHEORY_V2_H
#define MUSICTHEORY_V2_H

// MIDI Note Names (C-1 to G9)
const char* midiNoteNames[128] = {
  "C-1", "C#-1", "D-1", "D#-1", "E-1", "F-1", "F#-1", "G-1", "G#-1", "A-1", "A#-1", "B-1",
  "C0", "C#0", "D0", "D#0", "E0", "F0", "F#0", "G0", "G#0", "A0", "A#0", "B0",
  "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
  "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
  "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
  "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
  "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
  "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6",
  "C7", "C#7", "D7", "D#7", "E7", "F7", "F#7", "G7", "G#7", "A7", "A#7", "B7",
  "C8", "C#8", "D8", "D#8", "E8", "F8", "F#8", "G8", "G#8", "A8", "A#8", "B8",
  "C9", "C#9", "D9", "D#9", "E9", "F9", "F#9", "G9"
};

// Scale Types (expanded from V1)
#define NUM_SCALES 32

const char* scaleNames[NUM_SCALES] = {
  "Major",        // 0
  "Minor",        // 1
  "Dorian",       // 2
  "Phrygian",     // 3
  "Lydian",       // 4
  "Mixolyd",      // 5
  "Locrian",      // 6
  "HarmMin",      // 7
  "JazzMin",      // 8
  "PentMaj",      // 9
  "PentMin",      // 10
  "Blues",        // 11
  "WholeTn",      // 12
  "Chromat",      // 13

  "PhrDom",       // 14
  "LydDom",       // 15
  "Altered",      // 16
  "DorB2",        // 17
  "DblHrm",       // 18
  "HungMin",      // 19
  "OctHW",        // 20
  "OctWH",        // 21

  // --- NEW +10 ---
  "UkrDor",       // 22 Ukrainian Dorian (Dorian #4)
  "LydAug",       // 23 Lydian Augmented (JazzMin mode 3)
  "Loc#2",        // 24 Locrian #2 (JazzMin mode 6)
  "MajBeb",       // 25 Major Bebop (8-note)
  "DomBeb",       // 26 Dominant Bebop (8-note)
  "MinBeb",       // 27 Minor Bebop / Bebop Dorian (8-note)
  "NeaMin",       // 28 Neapolitan Minor
  "NeaMaj",       // 29 Neapolitan Major
  "Enigmat",      // 30 Enigmatic
  "Hiraj"         // 31 Hirajoshi (Japanese pentatonic)
};

// Scale intervals in semitones (-1 padding)
const int scaleIntervals[NUM_SCALES][12] = {
  {0,2,4,5,7,9,11,-1,-1,-1,-1,-1},          // 0 Major
  {0,2,3,5,7,8,10,-1,-1,-1,-1,-1},          // 1 Minor
  {0,2,3,5,7,9,10,-1,-1,-1,-1,-1},          // 2 Dorian
  {0,1,3,5,7,8,10,-1,-1,-1,-1,-1},          // 3 Phrygian
  {0,2,4,6,7,9,11,-1,-1,-1,-1,-1},          // 4 Lydian
  {0,2,4,5,7,9,10,-1,-1,-1,-1,-1},          // 5 Mixolydian
  {0,1,3,5,6,8,10,-1,-1,-1,-1,-1},          // 6 Locrian
  {0,2,3,5,7,8,11,-1,-1,-1,-1,-1},          // 7 Harmonic Minor
  {0,2,3,5,7,9,11,-1,-1,-1,-1,-1},          // 8 Jazz Minor (melodic minor asc)
  {0,2,4,7,9,-1,-1,-1,-1,-1,-1,-1},         // 9 Major Pent
  {0,3,5,7,10,-1,-1,-1,-1,-1,-1,-1},        // 10 Minor Pent
  {0,3,5,6,7,10,-1,-1,-1,-1,-1,-1},         // 11 Blues
  {0,2,4,6,8,10,-1,-1,-1,-1,-1,-1},         // 12 Whole Tone
  {0,1,2,3,4,5,6,7,8,9,10,11},              // 13 Chromatic

  {0,1,4,5,7,8,10,-1,-1,-1,-1,-1},          // 14 Phrygian Dominant
  {0,2,4,6,7,9,10,-1,-1,-1,-1,-1},          // 15 Lydian Dominant
  {0,1,3,4,6,8,10,-1,-1,-1,-1,-1},          // 16 Altered / Super Locrian
  {0,1,3,5,7,9,10,-1,-1,-1,-1,-1},          // 17 Dorian b2
  {0,1,4,5,7,8,11,-1,-1,-1,-1,-1},          // 18 Double Harmonic
  {0,2,3,6,7,8,11,-1,-1,-1,-1,-1},          // 19 Hungarian Minor
  {0,1,3,4,6,7,9,10,-1,-1,-1,-1},           // 20 Octatonic Half-Whole
  {0,2,3,5,6,8,9,11,-1,-1,-1,-1},           // 21 Octatonic Whole-Half

  // --- NEW +10 intervals ---
  {0,2,3,6,7,9,10,-1,-1,-1,-1,-1},          // 22 Ukrainian Dorian (Dorian #4)
  {0,2,4,6,8,9,11,-1,-1,-1,-1,-1},          // 23 Lydian Augmented
  {0,2,3,5,6,8,10,-1,-1,-1,-1,-1},          // 24 Locrian #2
  {0,2,4,5,7,8,9,11,-1,-1,-1,-1},           // 25 Major Bebop (8 notes)
  {0,2,4,5,7,9,10,11,-1,-1,-1,-1},          // 26 Dominant Bebop (8 notes)
  {0,2,3,4,5,7,9,10,-1,-1,-1,-1},           // 27 Minor Bebop / Bebop Dorian (8 notes)
  {0,1,3,5,7,8,11,-1,-1,-1,-1,-1},          // 28 Neapolitan Minor
  {0,1,3,5,7,9,11,-1,-1,-1,-1,-1},          // 29 Neapolitan Major
  {0,1,4,6,8,10,11,-1,-1,-1,-1,-1},         // 30 Enigmatic
  {0,2,3,7,8,-1,-1,-1,-1,-1,-1,-1}          // 31 Hirajoshi (5 notes)
};

// Number of notes in each scale
const int scaleNoteCounts[NUM_SCALES] = {
  7,7,7,7,7,7,7,7,7, 5,5,6,6,12,
  7,7,7,7,7,7, 8,8,
  7,7,7, 8,8,8, 7,7,7, 5
};

// Chord quality types
#define NUM_CHORD_TYPES 12

const char* chordTypeNames[NUM_CHORD_TYPES] = {
  "Maj",      // Major triad
  "Min",      // Minor triad
  "Dim",      // Diminished
  "Aug",      // Augmented
  "Maj7",     // Major 7th
  "Min7",     // Minor 7th
  "Dom7",     // Dominant 7th
  "Dim7",     // Diminished 7th
  "Sus2",     // Suspended 2nd
  "Sus4",     // Suspended 4th
  "Add9",     // Add 9
  "Power"     // Power chord (root + 5th)
};

// Chord intervals from root (in semitones)
// -1 indicates no note at that position
const int chordIntervals[NUM_CHORD_TYPES][8] = {
  {0, 4, 7, -1, -1, -1, -1, -1},        // Maj: R, 3, 5
  {0, 3, 7, -1, -1, -1, -1, -1},        // Min: R, b3, 5
  {0, 3, 6, -1, -1, -1, -1, -1},        // Dim: R, b3, b5
  {0, 4, 8, -1, -1, -1, -1, -1},        // Aug: R, 3, #5
  {0, 4, 7, 11, -1, -1, -1, -1},        // Maj7: R, 3, 5, 7
  {0, 3, 7, 10, -1, -1, -1, -1},        // Min7: R, b3, 5, b7
  {0, 4, 7, 10, -1, -1, -1, -1},        // Dom7: R, 3, 5, b7
  {0, 3, 6, 9, -1, -1, -1, -1},         // Dim7: R, b3, b5, bb7
  {0, 2, 7, -1, -1, -1, -1, -1},        // Sus2: R, 2, 5
  {0, 5, 7, -1, -1, -1, -1, -1},        // Sus4: R, 4, 5
  {0, 4, 7, 14, -1, -1, -1, -1},        // Add9: R, 3, 5, 9
  {0, 7, 12, -1, -1, -1, -1, -1}        // Power: R, 5, 8ve
};

// Number of notes in each chord type
const int chordNoteCounts[NUM_CHORD_TYPES] = {
  3, 3, 3, 3, 4, 4, 4, 4, 3, 3, 4, 3
};

// Arpeggiator timing values (ms) at 120 BPM
// Index: 0=off, 1=1/2, 2=1/4, 3=1/8, 4=1/16, 5=1/32, 6=1/64
const int arpTimings[7] = {
  0,      // OFF
  1000,   // 1/2 note
  500,    // 1/4 note
  250,    // 1/8 note
  125,    // 1/16 note
  62,     // 1/32 note
  31      // 1/64 note
};

const char* arpRateNames[7] = {
  "OFF", "1/2", "1/4", "1/8", "1/16", "1/32", "1/64"
};

// Arpeggiator mode names
#define NUM_ARP_MODES 8

const char* arpModeNames[NUM_ARP_MODES] = {
  "Up",       // 0: Low to high
  "Down",     // 1: High to low
  "UpDown",   // 2: Up then down (ping-pong)
  "DownUp",   // 3: Down then up
  "Random",   // 4: Random note each step
  "Order",    // 5: Play in order notes were added
  "Chord",    // 6: All notes together (like strumming)
  "2Oct"      // 7: Up through 2 octaves
};

// Common chord progressions (as scale degrees, 0-indexed)
// Can be used for suggestions or presets
const int progressionI_IV_V_I[4] = {0, 3, 4, 0};      // I-IV-V-I
const int progressionI_V_vi_IV[4] = {0, 4, 5, 3};     // I-V-vi-IV (Pop)
const int progressionii_V_I[3] = {1, 4, 0};           // ii-V-I (Jazz)
const int progressionI_vi_IV_V[4] = {0, 5, 3, 4};     // I-vi-IV-V (50s)

// LED Colors for V2 (RGB hex values)
#define COLOR_CHORD_1    0xFF6B6B  // Coral red
#define COLOR_CHORD_2    0x4ECDC4  // Teal
#define COLOR_CHORD_3    0x45B7D1  // Sky blue
#define COLOR_CHORD_4    0x96CEB4  // Sage green
#define COLOR_CHORD_5    0xFFEEAD  // Cream yellow
#define COLOR_CHORD_6    0xD4A5A5  // Dusty rose
#define COLOR_CHORD_7    0x9B59B6  // Purple
#define COLOR_CHORD_8    0xF39C12  // Orange
#define COLOR_CHORD_9    0x1ABC9C  // Emerald

#define COLOR_OCTAVE     0x3498DB  // Blue
#define COLOR_ARP        0x2ECC71  // Green
#define COLOR_SHIFT      0xE74C3C  // Red
#define COLOR_PLAYING    0xFFFFFF  // White
#define COLOR_EDIT       0xFF00FF  // Magenta

// Pad colors array for easy access
const uint32_t padColors[9] = {
  COLOR_CHORD_1, COLOR_CHORD_2, COLOR_CHORD_3,
  COLOR_CHORD_4, COLOR_CHORD_5, COLOR_CHORD_6,
  COLOR_CHORD_7, COLOR_CHORD_8, COLOR_CHORD_9
};

#endif // MUSICTHEORY_V2_H
