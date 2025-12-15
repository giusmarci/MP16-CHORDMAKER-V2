# MP16 ChordMaker V2

**Version 2.1.0**

Custom firmware for the **PATS MP-16** — a 16-key, RGB-lit MIDI macropad you assemble yourself. Connect to any DAW, synth, or MIDI device via USB or TRS MIDI.

This V2 firmware reimagines the MP-16 as a **9-pad chord controller** with dedicated controls for octave, arpeggiator, settings, and HOLD mode. Each pad triggers rich, customizable chords with up to 8 voices, a full-featured arpeggiator with MIDI clock sync, 16 style preset banks, and a **Generative Mode** that creates evolving harmonies automatically.

## Credits

Built on top of the original MP16 firmware by **Samuel Verburg** ([@samuelverburg](https://github.com/samuelverburg)).

- **Original Project:** [MP16-DIY-Kit](https://github.com/samuelverburg/MP16-DIY-Kit)
- **Get the Hardware:** [PATS MP-16 DIY Kit](https://patsaudio.com/?product=pats-mp-16-diy-kit)

**V2 developed in collaboration with Gianluca Simonelli** — Many of the features came from our modular jam sessions and brainstorming together.

## Features

### 9-Pad Chord Controller
- 3x3 grid of chord pads with unique LED colors
- Up to 8 notes per chord with individual velocity and octave modifiers
- Per-note MIDI channel routing for multi-timbral setups
- Octave shift controls (-3 to +3 octaves)
- HOLD mode for sustained chords and arpeggios

### Advanced Arpeggiator
- **7 Rate Options:** OFF, 1/2, 1/4, 1/8, 1/16, 1/32, 1/64 notes
- **8 Play Modes:** Up, Down, Up/Down, Down/Up, Random, Order, Chord (strum), 2-Octave
- **6 Patterns:** Straight, Swing, Dotted, Triplet, Humanize, Stutter
- **Advanced Settings:** Gate length, swing amount, humanization, velocity variation
- **Octave Range:** Expand arpeggios across multiple octaves
- **Clock Sync:** External MIDI clock input or internal BPM (20-300)
- **Play Chords:** When enabled, plays full chords alongside arpeggiated notes (Arp Settings page 8)

### Dual Play Modes

**Scale Mode** - Generates chords dynamically based on:
- 14 scale types: Major, Minor, Dorian, Phrygian, Lydian, Mixolydian, Locrian, Harmonic Minor, Melodic Minor, Pentatonic (Major/Minor), Blues, Whole Tone, Chromatic
- Live root key changes via encoder
- Live scale type changes via Shift+encoder

**Preset Mode** - 16 curated style banks with chord voicings:
| Bank | Style | Description |
|------|-------|-------------|
| 0 | DEFAULT | Diatonic major scale (I ii iii IV V vi vii°) |
| 1 | JAZZ | Rich ii-V-I voicings with 9ths, 13ths, altered dominants |
| 2 | POP | Modern I-V-vi-IV progressions with suspensions |
| 3 | LOFI | Chill hip-hop jazzy 7ths and 9ths |
| 4 | EDM | Big power chords and stabs for drops |
| 5 | SAD | Melancholic minor progressions (i-VI-III-VII) |
| 6 | FUNK | Funky 9th chords and dominant 7ths |
| 7 | RNB | Smooth R&B/Soul voicings |
| 8 | GOSPEL | Soulful church voicings with suspensions |
| 9 | AMBIENT | Lush atmospheric pads with open voicings |
| 10 | NEOSOUL | Modern jazzy soul (Erykah Badu / D'Angelo vibes) |
| 11 | ROCK | Classic rock power chords |
| 12 | BLUES | 12-bar blues voicings |
| 13 | LATIN | Bossa nova / Latin jazz progressions |
| 14 | CINEMA | Epic cinematic chords (Hans Zimmer style) |
| 15 | TRAP | Dark trap/hip-hop minor progressions |

### Generative Mode (NEW in v2.1)

Automatic evolving harmonies that mutate while you hold a chord.

**Access:**
- **Button 11** → Special Modes menu
- Rotate encoder → select "Generative"
- Click encoder → enter settings

**Settings:**
| Setting | Values | Description |
|---------|--------|-------------|
| Speed | 0-100% | Mutation rate. 0=slow, 100=rapid |
| Type | Chords/Scales | Chords=hop between pads, Scales=morph scale types |

**Controls:**
- Rotate encoder = change value
- Click encoder = next setting (Speed → Type → exit)
- Button 11 = exit menu anytime

**Using it:**
1. Enable Generative, exit menu
2. Hold any chord pad
3. Harmony evolves automatically
4. LED flashes cyan on mutation

### Screensaver (NEW in v2.1)

A mesmerizing starfield animation activates after 30 seconds of inactivity. Stars fly outward from the center, accelerating as they approach the edges. Any button press or encoder movement instantly returns to normal display.

### Quick Controls
- **Encoder:** Change root key in real-time
- **Shift + Encoder:** Change scale type
- **Shift + Pad:** Quick root key change to that note
- **Shift + Encoder Click:** Toggle between Scale and Preset modes
- **Button 11:** Open Special Modes menu (Generative Mode)
- **Shift + Arp+:** Open Arp Settings menu

### Visual Feedback
- 128x64 OLED display with piano visualization
- 17 WS2812B NeoPixel LEDs (one per button + shift key)
- Color-coded pads for each chord
- LED brightness for arp rate indication
- Animated startup sequence


## Button Layout

```
[0] [1] [2] [3]     Chord1  Chord2  Chord3  Settings
[4] [5] [6] [7]     Chord4  Chord5  Chord6  HOLD
[8] [9] [10][11]    Chord7  Chord8  Chord9  SpecialModes
[12][13][14][15]    Oct-    Oct+    Arp-    Arp+
```

### Button Functions

| Button | Normal Press | With Shift |
|--------|--------------|------------|
| 0-2, 4-6, 8-10 | Play chord | Quick root key change |
| 3 | Toggle Settings menu | - |
| 7 | Toggle HOLD mode | Max Notes menu |
| 11 | Special Modes menu | - |
| 12 | Octave down | - |
| 13 | Octave up | - |
| 14 | Arp rate down | - |
| 15 | Arp rate up | Arp Settings menu |

## Installation

1. Install the Arduino IDE
2. Install required libraries via Library Manager
3. Clone this repository
4. Open `V2/MP16_Chordmaker/MP16_Chordmaker.ino`
5. Select your board and upload

## Dependencies

```cpp
#include <Adafruit_TinyUSB.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <LittleFS.h>
```

## Usage Tips

### Getting Started
1. Connect via USB - the device appears as a USB MIDI controller
2. Use the encoder to set your root key (default: C3)
3. Press chord pads to play chords
4. Use Arp+/- buttons to enable and set arpeggiator speed

### Using MIDI Clock Sync
- The arpeggiator syncs automatically to incoming MIDI clock
- When no external clock is detected, internal BPM is used
- Adjust internal BPM in Settings menu

### Preset Mode Tips
- Press Shift + Encoder click to toggle Preset mode
- Use encoder to browse through 16 style banks
- Each bank has 9 pre-configured chords
- Great for quick songwriting and live performance

### Max Notes Feature
- Access via Shift + Button 7
- Limits how many notes play per chord (1-8)
- Useful for creating thinner voicings or single-note lines
- Works with both chord playback and arpeggiator

## License

Open source - feel free to use, modify, and share.

## Contributing

Contributions welcome! Please open an issue or pull request.
