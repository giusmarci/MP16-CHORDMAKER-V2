/*
 * MP16 ChordMaker V2
 * A 9-pad chord controller with arpeggiator and octave controls
 *
 * Button Layout:
 * [0][1][2][3]  - Chords 1-4
 * [4][5][6][7]  - Chords 5-8
 * [8][9][10][11] - Chord 9, (unused), Arp-, Arp+
 * [12][13][14][15] - Oct-, Oct+, (reserved), (reserved)
 */

#include <Adafruit_TinyUSB.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <LittleFS.h>

// Pin Definitions
#define SHIFT_PIN 3
#define NEOPIXEL_PIN 2
#define NUM_PIXELS 17
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C
#define TX_PIN 0
#define RX_PIN 1

#define COL0_PIN 9
#define COL1_PIN 10
#define COL2_PIN 11
#define COL3_PIN 12

#define ROW0_PIN 13
#define ROW1_PIN 14
#define ROW2_PIN 15
#define ROW3_PIN 26

#define ENCODER_S 6
#define ENCODER_A 7
#define ENCODER_B 8

// Button role definitions for V2
// Layout:
// [0][1][2][3]    Chord1, Chord2, Chord3, (unused)
// [4][5][6][7]    Chord4, Chord5, Chord6, (unused)
// [8][9][10][11]  Chord7, Chord8, Chord9, (unused)
// [12][13][14][15] Oct-, Oct+, Arp-, Arp+

#define NUM_CHORD_PADS 9
const int CHORD_PAD_BUTTONS[NUM_CHORD_PADS] = {0, 1, 2, 4, 5, 6, 8, 9, 10};
#define BTN_OCT_DOWN 12
#define BTN_OCT_UP 13
#define BTN_ARP_DOWN 14
#define BTN_ARP_UP 15

// Hardware objects
Adafruit_NeoPixel pixels(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_USBD_MIDI usb_midi;

// Include V2 headers after struct definitions needed
#include "musicTheoryV2.h"
#include "presetV2.h"

// Key state arrays
bool shiftState = false;
bool previousShiftState = false;
bool encoderState = false;
bool previousEncoderState = false;
bool keyStates[16] = {false};
bool previousKeyStates[16] = {false};
bool padStates[9] = {false};
bool previousPadStates[9] = {false};

// Encoder handling
volatile int encoderValue = 0;
volatile int lastEncoded = 0;
volatile int stepCounter = 0;
const int encoderStates[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

// MIDI state machine
enum MIDIState {
  WAITING_FOR_STATUS,
  WAITING_FOR_DATA1,
  WAITING_FOR_DATA2
};

volatile uint8_t midiStatus = 0;
volatile uint8_t midiData1 = 0;
volatile uint8_t midiData2 = 0;
volatile MIDIState currentMidiState = WAITING_FOR_STATUS;
volatile bool midiMessageReady = false;

// Note reference counting (4 channels)
int noteCountA[128] = {0};
int noteCountB[128] = {0};
int noteCountC[128] = {0};
int noteCountD[128] = {0};

// V2 Settings
struct SettingsV2 {
  int rootNote = 48;              // C3
  int scaleType = 0;              // Scale type for SCALE mode (0=Major, 1=Minor, etc.)
  int midiTrigChannel = 1;
  int midiOutputAChannel = 15;    // Default channel 16 (0-indexed = 15)
  int midiOutputBChannel = 15;
  int midiOutputCChannel = 15;
  int midiOutputDChannel = 15;
  bool midiThru = false;
  float velocityScaling = 1.0;
  int defaultVelocity = 100;
  int ledBrightness = 100;
  int playMode = 0;               // 0 = SCALE mode (user sets root/scale), 1 = PRESET mode (jazz, pop, etc.)
  bool midiClockSync = true;      // Sync arp to external MIDI clock (default ON)
  int presetBank = 0;             // Current preset bank for PRESET mode (0-5)
  int internalBpm = 120;          // Internal BPM when not receiving external clock (20-300)
  // Arp settings
  int arpPattern = 0;             // 0=straight, 1=swing, 2=dotted, 3=triplet, 4=humanize, 5=stutter
  int arpGate = 80;               // Gate length as % (10-100)
  int arpSwing = 50;              // Swing amount 0-100 (50=straight)
  int arpHumanize = 0;            // Timing randomness 0-50ms
  int arpVelocityVar = 0;         // Velocity variation 0-50
  int arpOctaveRange = 0;         // 0=none, 1=+1oct, 2=+2oct, 3=-1oct, 4=+/-1oct
  int maxNotesPerChord = 3;       // Max notes per chord (1-8, default 3)
};

// Arp pattern names
#define NUM_ARP_PATTERNS 6
const char* arpPatternNames[NUM_ARP_PATTERNS] = {
  "Straight", "Swing", "Dotted", "Triplet", "Human", "Stutter"
};

// Arp octave range names
#define NUM_ARP_OCTAVES 9
const char* arpOctaveNames[NUM_ARP_OCTAVES] = {
  "Off", "+1", "+2", "+3", "+4", "+5", "-1", "-2", "+/-1"
};

// V2 Runtime state
struct RuntimeState {
  int currentOctave = 0;          // -3 to +3
  int arpRate = 0;                // 0 = OFF, 1-6 = rates
  int arpMode = 0;                // 0=up, 1=down, 2=updown, 3=random
  bool arpDirection = true;       // true = up, false = down (for updown mode)
  int arpNoteIndex = 0;
  unsigned long lastArpTime = 0;
  int arpStepInPattern = 0;       // For pattern-based timing
  int arpOctaveStep = 0;          // Current octave in multi-octave mode
  bool inEditMode = false;
  int editingPad = -1;
  int editNoteIndex = 0;
  int activePad = -1;             // Currently playing pad (-1 = none)
  bool introComplete = false;
  bool inSettingsMode = false;    // Settings mode (toggle with button 3)
  bool inArpSettings = false;     // Arp settings mode (toggle with button 11)
  bool inMaxNotesMenu = false;    // Max notes menu (toggle with Shift+7)
  int settingsPage = 0;           // Settings page index
  int arpSettingsPage = 0;        // Arp settings page: 0=Pattern, 1=Gate, 2=Swing, 3=Humanize, 4=Velocity, 5=Octave
  bool holdMode = false;          // HOLD mode - sustain notes after releasing pad
};

// Play mode names
const char* playModeNames[2] = {"SCALE", "PRESET"};

// MIDI Clock state
volatile bool midiClockReceived = false;    // Flag set by interrupt when clock pulse received
volatile int midiClockCounter = 0;          // Counts clock pulses (24 PPQN)
volatile bool midiTransportRunning = false; // Start/Stop state
volatile unsigned long lastClockTime = 0;   // For detecting clock presence
volatile int arpClockCount = 0;             // Counts clocks since last arp trigger (for sync)
bool externalClockActive = false;           // True when receiving valid external clock
bool clockPulseIndicator = false;           // Blinks on each clock pulse for visual feedback
unsigned long lastClockPulseTime = 0;       // For clock indicator timing

// BPM detection and internal clock
volatile unsigned long lastClockMicros = 0; // For BPM calculation
volatile unsigned long clockIntervalMicros = 0; // Average interval between clocks
int detectedBpm = 0;                        // Calculated BPM from external clock
unsigned long lastInternalClockTime = 0;    // For internal clock generation
int internalClockCounter = 0;               // Internal clock pulse counter

// Arpeggiator note tracking (to prevent stuck notes)
int lastArpPad = -1;                        // Which pad the last arp note was from
int lastArpNoteIndex = -1;                  // Which note index was last played
int lastArpNoteMidi = -1;                   // Actual MIDI note number that was played (with octave shift)
int lastArpNoteChannel = 0;                 // Channel used for last note
bool arpNotePlaying = false;                // Is an arp note currently sounding

SettingsV2 settings;
RuntimeState state;
PadV2 pads[9];

// Animation state
unsigned long animStartTime = 0;
int animPhase = 0;

// Display state
float dimFactor = 0.3;

//================================ SETUP ================================

void setup() {
  initHardware();
  usb_midi.begin();

  pinMode(RX_PIN, INPUT_PULLUP);
  Serial1.begin(31250);

  if (!LittleFS.begin()) {
    // LittleFS failed - continue anyway
  }

  loadSettings();
  initPadsFromPreset();

  animStartTime = millis();
  playIntroAnimation();
}

//================================ LOOP ================================

void loop() {
  if (!state.introComplete) {
    updateIntroAnimation();
    return;
  }

  // Main V2 loop
  checkKeys();
  updateMIDI();
  updateInternalClock();  // Generate internal clock when no external
  updateArpeggiator();
  updateVisuals();
  updateDisplay();
}

//================================ HARDWARE INIT ================================

void initHardware() {
  // Key matrix
  pinMode(ROW0_PIN, OUTPUT);
  pinMode(ROW1_PIN, OUTPUT);
  pinMode(ROW2_PIN, OUTPUT);
  pinMode(ROW3_PIN, OUTPUT);
  pinMode(COL0_PIN, INPUT_PULLUP);
  pinMode(COL1_PIN, INPUT_PULLUP);
  pinMode(COL2_PIN, INPUT_PULLUP);
  pinMode(COL3_PIN, INPUT_PULLUP);

  // Controls
  pinMode(SHIFT_PIN, INPUT_PULLUP);
  pinMode(ENCODER_S, INPUT_PULLUP);
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);

  // Interrupts
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RX_PIN), midiInterruptHandler, FALLING);

  // NeoPixels
  pixels.begin();
  pixels.setBrightness(100);
  pixels.clear();
  pixels.show();

  // OLED Display
  display.setRotation(2);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();
}

//================================ INTRO ANIMATION ================================

void playIntroAnimation() {
  animPhase = 0;
  animStartTime = millis();
}

void updateIntroAnimation() {
  unsigned long elapsed = millis() - animStartTime;

  display.clearDisplay();

  // Phase 1: Scanline effect (0-800ms)
  if (elapsed < 800) {
    int scanLine = map(elapsed, 0, 800, 0, 64);

    // Draw pixelated "MP16" text
    display.setTextSize(4);
    display.setCursor(16, 8);
    display.print("MP16");

    // Scanline mask effect
    for (int y = scanLine; y < 64; y++) {
      display.drawFastHLine(0, y, 128, BLACK);
    }

    // LED cascade with bounce
    int litPixels = map(elapsed, 0, 800, 0, 17);
    pixels.clear();
    for (int i = 0; i < min(litPixels, 17); i++) {
      pixels.setPixelColor(i, padColors[i % 9]);
    }
    pixels.show();
  }
  // Phase 2: Glitch effect (800-1200ms)
  else if (elapsed < 1200) {
    display.setTextSize(4);

    // Glitch offset
    int glitchX = random(-2, 3);
    int glitchY = random(-1, 2);

    display.setCursor(16 + glitchX, 8 + glitchY);
    display.print("MP16");

    // Random pixel noise
    for (int i = 0; i < 20; i++) {
      display.drawPixel(random(128), random(64), WHITE);
    }

    // V2 appears
    display.setTextSize(2);
    display.setCursor(48, 48);
    display.print("V2");

    // LEDs flash
    pixels.clear();
    if ((elapsed / 50) % 2 == 0) {
      for (int i = 0; i < 9; i++) {
        pixels.setPixelColor(CHORD_PAD_BUTTONS[i] + 1, padColors[i]);
      }
    }
    pixels.show();
  }
  // Phase 3: Stable with pulse (1200-2200ms)
  else if (elapsed < 2200) {
    display.setTextSize(4);
    display.setCursor(16, 8);
    display.print("MP16");

    // Underline animation
    int lineWidth = map(elapsed - 1200, 0, 300, 0, 96);
    lineWidth = min(lineWidth, 96);
    display.drawFastHLine(16, 42, lineWidth, WHITE);

    display.setTextSize(2);
    display.setCursor(48, 48);
    display.print("V2");

    // LEDs breathe
    float pulse = (sin((elapsed - 1200) * 0.006) + 1) * 0.5;
    pixels.clear();
    for (int i = 0; i < 9; i++) {
      uint32_t color = dimColor(padColors[i], 0.2 + pulse * 0.8);
      pixels.setPixelColor(CHORD_PAD_BUTTONS[i] + 1, color);
    }
    pixels.show();
  }
  // Done
  else {
    state.introComplete = true;
    encoderValue = 0;
  }

  display.display();
}


//================================ KEY SCANNING ================================

void checkKeys() {
  previousShiftState = shiftState;
  previousEncoderState = encoderState;
  for (int i = 0; i < 16; i++) {
    previousKeyStates[i] = keyStates[i];
  }
  for (int i = 0; i < 9; i++) {
    previousPadStates[i] = padStates[i];
  }

  shiftState = !digitalRead(SHIFT_PIN);
  encoderState = !digitalRead(ENCODER_S);

  // Scan key matrix
  for (int row = 0; row < 4; row++) {
    digitalWrite(ROW0_PIN, row == 0 ? LOW : HIGH);
    digitalWrite(ROW1_PIN, row == 1 ? LOW : HIGH);
    digitalWrite(ROW2_PIN, row == 2 ? LOW : HIGH);
    digitalWrite(ROW3_PIN, row == 3 ? LOW : HIGH);

    for (int col = 0; col < 4; col++) {
      int keyIndex = row * 4 + col;
      keyStates[keyIndex] = !digitalRead(COL0_PIN + col);
    }
  }
  digitalWrite(ROW3_PIN, HIGH);

  // Process button presses
  processButtonPresses();
}

void processButtonPresses() {
  // Button 3 = Settings toggle (click to open/close)
  if (keyStates[3] && !previousKeyStates[3]) {
    if (state.inArpSettings) {
      // Close arp settings first if open
      state.inArpSettings = false;
    }
    state.inSettingsMode = !state.inSettingsMode;
    state.settingsPage = 0;
    if (!state.inSettingsMode) {
      saveSettings();
    }
    return;
  }

  // Button 11 = Arp Settings toggle (click to open/close)
  if (keyStates[11] && !previousKeyStates[11]) {
    if (state.inSettingsMode) {
      // Close main settings first if open
      state.inSettingsMode = false;
    }
    state.inArpSettings = !state.inArpSettings;
    state.arpSettingsPage = 0;
    if (!state.inArpSettings) {
      saveSettings();
    }
    return;
  }

  // Button 7 = HOLD toggle (sustain notes after releasing pad)
  // Shift + Button 7 = Max Notes menu
  if (keyStates[7] && !previousKeyStates[7]) {
    if (shiftState) {
      // Shift + 7 = toggle Max Notes menu
      state.inMaxNotesMenu = !state.inMaxNotesMenu;
    } else {
      // Just 7 = toggle HOLD mode
      state.holdMode = !state.holdMode;
      // If turning off hold, stop any sustained notes immediately
      if (!state.holdMode && state.activePad >= 0) {
        if (state.arpRate > 0) {
          stopCurrentArpNote();
        }
        stopChord(state.activePad);
        // Only clear activePad if pad is not physically pressed
        if (!padStates[state.activePad]) {
          state.activePad = -1;
        }
      }
    }
  }

  // Handle Max Notes menu (Shift+7)
  if (state.inMaxNotesMenu) {
    if (encoderValue != 0) {
      int oldMax = settings.maxNotesPerChord;
      if (encoderValue > 0) {
        settings.maxNotesPerChord = constrain(settings.maxNotesPerChord + 1, 1, 8);
      } else {
        settings.maxNotesPerChord = constrain(settings.maxNotesPerChord - 1, 1, 8);
      }
      // If playing and changed, restart chord with new note count
      if (state.activePad >= 0 && settings.maxNotesPerChord != oldMax) {
        if (state.arpRate > 0) {
          stopCurrentArpNote();
        } else {
          stopChord(state.activePad);
          playChord(state.activePad);
        }
      }
      encoderValue = 0;
      saveSettings();
    }
  }

  // Handle arp settings mode (Shift+7)
  // Pages: 0=Pattern, 1=Gate, 2=Swing, 3=Humanize, 4=VelVar, 5=Octave, 6=Mode
  if (state.inArpSettings) {
    const int maxArpPages = 6;

    // Encoder click cycles through arp settings pages (wraps around, only Shift+7 exits)
    if (encoderState && !previousEncoderState) {
      state.arpSettingsPage++;
      if (state.arpSettingsPage > maxArpPages) {
        state.arpSettingsPage = 0;  // Wrap around, don't exit
      }
    }

    // Encoder rotation changes current arp setting
    if (encoderValue != 0) {
      switch (state.arpSettingsPage) {
        case 0: // Pattern
          if (encoderValue > 0) {
            settings.arpPattern = (settings.arpPattern + 1) % NUM_ARP_PATTERNS;
          } else {
            settings.arpPattern = (settings.arpPattern + NUM_ARP_PATTERNS - 1) % NUM_ARP_PATTERNS;
          }
          break;
        case 1: // Gate
          settings.arpGate = constrain(settings.arpGate + (encoderValue > 0 ? 10 : -10), 10, 100);
          break;
        case 2: // Swing
          settings.arpSwing = constrain(settings.arpSwing + (encoderValue > 0 ? 5 : -5), 0, 100);
          break;
        case 3: // Humanize
          settings.arpHumanize = constrain(settings.arpHumanize + (encoderValue > 0 ? 5 : -5), 0, 50);
          break;
        case 4: // Velocity Variation
          settings.arpVelocityVar = constrain(settings.arpVelocityVar + (encoderValue > 0 ? 5 : -5), 0, 50);
          break;
        case 5: // Octave Range
          if (encoderValue > 0) {
            settings.arpOctaveRange = (settings.arpOctaveRange + 1) % NUM_ARP_OCTAVES;
          } else {
            settings.arpOctaveRange = (settings.arpOctaveRange + NUM_ARP_OCTAVES - 1) % NUM_ARP_OCTAVES;
          }
          break;
        case 6: // Arp Mode (Up/Down/UpDown/Random)
          if (encoderValue > 0) {
            state.arpMode = (state.arpMode + 1) % NUM_ARP_MODES;
          } else {
            state.arpMode = (state.arpMode + NUM_ARP_MODES - 1) % NUM_ARP_MODES;
          }
          break;
      }
      encoderValue = 0;
    }

    previousEncoderState = encoderState;
    // DON'T return - allow chord pads to work while in arp settings!
  }

  // Handle main settings mode
  // SCALE mode pages: 0=Mode, 1=Root, 2=Scale, 3=Channel, 4=Clock
  // PRESET mode pages: 0=Mode, 1=Bank, 2=Channel, 3=Clock
  if (state.inSettingsMode) {
    // Pages: 0=Mode, 1=Channel, 2=BPM (same for both modes now)
    int maxPages = 2;

    // Encoder click cycles through settings pages (wraps around, only Shift+3 exits)
    if (encoderState && !previousEncoderState) {
      state.settingsPage++;
      if (state.settingsPage > maxPages) {
        state.settingsPage = 0;  // Wrap around, don't exit
        loadCurrentMode();  // Reload in case settings changed
      }
    }

    // Encoder rotation changes current setting
    if (encoderValue != 0) {
      switch (state.settingsPage) {
        case 0:  // Play Mode
          settings.playMode = 1 - settings.playMode;  // Toggle 0/1
          state.settingsPage = 0;  // Reset to show new mode's settings
          loadCurrentMode();
          break;
        case 1:  // MIDI Channel
          if (encoderValue > 0) {
            settings.midiOutputAChannel = (settings.midiOutputAChannel + 1) % 16;
          } else {
            settings.midiOutputAChannel = (settings.midiOutputAChannel + 15) % 16;
          }
          break;
        case 2:  // BPM (only editable when no external clock)
          if (!externalClockActive) {
            if (encoderValue > 0) {
              settings.internalBpm = constrain(settings.internalBpm + 1, 20, 300);
            } else {
              settings.internalBpm = constrain(settings.internalBpm - 1, 20, 300);
            }
          }
          break;
      }
      encoderValue = 0;
    }

    // Settings menu stays open until Shift+3 is pressed again (handled at top of function)
    return;
  }

  // Encoder changes root note (works while playing too for live transposition!)
  if (!state.inEditMode && !state.inSettingsMode && !state.inArpSettings) {
    if (encoderValue != 0) {
      if (settings.playMode == 0) {
        // SCALE mode: Shift+encoder = change scale, encoder alone = change root
        if (shiftState) {
          // Shift+encoder: change scale type
          int oldScale = settings.scaleType;
          if (encoderValue > 0) {
            settings.scaleType = (settings.scaleType + 1) % NUM_SCALES;
          } else {
            settings.scaleType = (settings.scaleType + NUM_SCALES - 1) % NUM_SCALES;
          }
          // If playing, update live
          if (state.activePad >= 0 && settings.scaleType != oldScale) {
            if (state.arpRate > 0) {
              stopCurrentArpNote();
            }
            stopChord(state.activePad);
            loadScaleMode();
            if (state.arpRate == 0) {
              playChord(state.activePad);
            }
          } else {
            loadCurrentMode();
          }
        } else {
          // Encoder alone: change root note (live!)
          int oldRoot = settings.rootNote;
          if (encoderValue > 0) {
            settings.rootNote = constrain(settings.rootNote + 1, 24, 72);
          } else {
            settings.rootNote = constrain(settings.rootNote - 1, 24, 72);
          }

          // If playing, transpose live: stop old notes, play new
          if (state.activePad >= 0 && settings.rootNote != oldRoot) {
            // Stop any arp note first (uses stored MIDI note, no recalc needed)
            if (state.arpRate > 0) {
              stopCurrentArpNote();
            }

            // Stop current chord with old root
            int tempRoot = settings.rootNote;
            settings.rootNote = oldRoot;
            stopChord(state.activePad);
            settings.rootNote = tempRoot;

            // Play chord with new root (arp will pick up on next tick)
            loadScaleMode();  // Regenerate chord offsets
            if (state.arpRate == 0) {
              playChord(state.activePad);
            }
            // If arp is on, don't play chord - let arp handle it
          } else {
            loadCurrentMode();
          }
        }
      } else {
        // PRESET mode: encoder changes preset bank (only when idle)
        if (state.activePad < 0) {
          if (encoderValue > 0) {
            settings.presetBank = (settings.presetBank + 1) % NUM_PRESET_BANKS;
          } else {
            settings.presetBank = (settings.presetBank + NUM_PRESET_BANKS - 1) % NUM_PRESET_BANKS;
          }
          loadCurrentMode();
        }
      }
      encoderValue = 0;
      saveSettings();
    }
  }

  // Handle chord pads (0-8)
  for (int i = 0; i < NUM_CHORD_PADS; i++) {
    int btnIndex = CHORD_PAD_BUTTONS[i];

    if (keyStates[btnIndex] && !previousKeyStates[btnIndex]) {
      // Button pressed
      if (shiftState) {
        // Enter edit mode for this pad
        state.inEditMode = true;
        state.editingPad = i;
        state.editNoteIndex = 0;
      } else {
        // Stop any currently playing notes before switching pads
        if (state.activePad >= 0 && state.activePad != i) {
          if (state.arpRate > 0) {
            stopCurrentArpNote();
          }
          // Stop held chord if switching pads
          stopChord(state.activePad);
        }
        // Play chord
        padStates[i] = true;
        state.activePad = i;
        // Reset arp state when switching pads
        state.arpNoteIndex = 0;
        state.arpDirection = true;
        state.arpOctaveStep = 0;  // Reset octave cycling
      }
    } else if (!keyStates[btnIndex] && previousKeyStates[btnIndex]) {
      // Button released
      if (!shiftState && !state.inEditMode) {
        padStates[i] = false;
        // If HOLD mode is on, keep playing (don't stop notes or clear activePad)
        if (!state.holdMode && state.activePad == i) {
          // Stop any arp note that's currently playing before clearing activePad
          if (state.arpRate > 0) {
            stopCurrentArpNote();
          }
          state.activePad = -1;
        }
      }
    }
  }

  // Handle octave buttons
  if (keyStates[BTN_OCT_DOWN] && !previousKeyStates[BTN_OCT_DOWN]) {
    state.currentOctave = constrain(state.currentOctave - 1, -3, 3);
    killAllNotes();
  }
  if (keyStates[BTN_OCT_UP] && !previousKeyStates[BTN_OCT_UP]) {
    state.currentOctave = constrain(state.currentOctave + 1, -3, 3);
    killAllNotes();
  }

  // Handle arp buttons
  if (keyStates[BTN_ARP_DOWN] && !previousKeyStates[BTN_ARP_DOWN]) {
    // Stop current arp note before changing rate
    stopCurrentArpNote();
    state.arpRate = constrain(state.arpRate - 1, 0, 6);
    if (state.arpRate == 0) {
      state.arpNoteIndex = 0;
      state.arpOctaveStep = 0;  // Reset octave cycling
    }
  }
  if (keyStates[BTN_ARP_UP] && !previousKeyStates[BTN_ARP_UP]) {
    // Stop current arp note before changing rate
    stopCurrentArpNote();
    state.arpRate = constrain(state.arpRate + 1, 0, 6);
  }

  // Handle edit mode with encoder
  if (state.inEditMode) {
    if (encoderValue > 0) {
      state.editNoteIndex = (state.editNoteIndex + 1) % 8;
      encoderValue = 0;
    } else if (encoderValue < 0) {
      state.editNoteIndex = (state.editNoteIndex + 7) % 8;
      encoderValue = 0;
    }

    // Encoder click toggles note
    if (encoderState && !previousEncoderState) {
      pads[state.editingPad].chord.isActive[state.editNoteIndex] =
        !pads[state.editingPad].chord.isActive[state.editNoteIndex];
    }

    // Exit edit mode when shift released
    if (!shiftState && previousShiftState) {
      state.inEditMode = false;
      state.editingPad = -1;
    }
  }
}

//================================ MIDI PROCESSING ================================

void updateMIDI() {
  // Poll Serial1 for MIDI data (more reliable than interrupt for clock)
  while (Serial1.available()) {
    uint8_t inByte = Serial1.read();

    // Handle real-time messages immediately (single byte, 0xF8-0xFF)
    if (inByte >= 0xF8) {
      switch (inByte) {
        case 0xF8:  // MIDI Clock (24 PPQN)
          {
            midiClockReceived = true;
            midiClockCounter++;
            arpClockCount++;  // Count for arpeggiator sync
            lastClockTime = millis();

            // Calculate BPM from clock interval (average over 24 clocks = 1 beat)
            unsigned long now = micros();
            if (lastClockMicros > 0) {
              // Smooth averaging of clock intervals
              unsigned long interval = now - lastClockMicros;
              if (clockIntervalMicros == 0) {
                clockIntervalMicros = interval;
              } else {
                clockIntervalMicros = (clockIntervalMicros * 7 + interval) / 8;  // Smoothing
              }
              // BPM = 60,000,000 / (interval_micros * 24)
              if (clockIntervalMicros > 0) {
                detectedBpm = 60000000UL / (clockIntervalMicros * 24);
                detectedBpm = constrain(detectedBpm, 20, 300);
              }
            }
            lastClockMicros = now;

            // Visual indicator - pulse every beat (every 24 clocks)
            if (midiClockCounter % 24 == 0) {
              clockPulseIndicator = true;
              lastClockPulseTime = millis();
            }
          }
          break;
        case 0xFA:  // Start
          midiTransportRunning = true;
          midiClockCounter = 0;
          break;
        case 0xFB:  // Continue
          midiTransportRunning = true;
          break;
        case 0xFC:  // Stop
          midiTransportRunning = false;
          break;
      }
      continue;  // Don't process as regular MIDI
    }

    // Handle regular MIDI messages via state machine
    switch (currentMidiState) {
      case WAITING_FOR_STATUS:
        if (inByte >= 0x80 && inByte < 0xF0) {
          midiStatus = inByte;
          currentMidiState = WAITING_FOR_DATA1;
        }
        break;
      case WAITING_FOR_DATA1:
        midiData1 = inByte;
        currentMidiState = WAITING_FOR_DATA2;
        break;
      case WAITING_FOR_DATA2:
        midiData2 = inByte;
        processIncomingMIDI(midiStatus, midiData1, midiData2);
        currentMidiState = WAITING_FOR_STATUS;
        break;
    }
  }

  // Check for incoming USB MIDI
  while (usb_midi.available()) {
    uint32_t packet = usb_midi.read();
    uint8_t status = (packet >> 8) & 0xFF;
    uint8_t data1 = (packet >> 16) & 0xFF;
    uint8_t data2 = (packet >> 24) & 0xFF;

    // Handle USB MIDI clock too
    if (status == 0xF8) {
      midiClockReceived = true;
      midiClockCounter++;
      arpClockCount++;  // Count for arpeggiator sync
      lastClockTime = millis();
      if (midiClockCounter % 24 == 0) {
        clockPulseIndicator = true;
        lastClockPulseTime = millis();
      }
    } else if (status == 0xFA) {
      midiTransportRunning = true;
      midiClockCounter = 0;
    } else if (status == 0xFB) {
      midiTransportRunning = true;
    } else if (status == 0xFC) {
      midiTransportRunning = false;
    } else {
      processIncomingMIDI(status, data1, data2);
    }
  }

  // Reset clock pulse indicator after display time
  if (clockPulseIndicator && (millis() - lastClockPulseTime > 100)) {
    clockPulseIndicator = false;
  }

  // Process pad state changes (only when arp is off)
  if (state.arpRate == 0) {
    for (int i = 0; i < 9; i++) {
      if (padStates[i] && !previousPadStates[i]) {
        playChord(i);
      } else if (!padStates[i] && previousPadStates[i]) {
        // Only stop chord if HOLD mode is off
        if (!state.holdMode) {
          stopChord(i);
        }
        // If HOLD is on, chord keeps playing (activePad remains set)
      }
    }
  }
}

void processIncomingMIDI(uint8_t status, uint8_t data1, uint8_t data2) {
  uint8_t command = status & 0xF0;
  uint8_t channel = status & 0x0F;

  if (settings.midiThru) {
    forwardMIDI(status, data1, data2);
  }

  // Handle external triggers
  if (channel == settings.midiTrigChannel) {
    if (command == 0x90 && data2 > 0) {
      // Note On - find matching pad
      for (int i = 0; i < 9; i++) {
        if (data1 == pads[i].triggerNote) {
          // Stop any currently playing arp note before switching pads
          if (state.arpRate > 0 && state.activePad >= 0 && state.activePad != i) {
            stopCurrentArpNote();
          }
          padStates[i] = true;
          state.activePad = i;
          // Reset arp state when switching pads
          state.arpNoteIndex = 0;
          state.arpDirection = true;
          state.arpOctaveStep = 0;  // Reset octave cycling
        }
      }
    } else if (command == 0x80 || (command == 0x90 && data2 == 0)) {
      // Note Off
      for (int i = 0; i < 9; i++) {
        if (data1 == pads[i].triggerNote) {
          padStates[i] = false;
          if (state.activePad == i) {
            // Stop any arp note that's currently playing
            if (state.arpRate > 0) {
              stopCurrentArpNote();
            }
            state.activePad = -1;
          }
        }
      }
    }
  }
}

void forwardMIDI(uint8_t status, uint8_t data1, uint8_t data2) {
  uint8_t usb_packet[] = {status, data1, data2};
  usb_midi.write(usb_packet, 3);
  Serial1.write(status);
  Serial1.write(data1);
  Serial1.write(data2);
}

//================================ ARPEGGIATOR ================================

// Clock dividers for each arp rate (MIDI clock = 24 PPQN)
// Rate: 0=off, 1=1/2, 2=1/4, 3=1/8, 4=1/16, 5=1/32, 6=1/64
const int clockDividers[7] = {
  0,   // OFF
  48,  // 1/2 note = 48 clocks (2 beats)
  24,  // 1/4 note = 24 clocks (1 beat)
  12,  // 1/8 note = 12 clocks
  6,   // 1/16 note = 6 clocks
  3,   // 1/32 note = 3 clocks
  2    // 1/64 note = 2 clocks
};

// For gate timing
unsigned long arpNoteOnTime = 0;
bool arpGateOpen = false;

// Calculate interval with pattern modifiers (for internal timing)
int getPatternInterval(int baseInterval) {
  int step = state.arpStepInPattern % 4;

  switch (settings.arpPattern) {
    case 0: // Straight
      return baseInterval;
    case 1: // Swing
      if (step % 2 == 1) {
        return baseInterval + (settings.arpSwing - 50) * baseInterval / 100;
      }
      return baseInterval;
    case 2: // Dotted
      return (step % 2 == 0) ? baseInterval * 3 / 2 : baseInterval / 2;
    case 3: // Triplet
      return baseInterval * 2 / 3;
    case 4: // Humanize
      return max(10, baseInterval + random(-settings.arpHumanize, settings.arpHumanize + 1));
    case 5: // Stutter
      return (step % 2 == 1) ? baseInterval / 4 : baseInterval * 3 / 4;
    default:
      return baseInterval;
  }
}

// Track last clock position we triggered on (for proper grid sync)
int lastArpTriggerClock = -1;

// Generate internal MIDI clock and send out when no external clock
void updateInternalClock() {
  unsigned long currentTime = millis();

  // Only generate internal clock if no external clock present
  bool clockPresent = (currentTime - lastClockTime) < 500;
  if (clockPresent) {
    return;  // External clock active, don't generate internal
  }

  // Calculate interval between clock pulses (24 PPQN)
  // At BPM, quarter note = 60000/BPM ms, so clock pulse = 60000/(BPM*24) ms
  unsigned long clockInterval = 60000UL / (settings.internalBpm * 24);

  if (currentTime - lastInternalClockTime >= clockInterval) {
    lastInternalClockTime = currentTime;

    // Send MIDI clock out
    Serial1.write(0xF8);

    // Increment counter for internal sync
    internalClockCounter++;
    midiClockCounter = internalClockCounter;  // Sync with arp counter

    // Visual indicator - pulse every beat (every 24 clocks)
    if (internalClockCounter % 24 == 0) {
      clockPulseIndicator = true;
      lastClockPulseTime = currentTime;
    }
  }
}

void updateArpeggiator() {
  if (state.arpRate == 0 || state.activePad < 0) {
    return;
  }

  unsigned long currentTime = millis();

  // Check if external clock is active (received within last 500ms)
  bool clockPresent = (currentTime - lastClockTime) < 500;
  externalClockActive = clockPresent && settings.midiClockSync;

  // Handle gate off (note release before next trigger)
  if (arpGateOpen && arpNotePlaying) {
    int baseInterval = externalClockActive ? 80 : arpTimings[state.arpRate];
    int gateTime = max(15, baseInterval * settings.arpGate / 100);

    if (currentTime - arpNoteOnTime >= gateTime) {
      stopArpNote(state.activePad, state.arpNoteIndex);
      arpGateOpen = false;
    }
  }

  bool shouldTrigger = false;

  if (externalClockActive) {
    // QUANTIZED clock sync: trigger on clock grid positions
    // Use global midiClockCounter to stay locked to transport
    int divider = clockDividers[state.arpRate];

    if (divider > 0) {
      // Calculate which "slot" we're on in the clock grid
      int currentSlot = midiClockCounter / divider;
      int lastSlot = lastArpTriggerClock / divider;

      // Trigger when we enter a new slot (quantized to grid)
      if (currentSlot != lastSlot || lastArpTriggerClock < 0) {
        shouldTrigger = true;
        lastArpTriggerClock = midiClockCounter;
      }
    }
  } else {
    // Internal timing
    int baseInterval = arpTimings[state.arpRate];
    int interval = getPatternInterval(baseInterval);

    if (currentTime - state.lastArpTime >= interval) {
      state.lastArpTime = currentTime;
      shouldTrigger = true;
    }
  }

  if (shouldTrigger) {
    // Stop previous note if gate still open
    if (arpGateOpen) {
      stopArpNote(lastArpPad, lastArpNoteIndex);
    }

    // Advance to next note
    advanceArpIndex(state.activePad);
    state.arpStepInPattern++;

    // Play new note
    playArpNote(state.activePad, state.arpNoteIndex);
    arpNoteOnTime = currentTime;
    arpGateOpen = true;
  }
}

void advanceArpIndex(int pad) {
  ChordV2& chord = pads[pad].chord;

  // Count active notes (limited by maxNotesPerChord)
  int activeCount = 0;
  int activeIndices[8];
  for (int i = 0; i < 8; i++) {
    if (chord.isActive[i]) {
      if (activeCount < settings.maxNotesPerChord) {
        activeIndices[activeCount++] = i;
      }
    }
  }

  if (activeCount == 0) return;

  // Find current position in active notes
  int currentPos = 0;
  for (int i = 0; i < activeCount; i++) {
    if (activeIndices[i] == state.arpNoteIndex) {
      currentPos = i;
      break;
    }
  }

  int prevPos = currentPos;

  switch (state.arpMode) {
    case 0: // Up
      currentPos = (currentPos + 1) % activeCount;
      break;

    case 1: // Down
      currentPos = (currentPos - 1 + activeCount) % activeCount;
      break;

    case 2: // Up-Down (ping-pong)
      if (state.arpDirection) {
        currentPos++;
        if (currentPos >= activeCount - 1) {
          state.arpDirection = false;
        }
      } else {
        currentPos--;
        if (currentPos <= 0) {
          state.arpDirection = true;
        }
      }
      currentPos = constrain(currentPos, 0, activeCount - 1);
      break;

    case 3: // Down-Up (reverse ping-pong)
      if (!state.arpDirection) {
        currentPos--;
        if (currentPos <= 0) {
          state.arpDirection = true;
        }
      } else {
        currentPos++;
        if (currentPos >= activeCount - 1) {
          state.arpDirection = false;
        }
      }
      currentPos = constrain(currentPos, 0, activeCount - 1);
      break;

    case 4: // Random
      currentPos = random(activeCount);
      break;

    case 5: // Order (play in slot order 0-7)
      currentPos = (currentPos + 1) % activeCount;
      break;

    case 6: // Chord (play all notes - handled specially, just cycle)
      currentPos = (currentPos + 1) % activeCount;
      break;

    case 7: // 2Oct (up through 2 octaves then reset)
      currentPos = (currentPos + 1) % activeCount;
      // Octave stepping handled by arpOctaveStep
      break;
  }

  state.arpNoteIndex = activeIndices[currentPos];

  // Advance octave step when wrapping around (completing a cycle)
  bool wrapped = (currentPos == 0 && prevPos != 0);
  if (wrapped && settings.arpOctaveRange > 0) {
    state.arpOctaveStep++;
  }
}

void playArpNote(int pad, int noteIndex) {
  if (pad < 0 || pad >= 9) return;
  if (!pads[pad].chord.isActive[noteIndex]) return;

  ChordV2& chord = pads[pad].chord;
  int note = settings.rootNote + chord.rootOffset + chord.intervals[noteIndex]
             + (chord.octaveModifiers[noteIndex] * 12) + (state.currentOctave * 12);

  // Apply octave range modifier
  // 0=Off, 1=+1, 2=+2, 3=+3, 4=+4, 5=+5, 6=-1, 7=-2, 8=+/-1
  int octaveShift = 0;
  switch (settings.arpOctaveRange) {
    case 1: // +1 octave cycling
      octaveShift = (state.arpOctaveStep % 2) * 12;
      break;
    case 2: // +2 octaves cycling
      octaveShift = (state.arpOctaveStep % 3) * 12;
      break;
    case 3: // +3 octaves cycling
      octaveShift = (state.arpOctaveStep % 4) * 12;
      break;
    case 4: // +4 octaves cycling
      octaveShift = (state.arpOctaveStep % 5) * 12;
      break;
    case 5: // +5 octaves cycling
      octaveShift = (state.arpOctaveStep % 6) * 12;
      break;
    case 6: // -1 octave cycling
      octaveShift = (state.arpOctaveStep % 2) * -12;
      break;
    case 7: // -2 octaves cycling
      octaveShift = (state.arpOctaveStep % 3) * -12;
      break;
    case 8: // +/-1 octave cycling
      {
        int cycle = state.arpOctaveStep % 3;
        octaveShift = (cycle == 0) ? 0 : (cycle == 1) ? 12 : -12;
      }
      break;
  }
  note += octaveShift;
  note = constrain(note, 0, 127);

  // Calculate velocity with variation
  int baseVelocity = settings.velocityScaling * (pads[pad].velocity + chord.velocityModifiers[noteIndex]);

  // Add velocity variation if enabled
  if (settings.arpVelocityVar > 0) {
    int variation = random(-settings.arpVelocityVar, settings.arpVelocityVar + 1);
    baseVelocity += variation;
  }

  // Add accent on first beat of pattern
  if (state.arpStepInPattern % 4 == 0) {
    baseVelocity += 10;  // Slight accent
  }

  int velocity = constrain(baseVelocity, 1, 127);

  int channel = chord.channel[noteIndex];
  sendNoteOn(note, velocity, getOutputChannel(channel));

  // Track what's playing so we can stop it later (store actual MIDI note!)
  lastArpPad = pad;
  lastArpNoteIndex = noteIndex;
  lastArpNoteMidi = note;  // Store the actual note with octave shift applied
  lastArpNoteChannel = getOutputChannel(channel);
  arpNotePlaying = true;
}

void stopArpNote(int pad, int noteIndex) {
  // Use the stored MIDI note - this includes octave shift that was applied during playback
  if (lastArpNoteMidi >= 0) {
    sendNoteOff(lastArpNoteMidi, 0, lastArpNoteChannel);
  }
}

// Stop whatever arp note is currently playing
void stopCurrentArpNote() {
  if (arpNotePlaying && lastArpNoteMidi >= 0) {
    sendNoteOff(lastArpNoteMidi, 0, lastArpNoteChannel);
    arpNotePlaying = false;
    lastArpPad = -1;
    lastArpNoteIndex = -1;
    lastArpNoteMidi = -1;
  }
}

//================================ CHORD PLAYBACK ================================

void playChord(int pad) {
  ChordV2& chord = pads[pad].chord;

  int notesPlayed = 0;
  for (int j = 0; j < 8; j++) {
    if (chord.isActive[j]) {
      if (notesPlayed < settings.maxNotesPerChord) {
        playNote(pad, j);
        notesPlayed++;
      }
    }
  }
}

void playNote(int pad, int noteIndex) {
  ChordV2& chord = pads[pad].chord;

  int note = settings.rootNote + chord.rootOffset + chord.intervals[noteIndex]
             + (chord.octaveModifiers[noteIndex] * 12) + (state.currentOctave * 12);

  note = constrain(note, 0, 127);

  int velocity = constrain(
    settings.velocityScaling * (pads[pad].velocity + chord.velocityModifiers[noteIndex]
    + random(-pads[pad].velocityVariation, pads[pad].velocityVariation + 1)),
    1, 127
  );

  int channelIndex = chord.channel[noteIndex];
  int outputChannel = getOutputChannel(channelIndex);

  // Reference counting
  switch (channelIndex) {
    case 0:
      if (noteCountA[note] > 0) sendNoteOff(note, 0, outputChannel);
      noteCountA[note]++;
      break;
    case 1:
      if (noteCountB[note] > 0) sendNoteOff(note, 0, outputChannel);
      noteCountB[note]++;
      break;
    case 2:
      if (noteCountC[note] > 0) sendNoteOff(note, 0, outputChannel);
      noteCountC[note]++;
      break;
    case 3:
      if (noteCountD[note] > 0) sendNoteOff(note, 0, outputChannel);
      noteCountD[note]++;
      break;
  }

  sendNoteOn(note, velocity, outputChannel);
}

void stopChord(int pad) {
  ChordV2& chord = pads[pad].chord;

  for (int j = 0; j < 8; j++) {
    if (chord.isActive[j]) {
      stopNote(pad, j);
    }
  }
}

void stopNote(int pad, int noteIndex) {
  ChordV2& chord = pads[pad].chord;

  int note = settings.rootNote + chord.rootOffset + chord.intervals[noteIndex]
             + (chord.octaveModifiers[noteIndex] * 12) + (state.currentOctave * 12);

  note = constrain(note, 0, 127);

  int channelIndex = chord.channel[noteIndex];
  int outputChannel = getOutputChannel(channelIndex);

  // Reference counting
  switch (channelIndex) {
    case 0:
      if (noteCountA[note] > 0) {
        noteCountA[note]--;
        if (noteCountA[note] == 0) sendNoteOff(note, 0, outputChannel);
      }
      break;
    case 1:
      if (noteCountB[note] > 0) {
        noteCountB[note]--;
        if (noteCountB[note] == 0) sendNoteOff(note, 0, outputChannel);
      }
      break;
    case 2:
      if (noteCountC[note] > 0) {
        noteCountC[note]--;
        if (noteCountC[note] == 0) sendNoteOff(note, 0, outputChannel);
      }
      break;
    case 3:
      if (noteCountD[note] > 0) {
        noteCountD[note]--;
        if (noteCountD[note] == 0) sendNoteOff(note, 0, outputChannel);
      }
      break;
  }
}

int getOutputChannel(int index) {
  switch (index) {
    case 0: return settings.midiOutputAChannel;
    case 1: return settings.midiOutputBChannel;
    case 2: return settings.midiOutputCChannel;
    case 3: return settings.midiOutputDChannel;
    default: return settings.midiOutputAChannel;
  }
}

void sendNoteOn(int note, int velocity, int channel) {
  if (channel < 0 || channel > 15) return;
  uint8_t status = 0x90 | channel;
  uint8_t usb_packet[] = {status, (uint8_t)note, (uint8_t)velocity};
  usb_midi.write(usb_packet, 3);
  Serial1.write(status);
  Serial1.write(note);
  Serial1.write(velocity);
}

void sendNoteOff(int note, int velocity, int channel) {
  if (channel < 0 || channel > 15) return;
  uint8_t status = 0x80 | channel;
  uint8_t usb_packet[] = {status, (uint8_t)note, (uint8_t)velocity};
  usb_midi.write(usb_packet, 3);
  Serial1.write(status);
  Serial1.write(note);
  Serial1.write(velocity);
}

void killAllNotes() {
  for (int i = 0; i < 128; i++) {
    if (noteCountA[i] > 0) sendNoteOff(i, 0, settings.midiOutputAChannel);
    noteCountA[i] = 0;
    if (noteCountB[i] > 0) sendNoteOff(i, 0, settings.midiOutputBChannel);
    noteCountB[i] = 0;
    if (noteCountC[i] > 0) sendNoteOff(i, 0, settings.midiOutputCChannel);
    noteCountC[i] = 0;
    if (noteCountD[i] > 0) sendNoteOff(i, 0, settings.midiOutputDChannel);
    noteCountD[i] = 0;
  }
}

//================================ VISUALS ================================

void updateVisuals() {
  pixels.clear();

  // Shift key LED (pixel 0)
  if (shiftState) {
    pixels.setPixelColor(0, COLOR_SHIFT);
  } else if (state.inEditMode) {
    pixels.setPixelColor(0, dimColor(COLOR_SHIFT, dimFactor));
  }

  // Chord pad LEDs - map to physical button positions
  // Layout: buttons 0,1,2 (row0), 4,5,6 (row1), 8,9,10 (row2) are chord pads
  // Pixel index = button index + 1
  for (int i = 0; i < 9; i++) {
    int btnIndex = CHORD_PAD_BUTTONS[i];
    int pixelIndex = btnIndex + 1;
    uint32_t color;

    if (state.inEditMode && state.editingPad == i) {
      // Editing this pad - pulse effect
      float pulse = (sin(millis() * 0.01) + 1) * 0.5;
      color = dimColor(COLOR_EDIT, 0.3 + pulse * 0.7);
    } else if (padStates[i] || (state.activePad == i && state.arpRate > 0)) {
      // Playing
      color = COLOR_PLAYING;
    } else {
      // Idle - show pad color
      color = padColors[i];
    }

    pixels.setPixelColor(pixelIndex, color);
  }

  // Column 4 buttons (3, 7, 11) - special functions
  // Button 3 = settings toggle
  if (state.inSettingsMode) {
    float pulse = (sin(millis() * 0.01) + 1) * 0.5;
    pixels.setPixelColor(4, dimColor(0xFFFF00, 0.3 + pulse * 0.7));  // Yellow pulse
  } else {
    pixels.setPixelColor(4, dimColor(0xFFFF00, 0.2));  // Dim yellow
  }

  // Button 7 = HOLD toggle
  if (state.holdMode) {
    float pulse = (sin(millis() * 0.008) + 1) * 0.5;
    pixels.setPixelColor(8, dimColor(0xFF00FF, 0.4 + pulse * 0.6));  // Magenta pulse when active
  } else {
    pixels.setPixelColor(8, dimColor(0xFF00FF, 0.15));  // Dim magenta
  }

  // Button 11 = arp settings toggle
  if (state.inArpSettings) {
    float pulse = (sin(millis() * 0.01) + 1) * 0.5;
    pixels.setPixelColor(12, dimColor(0x00FFFF, 0.3 + pulse * 0.7));  // Cyan pulse
  } else {
    pixels.setPixelColor(12, dimColor(0x00FFFF, 0.2));  // Dim cyan
  }

  // Bottom row controls
  // Octave buttons (12, 13) -> pixels 13, 14
  uint32_t octDownColor = (state.currentOctave > -3) ? COLOR_OCTAVE : dimColor(COLOR_OCTAVE, 0.1);
  uint32_t octUpColor = (state.currentOctave < 3) ? COLOR_OCTAVE : dimColor(COLOR_OCTAVE, 0.1);
  pixels.setPixelColor(13, octDownColor);  // Oct-
  pixels.setPixelColor(14, octUpColor);    // Oct+

  // Arp buttons (14, 15) -> pixels 15, 16
  uint32_t arpColor;
  if (state.arpRate > 0) {
    float brightness = 0.3 + (state.arpRate / 6.0) * 0.7;
    arpColor = dimColor(COLOR_ARP, brightness);
  } else {
    arpColor = dimColor(COLOR_ARP, 0.2);
  }
  pixels.setPixelColor(15, arpColor);  // Arp-
  pixels.setPixelColor(16, arpColor);  // Arp+

  pixels.show();
}

uint32_t dimColor(uint32_t color, float factor) {
  uint8_t red = (color >> 16) & 0xFF;
  uint8_t green = (color >> 8) & 0xFF;
  uint8_t blue = color & 0xFF;

  red = (uint8_t)(red * factor);
  green = (uint8_t)(green * factor);
  blue = (uint8_t)(blue * factor);

  return (red << 16) | (green << 8) | blue;
}

//================================ DISPLAY ================================

void updateDisplay() {
  display.clearDisplay();

  if (state.inSettingsMode) {
    drawSettingsScreen();
  } else if (state.inArpSettings) {
    drawArpSettingsScreen();
  } else if (state.inMaxNotesMenu) {
    drawMaxNotesScreen();
  } else if (state.inEditMode) {
    drawEditScreen();
  } else {
    drawMainScreen();
  }

  display.display();
}

void drawArpSettingsScreen() {
  // Compact header with page dots
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("ARP");

  // Page indicator dots (7 pages)
  for (int i = 0; i <= 6; i++) {
    int x = 30 + i * 10;
    if (i == state.arpSettingsPage) {
      display.fillRect(x, 2, 6, 6, WHITE);
    } else {
      display.drawRect(x, 2, 6, 6, WHITE);
    }
  }

  // Current arp rate on right
  display.setCursor(100, 0);
  display.print(arpRateNames[state.arpRate]);

  // Setting label (small)
  display.setTextSize(1);
  display.setCursor(0, 14);

  const char* labels[7] = {"PATTERN", "GATE", "SWING", "HUMAN", "VEL", "OCT", "MODE"};
  display.print(labels[state.arpSettingsPage]);

  // Value (large, centered)
  display.setTextSize(2);
  char valueStr[16];

  switch (state.arpSettingsPage) {
    case 0: // Pattern
      snprintf(valueStr, sizeof(valueStr), "%s", arpPatternNames[settings.arpPattern]);
      break;
    case 1: // Gate
      snprintf(valueStr, sizeof(valueStr), "%d%%", settings.arpGate);
      break;
    case 2: // Swing
      snprintf(valueStr, sizeof(valueStr), "%d%%", settings.arpSwing);
      break;
    case 3: // Humanize
      snprintf(valueStr, sizeof(valueStr), "%dms", settings.arpHumanize);
      break;
    case 4: // Velocity Var
      snprintf(valueStr, sizeof(valueStr), "+/-%d", settings.arpVelocityVar);
      break;
    case 5: // Octave
      snprintf(valueStr, sizeof(valueStr), "%s", arpOctaveNames[settings.arpOctaveRange]);
      break;
    case 6: // Mode
      snprintf(valueStr, sizeof(valueStr), "%s", arpModeNames[state.arpMode]);
      break;
  }

  int len = strlen(valueStr);
  int xPos = 64 - (len * 6);  // Center
  display.setCursor(xPos, 28);
  display.print(valueStr);

  // Visual bar for percentage values
  if (state.arpSettingsPage >= 1 && state.arpSettingsPage <= 4) {
    int barVal = 0;
    switch (state.arpSettingsPage) {
      case 1: barVal = settings.arpGate; break;
      case 2: barVal = settings.arpSwing; break;
      case 3: barVal = settings.arpHumanize * 2; break;  // Scale 0-50 to 0-100
      case 4: barVal = settings.arpVelocityVar * 2; break;
    }
    int barWidth = barVal * 120 / 100;
    display.drawRect(4, 48, 120, 6, WHITE);
    display.fillRect(4, 48, barWidth, 6, WHITE);
  }

  // Bottom hint
  display.setTextSize(1);
  display.setCursor(0, 56);
  display.print("Rotate=Adj Click=Next");
}

void drawMaxNotesScreen() {
  // Header
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("MAX NOTES");

  // Big number in center
  display.setTextSize(4);
  display.setCursor(52, 18);
  display.print(settings.maxNotesPerChord);

  // Visual indicator - dots showing max notes
  display.setTextSize(1);
  int dotY = 54;
  int startX = 64 - (settings.maxNotesPerChord * 8 / 2);
  for (int i = 0; i < settings.maxNotesPerChord; i++) {
    display.fillCircle(startX + i * 8, dotY, 3, WHITE);
  }
  // Show empty slots
  for (int i = settings.maxNotesPerChord; i < 8; i++) {
    display.drawCircle(startX + i * 8, dotY, 3, WHITE);
  }
}

// Draw 8-bit piano keyboard with active notes highlighted (1 octave)
void drawPianoKeyboard(int baseNote, uint8_t* activeNotes, int numActive) {
  // Piano spans 1 octave starting from baseNote rounded to C
  int startNote = (baseNote / 12) * 12;  // Round down to nearest C

  // Key dimensions for 1 octave centered on 128px screen
  const int whiteKeyW = 16;  // White key width (7 keys * 16 = 112px)
  const int whiteKeyH = 36;  // White key height
  const int blackKeyW = 10;  // Black key width
  const int blackKeyH = 22;  // Black key height
  const int keyboardX = 8;   // Left margin to center
  const int keyboardY = 16;  // Y position of keyboard

  // Draw white keys first (C D E F G A B = 7 keys)
  int whiteKeyIndex = 0;
  for (int note = 0; note < 12; note++) {
    int midiNote = startNote + note;

    // Only white keys: C(0) D(2) E(4) F(5) G(7) A(9) B(11)
    if (note == 0 || note == 2 || note == 4 || note == 5 || note == 7 || note == 9 || note == 11) {
      int x = keyboardX + (whiteKeyIndex * whiteKeyW);

      // Check if this note is active (check all octaves)
      bool isActive = false;
      for (int i = 0; i < numActive; i++) {
        if ((activeNotes[i] % 12) == note) {
          isActive = true;
          break;
        }
      }

      if (isActive) {
        display.fillRect(x, keyboardY, whiteKeyW - 1, whiteKeyH, WHITE);
      } else {
        display.drawRect(x, keyboardY, whiteKeyW - 1, whiteKeyH, WHITE);
      }
      whiteKeyIndex++;
    }
  }

  // Draw black keys on top (C# D# F# G# A# = 5 keys)
  // Positions after white keys: C#(after C), D#(after D), F#(after F), G#(after G), A#(after A)
  const int blackKeyPositions[5] = {0, 1, 3, 4, 5};  // After which white key
  const int blackNotes[5] = {1, 3, 6, 8, 10};        // Note numbers (C#, D#, F#, G#, A#)

  for (int i = 0; i < 5; i++) {
    int x = keyboardX + (blackKeyPositions[i] * whiteKeyW) + whiteKeyW - (blackKeyW / 2);

    // Check if this black note is active
    bool isActive = false;
    for (int j = 0; j < numActive; j++) {
      if ((activeNotes[j] % 12) == blackNotes[i]) {
        isActive = true;
        break;
      }
    }

    if (isActive) {
      display.fillRect(x, keyboardY, blackKeyW, blackKeyH, WHITE);
    } else {
      display.fillRect(x, keyboardY, blackKeyW, blackKeyH, BLACK);
      display.drawRect(x, keyboardY, blackKeyW, blackKeyH, WHITE);
    }
  }
}

void drawMainScreen() {
  // Playing state - show piano keyboard with pressed notes
  if (state.activePad >= 0) {
    ChordV2& chord = pads[state.activePad].chord;

    // Top-left: root key info (tiny)
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(midiNoteNames[settings.rootNote]);

    // Top-center: chord/scale info
    display.setCursor(40, 0);
    if (settings.playMode == 1) {
      display.print(presetBankInfo[settings.presetBank].chordNames[state.activePad]);
    } else {
      display.print(scaleNames[settings.scaleType]);
    }

    // Top-right: pad number
    display.setCursor(116, 0);
    display.print(state.activePad + 1);

    // Build array of currently playing MIDI notes
    uint8_t playingNotes[8];
    int numPlaying = 0;
    int chordRoot = settings.rootNote + chord.rootOffset + (state.currentOctave * 12);
    chordRoot = constrain(chordRoot, 0, 127);

    for (int i = 0; i < 8; i++) {
      if (chord.isActive[i]) {
        int note = chordRoot + chord.intervals[i];
        if (note >= 0 && note <= 127) {
          playingNotes[numPlaying++] = note;
        }
      }
    }

    // Draw piano keyboard with active notes
    drawPianoKeyboard(chordRoot, playingNotes, numPlaying);

    // Bottom bar: hold + arp indicator + octave
    display.setTextSize(1);
    display.setCursor(0, 56);
    if (state.holdMode) {
      display.print("HLD ");
    }
    if (state.arpRate > 0) {
      display.print("ARP ");
      display.print(arpRateNames[state.arpRate]);
    }

    // Octave indicator on right
    display.setCursor(100, 56);
    if (state.currentOctave != 0) {
      display.print("Oct");
      if (state.currentOctave > 0) display.print("+");
      display.print(state.currentOctave);
    }

  } else {
    // IDLE state - clean minimal design

    // Top bar: mode indicator + clock pulse
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(settings.playMode == 1 ? "PRE" : "SCL");

    // Clock indicator dot (blinks on beat)
    if (settings.midiClockSync) {
      if (clockPulseIndicator && (millis() - lastClockPulseTime < 100)) {
        // Filled dot when receiving clock pulse
        display.fillCircle(122, 4, 4, WHITE);
      } else if (externalClockActive) {
        // Hollow dot when clock active but not on beat
        display.drawCircle(122, 4, 4, WHITE);
      } else {
        // Small dot waiting for clock
        display.drawCircle(122, 4, 2, WHITE);
      }
    }

    // Main content area - centered
    if (settings.playMode == 1) {
      // PRESET mode - bank name centered
      display.setTextSize(2);
      const char* bankName = presetBankInfo[settings.presetBank].name;
      int nameLen = strlen(bankName);
      int xPos = 64 - (nameLen * 6);  // Center the text
      display.setCursor(xPos, 18);
      display.print(bankName);

    } else {
      // SCALE mode - root + scale on same line, centered
      display.setTextSize(2);

      // Build display string: "C4 Maj" or "F#3 Min"
      char displayStr[16];
      snprintf(displayStr, sizeof(displayStr), "%s %s",
               midiNoteNames[settings.rootNote],
               scaleNames[settings.scaleType]);

      int strLen = strlen(displayStr);
      int xPos = max(0, 64 - (strLen * 6));
      display.setCursor(xPos, 18);
      display.print(displayStr);
    }

    // Bottom bar: compact status
    display.setTextSize(1);

    // Left: octave with visual indicator
    display.setCursor(0, 56);
    if (state.currentOctave < 0) {
      for (int i = 0; i > state.currentOctave; i--) display.print("<");
    } else if (state.currentOctave > 0) {
      for (int i = 0; i < state.currentOctave; i++) display.print(">");
    } else {
      display.print("-");
    }

    // Center-left: HOLD indicator
    if (state.holdMode) {
      display.setCursor(30, 56);
      display.print("HLD");
    }

    // Center: arp rate (if active)
    if (state.arpRate > 0) {
      display.setCursor(60, 56);
      display.print(arpRateNames[state.arpRate]);
    }

    // Right: channel
    display.setCursor(108, 56);
    display.print(settings.midiOutputAChannel + 1);
  }
}

void drawEditScreen() {
  ChordV2& chord = pads[state.editingPad].chord;

  // Header - big pad number
  display.setTextSize(2);
  display.setCursor(4, 2);
  display.print("PAD");
  display.setTextSize(3);
  display.setCursor(52, 0);
  display.print(state.editingPad + 1);

  // 8 note slots as vertical bars
  const char* noteLabels[8] = {"1", "3", "5", "7", "9", "11", "13", "8"};

  for (int i = 0; i < 8; i++) {
    int x = 4 + (i * 15);
    int y = 28;

    // Selected indicator
    if (i == state.editNoteIndex) {
      // Arrow above
      display.fillTriangle(x + 6, y - 6, x + 2, y - 2, x + 10, y - 2, WHITE);
    }

    // Note bar
    if (chord.isActive[i]) {
      // Filled bar with height based on interval
      int barHeight = 20 + (chord.intervals[i] / 3);
      barHeight = constrain(barHeight, 16, 34);
      display.fillRect(x, 64 - barHeight, 12, barHeight, WHITE);

      // Label inverted
      display.setTextColor(BLACK);
      display.setTextSize(1);
      display.setCursor(x + 2, 64 - barHeight + 2);
      display.print(noteLabels[i]);
      display.setTextColor(WHITE);
    } else {
      // Empty box
      display.drawRect(x, 48, 12, 16, WHITE);
      display.setTextSize(1);
      display.setCursor(x + 4, 52);
      display.print("-");
    }
  }

  // Selected note info on right
  display.setTextSize(1);
  display.setCursor(90, 28);
  display.print("NOTE");
  display.setTextSize(2);
  display.setCursor(90, 38);
  if (chord.isActive[state.editNoteIndex]) {
    display.print("ON");
  } else {
    display.print("--");
  }
}

void drawSettingsScreen() {
  int maxPages = 3;  // Mode, Channel, BPM

  // Page indicator dots at top
  for (int i = 0; i < maxPages; i++) {
    int x = 64 - ((maxPages * 6) / 2) + (i * 12);
    if (i == state.settingsPage) {
      display.fillCircle(x, 4, 3, WHITE);
    } else {
      display.drawCircle(x, 4, 2, WHITE);
    }
  }

  // Animated arrows
  int arrowOffset = (millis() / 200) % 2;
  display.fillTriangle(8, 32, 16 + arrowOffset, 24, 16 + arrowOffset, 40, WHITE);
  display.fillTriangle(120, 32, 112 - arrowOffset, 24, 112 - arrowOffset, 40, WHITE);

  const char* label = "";

  // Pages: 0=Mode, 1=Channel, 2=BPM (same for both modes)
  switch (state.settingsPage) {
    case 0:  // Play Mode
      display.setTextSize(2);
      display.setCursor(settings.playMode == 0 ? 20 : 16, 20);
      display.print(settings.playMode == 0 ? "SCALE" : "PRESET");
      label = "PLAY MODE";
      break;
    case 1:  // MIDI Channel
      display.setTextSize(4);
      display.setCursor((settings.midiOutputAChannel + 1 < 10) ? 52 : 36, 18);
      display.print(settings.midiOutputAChannel + 1);
      label = "MIDI CHANNEL";
      break;
    case 2:  // BPM
      {
        int displayBpm = externalClockActive ? detectedBpm : settings.internalBpm;
        display.setTextSize(3);
        int xPos = (displayBpm >= 100) ? 28 : 40;
        display.setCursor(xPos, 16);
        display.print(displayBpm);

        // Clock indicator dot (shows sync status)
        if (externalClockActive) {
          // Pulsing dot for external clock
          if (clockPulseIndicator && (millis() - lastClockPulseTime < 100)) {
            display.fillCircle(110, 24, 6, WHITE);
          } else {
            display.drawCircle(110, 24, 6, WHITE);
          }
          label = "BPM (SYNC)";
        } else {
          // Small dot for internal
          display.fillCircle(110, 24, 3, WHITE);
          label = "BPM (INT)";
        }
      }
      break;
  }

  // Bottom label
  display.fillRect(0, 56, 128, 8, WHITE);
  display.setTextColor(BLACK);
  display.setTextSize(1);
  int labelLen = strlen(label);
  display.setCursor(64 - (labelLen * 3), 57);
  display.print(label);
  display.setTextColor(WHITE);
}

//================================ STORAGE ================================

void loadSettings() {
  File file = LittleFS.open("/v2settings.bin", "r");
  if (file) {
    file.read((uint8_t*)&settings, sizeof(SettingsV2));
    file.close();
  }
}

void saveSettings() {
  File file = LittleFS.open("/v2settings.bin", "w");
  if (file) {
    file.write((uint8_t*)&settings, sizeof(SettingsV2));
    file.close();
  }
}

void initPadsFromPreset() {
  loadCurrentMode();
}

void loadCurrentMode() {
  if (settings.playMode == 0) {
    // SCALE mode - generate diatonic chords from root + scale
    loadScaleMode();
  } else {
    // PRESET mode - load preset bank
    loadPresetBank();
  }
}

void loadPresetBank() {
  // Constrain bank to valid range
  settings.presetBank = constrain(settings.presetBank, 0, NUM_PRESET_BANKS - 1);

  // Get pointer to selected bank
  const ChordV2* bank = presetBanks[settings.presetBank];

  for (int i = 0; i < 9; i++) {
    pads[i].color = padColors[i];
    pads[i].triggerNote = settings.rootNote + i;
    pads[i].velocity = 100;
    pads[i].velocityVariation = 0;

    // Copy chord from selected bank
    pads[i].chord = bank[i];
  }
}

void loadScaleMode() {
  // Generate 9 diatonic chords from the selected scale
  // Pads 1-7: Scale degrees I through VII
  // Pad 8: V7 (dominant 7th)
  // Pad 9: Imaj7 or Im7

  settings.scaleType = constrain(settings.scaleType, 0, NUM_SCALES - 1);
  int noteCount = scaleNoteCounts[settings.scaleType];

  for (int i = 0; i < 9; i++) {
    pads[i].color = padColors[i];
    pads[i].triggerNote = settings.rootNote + i;
    pads[i].velocity = 100;
    pads[i].velocityVariation = 0;

    // Build chord for this scale degree
    ChordV2& chord = pads[i].chord;

    // Reset chord
    for (int j = 0; j < 8; j++) {
      chord.intervals[j] = 0;
      chord.octaveModifiers[j] = 0;
      chord.velocityModifiers[j] = 0;
      chord.isActive[j] = false;
      chord.channel[j] = 0;
    }

    int scaleDegree = i % noteCount;  // Wrap for scales with fewer notes

    if (i < 7 && noteCount >= 7) {
      // Diatonic triads for degrees I-VII
      int root = scaleIntervals[settings.scaleType][scaleDegree];
      int third = scaleIntervals[settings.scaleType][(scaleDegree + 2) % noteCount];
      int fifth = scaleIntervals[settings.scaleType][(scaleDegree + 4) % noteCount];

      // Handle octave wrapping
      if (third < root) third += 12;
      if (fifth < root) fifth += 12;

      chord.rootOffset = root;
      chord.intervals[0] = 0;                    // Root
      chord.intervals[1] = third - root;         // 3rd
      chord.intervals[2] = fifth - root;         // 5th
      chord.intervals[3] = 12;                   // Octave
      chord.intervals[4] = (third - root) + 12;  // 3rd up octave

      chord.velocityModifiers[0] = 0;
      chord.velocityModifiers[1] = -5;
      chord.velocityModifiers[2] = -5;
      chord.velocityModifiers[3] = -10;
      chord.velocityModifiers[4] = -15;

      chord.isActive[0] = true;
      chord.isActive[1] = true;
      chord.isActive[2] = true;
      chord.isActive[3] = true;
      chord.isActive[4] = false;

    } else if (i == 7) {
      // Pad 8: V7 (dominant seventh)
      int fifth = (noteCount >= 5) ? scaleIntervals[settings.scaleType][4] : 7;
      chord.rootOffset = fifth;
      chord.intervals[0] = 0;   // Root
      chord.intervals[1] = 4;   // Major 3rd
      chord.intervals[2] = 7;   // 5th
      chord.intervals[3] = 10;  // b7
      chord.intervals[4] = 12;  // Octave

      chord.velocityModifiers[0] = 0;
      chord.velocityModifiers[1] = -5;
      chord.velocityModifiers[2] = -5;
      chord.velocityModifiers[3] = -5;
      chord.velocityModifiers[4] = -10;

      chord.isActive[0] = true;
      chord.isActive[1] = true;
      chord.isActive[2] = true;
      chord.isActive[3] = true;
      chord.isActive[4] = false;

    } else if (i == 8) {
      // Pad 9: Imaj7 or Im7 depending on scale
      chord.rootOffset = 0;
      chord.intervals[0] = 0;   // Root

      // Check if scale has major or minor 3rd
      int thirdInterval = (noteCount >= 3) ? scaleIntervals[settings.scaleType][2] : 4;
      bool isMajorThird = (thirdInterval == 4);

      chord.intervals[1] = thirdInterval;  // 3rd from scale
      chord.intervals[2] = 7;              // 5th
      chord.intervals[3] = isMajorThird ? 11 : 10;  // maj7 or m7
      chord.intervals[4] = 12;             // Octave

      chord.velocityModifiers[0] = 0;
      chord.velocityModifiers[1] = -5;
      chord.velocityModifiers[2] = -5;
      chord.velocityModifiers[3] = -5;
      chord.velocityModifiers[4] = -10;

      chord.isActive[0] = true;
      chord.isActive[1] = true;
      chord.isActive[2] = true;
      chord.isActive[3] = true;
      chord.isActive[4] = true;
    }
  }
}

//================================ INTERRUPTS ================================

void updateEncoder() {
  int MSB = digitalRead(ENCODER_A);
  int LSB = digitalRead(ENCODER_B);
  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;

  stepCounter += encoderStates[sum];

  if (abs(stepCounter) >= 4) {
    if (stepCounter > 0) {
      encoderValue++;
    } else {
      encoderValue--;
    }
    stepCounter = 0;
  }

  lastEncoded = encoded;
}

void midiInterruptHandler() {
  // No longer used - MIDI is polled in updateMIDI() for better clock handling
  // Keep function to avoid removing interrupt attachment
}
