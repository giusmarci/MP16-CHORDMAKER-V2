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
};

// V2 Runtime state
struct RuntimeState {
  int currentOctave = 0;          // -3 to +3
  int arpRate = 0;                // 0 = OFF, 1-6 = rates
  int arpMode = 0;                // 0=up, 1=down, 2=updown, 3=random
  bool arpDirection = true;       // true = up, false = down (for updown mode)
  int arpNoteIndex = 0;
  unsigned long lastArpTime = 0;
  bool inEditMode = false;
  int editingPad = -1;
  int editNoteIndex = 0;
  int activePad = -1;             // Currently playing pad (-1 = none)
  bool introComplete = false;
  bool inSettingsMode = false;    // Settings mode
  int settingsPage = 0;           // 0=Mode, 1=Root/Bank, 2=Scale(if scale mode), 3=Channel, 4=Clock
};

// Play mode names
const char* playModeNames[2] = {"SCALE", "PRESET"};

// MIDI Clock state
volatile bool midiClockReceived = false;    // Flag set by interrupt when clock pulse received
volatile int midiClockCounter = 0;          // Counts clock pulses (24 PPQN)
volatile bool midiTransportRunning = false; // Start/Stop state
volatile unsigned long lastClockTime = 0;   // For detecting clock presence
bool externalClockActive = false;           // True when receiving valid external clock

// Arpeggiator note tracking (to prevent stuck notes)
int lastArpPad = -1;                        // Which pad the last arp note was from
int lastArpNoteIndex = -1;                  // Which note index was last played
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
  // Check for Shift + Button 3 to enter/exit settings mode
  if (shiftState && keyStates[3] && !previousKeyStates[3]) {
    state.inSettingsMode = !state.inSettingsMode;
    state.settingsPage = 0;  // Start on first page
    if (!state.inSettingsMode) {
      // Exiting settings - save
      saveSettings();
    }
    return;
  }

  // Handle settings mode
  // SCALE mode pages: 0=Mode, 1=Root, 2=Scale, 3=Channel, 4=Clock
  // PRESET mode pages: 0=Mode, 1=Bank, 2=Channel, 3=Clock
  if (state.inSettingsMode) {
    int maxPages = (settings.playMode == 0) ? 4 : 3;  // SCALE has more pages

    // Encoder click cycles through settings pages or exits
    if (encoderState && !previousEncoderState) {
      state.settingsPage++;
      if (state.settingsPage > maxPages) {
        // Exit settings after last page
        saveSettings();
        loadCurrentMode();
        state.inSettingsMode = false;
        state.settingsPage = 0;
      }
    }

    // Encoder rotation changes current setting
    if (encoderValue != 0) {
      if (settings.playMode == 0) {
        // SCALE mode settings
        switch (state.settingsPage) {
          case 0:  // Play Mode
            settings.playMode = 1 - settings.playMode;  // Toggle 0/1
            state.settingsPage = 0;  // Reset to show new mode's settings
            break;
          case 1:  // Root Note
            if (encoderValue > 0) {
              settings.rootNote = constrain(settings.rootNote + 1, 24, 72);
            } else {
              settings.rootNote = constrain(settings.rootNote - 1, 24, 72);
            }
            break;
          case 2:  // Scale Type
            if (encoderValue > 0) {
              settings.scaleType = (settings.scaleType + 1) % NUM_SCALES;
            } else {
              settings.scaleType = (settings.scaleType + NUM_SCALES - 1) % NUM_SCALES;
            }
            break;
          case 3:  // MIDI Channel
            if (encoderValue > 0) {
              settings.midiOutputAChannel = (settings.midiOutputAChannel + 1) % 16;
            } else {
              settings.midiOutputAChannel = (settings.midiOutputAChannel + 15) % 16;
            }
            break;
          case 4:  // Clock Sync
            settings.midiClockSync = !settings.midiClockSync;
            break;
        }
      } else {
        // PRESET mode settings
        switch (state.settingsPage) {
          case 0:  // Play Mode
            settings.playMode = 1 - settings.playMode;
            state.settingsPage = 0;
            break;
          case 1:  // Preset Bank
            if (encoderValue > 0) {
              settings.presetBank = (settings.presetBank + 1) % NUM_PRESET_BANKS;
            } else {
              settings.presetBank = (settings.presetBank + NUM_PRESET_BANKS - 1) % NUM_PRESET_BANKS;
            }
            break;
          case 2:  // MIDI Channel
            if (encoderValue > 0) {
              settings.midiOutputAChannel = (settings.midiOutputAChannel + 1) % 16;
            } else {
              settings.midiOutputAChannel = (settings.midiOutputAChannel + 15) % 16;
            }
            break;
          case 3:  // Clock Sync
            settings.midiClockSync = !settings.midiClockSync;
            break;
        }
      }
      encoderValue = 0;
    }

    // Exit settings when shift released
    if (!shiftState && previousShiftState) {
      saveSettings();
      loadCurrentMode();
      state.inSettingsMode = false;
    }
    return;
  }

  // Encoder changes when idle (not playing, not editing)
  if (state.activePad < 0 && !state.inEditMode) {
    if (encoderValue != 0) {
      if (settings.playMode == 0) {
        // SCALE mode: encoder changes root note
        if (encoderValue > 0) {
          settings.rootNote = constrain(settings.rootNote + 1, 24, 72);
        } else {
          settings.rootNote = constrain(settings.rootNote - 1, 24, 72);
        }
      } else {
        // PRESET mode: encoder changes preset bank
        if (encoderValue > 0) {
          settings.presetBank = (settings.presetBank + 1) % NUM_PRESET_BANKS;
        } else {
          settings.presetBank = (settings.presetBank + NUM_PRESET_BANKS - 1) % NUM_PRESET_BANKS;
        }
      }
      encoderValue = 0;
      loadCurrentMode();
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
        // Stop any currently playing arp note before switching pads
        if (state.arpRate > 0 && state.activePad >= 0 && state.activePad != i) {
          stopCurrentArpNote();
        }
        // Play chord
        padStates[i] = true;
        state.activePad = i;
        // Reset arp index when switching pads
        state.arpNoteIndex = 0;
        state.arpDirection = true;
      }
    } else if (!keyStates[btnIndex] && previousKeyStates[btnIndex]) {
      // Button released
      if (!shiftState && !state.inEditMode) {
        padStates[i] = false;
        if (state.activePad == i) {
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
          midiClockReceived = true;
          midiClockCounter++;
          lastClockTime = millis();
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
      lastClockTime = millis();
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

  // Process pad state changes (only when arp is off)
  if (state.arpRate == 0) {
    for (int i = 0; i < 9; i++) {
      if (padStates[i] && !previousPadStates[i]) {
        playChord(i);
      } else if (!padStates[i] && previousPadStates[i]) {
        stopChord(i);
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
          // Reset arp index when switching pads
          state.arpNoteIndex = 0;
          state.arpDirection = true;
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
  1    // 1/64 note = 1 clock (every pulse... very fast)
};

void updateArpeggiator() {
  if (state.arpRate == 0 || state.activePad < 0) {
    return;
  }

  unsigned long currentTime = millis();

  // Check if external clock is active (received within last 500ms)
  bool clockPresent = (currentTime - lastClockTime) < 500;
  externalClockActive = clockPresent && settings.midiClockSync;

  bool shouldTrigger = false;

  if (externalClockActive) {
    // Sync to external MIDI clock
    if (midiClockReceived) {
      midiClockReceived = false;

      int divider = clockDividers[state.arpRate];
      if (divider > 0 && (midiClockCounter % divider) == 0) {
        shouldTrigger = true;
      }
    }
  } else {
    // Use internal timing
    int interval = arpTimings[state.arpRate];
    if (currentTime - state.lastArpTime >= interval) {
      state.lastArpTime = currentTime;
      shouldTrigger = true;
    }
  }

  if (shouldTrigger) {
    // Stop previous note
    stopArpNote(state.activePad, state.arpNoteIndex);

    // Advance to next note
    advanceArpIndex(state.activePad);

    // Play new note
    playArpNote(state.activePad, state.arpNoteIndex);
  }
}

void advanceArpIndex(int pad) {
  ChordV2& chord = pads[pad].chord;

  // Count active notes
  int activeCount = 0;
  int activeIndices[8];
  for (int i = 0; i < 8; i++) {
    if (chord.isActive[i]) {
      activeIndices[activeCount++] = i;
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

  switch (state.arpMode) {
    case 0: // Up
      currentPos = (currentPos + 1) % activeCount;
      break;
    case 1: // Down
      currentPos = (currentPos - 1 + activeCount) % activeCount;
      break;
    case 2: // Up-Down
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
    case 3: // Random
      currentPos = random(activeCount);
      break;
  }

  state.arpNoteIndex = activeIndices[currentPos];
}

void playArpNote(int pad, int noteIndex) {
  if (pad < 0 || pad >= 9) return;
  if (!pads[pad].chord.isActive[noteIndex]) return;

  ChordV2& chord = pads[pad].chord;
  int note = settings.rootNote + chord.rootOffset + chord.intervals[noteIndex]
             + (chord.octaveModifiers[noteIndex] * 12) + (state.currentOctave * 12);

  note = constrain(note, 0, 127);

  int velocity = constrain(
    settings.velocityScaling * (pads[pad].velocity + chord.velocityModifiers[noteIndex]),
    1, 127
  );

  int channel = chord.channel[noteIndex];
  sendNoteOn(note, velocity, getOutputChannel(channel));

  // Track what's playing so we can stop it later
  lastArpPad = pad;
  lastArpNoteIndex = noteIndex;
  arpNotePlaying = true;
}

void stopArpNote(int pad, int noteIndex) {
  if (pad < 0 || pad >= 9) return;
  if (noteIndex < 0 || noteIndex >= 8) return;
  if (!pads[pad].chord.isActive[noteIndex]) return;

  ChordV2& chord = pads[pad].chord;
  int note = settings.rootNote + chord.rootOffset + chord.intervals[noteIndex]
             + (chord.octaveModifiers[noteIndex] * 12) + (state.currentOctave * 12);

  note = constrain(note, 0, 127);

  int channel = chord.channel[noteIndex];
  sendNoteOff(note, 0, getOutputChannel(channel));
}

// Stop whatever arp note is currently playing
void stopCurrentArpNote() {
  if (arpNotePlaying && lastArpPad >= 0 && lastArpNoteIndex >= 0) {
    stopArpNote(lastArpPad, lastArpNoteIndex);
    arpNotePlaying = false;
    lastArpPad = -1;
    lastArpNoteIndex = -1;
  }
}

//================================ CHORD PLAYBACK ================================

void playChord(int pad) {
  ChordV2& chord = pads[pad].chord;

  for (int j = 0; j < 8; j++) {
    if (chord.isActive[j]) {
      playNote(pad, j);
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

  // Unused buttons in column 4 (buttons 3, 7, 11)
  // Button 3 = settings shortcut (Shift+3)
  if (state.inSettingsMode) {
    float pulse = (sin(millis() * 0.01) + 1) * 0.5;
    pixels.setPixelColor(4, dimColor(0xFFFF00, 0.3 + pulse * 0.7));  // Yellow pulse
  } else if (shiftState) {
    pixels.setPixelColor(4, dimColor(0xFFFF00, 0.3));  // Dim yellow when shift held
  } else {
    pixels.setPixelColor(4, 0x000000);
  }
  pixels.setPixelColor(8, 0x000000);   // Button 7
  pixels.setPixelColor(12, 0x000000);  // Button 11

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
  } else if (state.inEditMode) {
    drawEditScreen();
  } else {
    drawMainScreen();
  }

  display.display();
}

void drawMainScreen() {
  // Big chord display when playing
  if (state.activePad >= 0) {
    if (settings.playMode == 1) {
      // PRESET mode - show chord name from bank
      const char* chordName = presetBankInfo[settings.presetBank].chordNames[state.activePad];

      display.setTextSize(4);
      int nameLen = strlen(chordName);
      int xPos = (nameLen <= 2) ? 40 : (nameLen <= 4) ? 16 : 4;
      if (nameLen > 4) display.setTextSize(3);

      display.setCursor(xPos, 8);
      display.print(chordName);

      // Mode indicator
      display.setTextSize(1);
      display.setCursor(4, 4);
      display.print(presetBankInfo[settings.presetBank].name);

    } else {
      // SCALE mode - show root note + scale degree
      int chordRoot = settings.rootNote + pads[state.activePad].chord.rootOffset + (state.currentOctave * 12);
      chordRoot = constrain(chordRoot, 0, 127);

      // Big root note name
      display.setTextSize(4);
      display.setCursor(16, 8);
      display.print(midiNoteNames[chordRoot]);

      // Scale name
      display.setTextSize(1);
      display.setCursor(4, 4);
      display.print(scaleNames[settings.scaleType]);
    }

    // Pad number
    display.setTextSize(1);
    display.setCursor(116, 4);
    display.print(state.activePad + 1);

    // Draw active notes as bars at bottom
    ChordV2& chord = pads[state.activePad].chord;
    int barX = 4;
    for (int i = 0; i < 8; i++) {
      if (chord.isActive[i]) {
        int barHeight = 10 + (chord.intervals[i] / 4);
        barHeight = constrain(barHeight, 8, 20);
        display.fillRect(barX, 64 - barHeight, 12, barHeight, WHITE);
      } else {
        display.drawRect(barX, 56, 12, 8, WHITE);
      }
      barX += 15;
    }

    // Arp indicator
    if (state.arpRate > 0) {
      display.setTextSize(1);
      display.setCursor(100, 56);
      display.print("ARP");
    }

  } else {
    // IDLE state - different for each mode
    if (settings.playMode == 1) {
      // PRESET mode idle - show bank name BIG
      display.setTextSize(3);
      const char* bankName = presetBankInfo[settings.presetBank].name;
      int nameLen = strlen(bankName);
      int xPos = max(4, 64 - (nameLen * 9));
      display.setCursor(xPos, 0);
      display.print(bankName);

      // Mode label
      display.setTextSize(1);
      display.setCursor(4, 26);
      display.print("PRESET MODE");

    } else {
      // SCALE mode idle - show ROOT + SCALE BIG
      display.setTextSize(3);
      display.setCursor(4, 0);
      display.print(midiNoteNames[settings.rootNote]);

      display.setTextSize(2);
      display.setCursor(64, 4);
      display.print(scaleNames[settings.scaleType]);

      // Mode label
      display.setTextSize(1);
      display.setCursor(4, 26);
      display.print("SCALE MODE");
    }

    // Common info line
    display.setTextSize(1);
    display.setCursor(4, 38);
    display.print("CH:");
    display.print(settings.midiOutputAChannel + 1);

    display.setCursor(40, 38);
    display.print("OCT:");
    if (state.currentOctave >= 0) display.print("+");
    display.print(state.currentOctave);

    display.setCursor(80, 38);
    display.print("ARP:");
    display.print(arpRateNames[state.arpRate]);

    // Clock status
    display.setCursor(4, 48);
    display.print("CLK:");
    display.print(externalClockActive ? "SYNC" : (settings.midiClockSync ? "WAIT" : "OFF"));

    // Hint
    display.drawFastHLine(4, 56, 120, WHITE);
    display.setCursor(8, 58);
    if (settings.playMode == 0) {
      display.print("<< ROOT NOTE >>");
    } else {
      display.print("<< PRESET BANK >>");
    }
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
  int maxPages = (settings.playMode == 0) ? 5 : 4;

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

  if (settings.playMode == 0) {
    // SCALE mode settings
    switch (state.settingsPage) {
      case 0:  // Play Mode
        display.setTextSize(2);
        display.setCursor(20, 20);
        display.print("SCALE");
        label = "PLAY MODE";
        break;
      case 1:  // Root Note
        display.setTextSize(3);
        display.setCursor(32, 18);
        display.print(midiNoteNames[settings.rootNote]);
        label = "ROOT NOTE";
        break;
      case 2:  // Scale Type
        display.setTextSize(2);
        display.setCursor(20, 22);
        display.print(scaleNames[settings.scaleType]);
        label = "SCALE TYPE";
        break;
      case 3:  // MIDI Channel
        display.setTextSize(4);
        display.setCursor((settings.midiOutputAChannel + 1 < 10) ? 52 : 36, 18);
        display.print(settings.midiOutputAChannel + 1);
        label = "MIDI CHANNEL";
        break;
      case 4:  // Clock Sync
        display.setTextSize(3);
        display.setCursor(32, 22);
        display.print(settings.midiClockSync ? "ON" : "OFF");
        label = "CLOCK SYNC";
        break;
    }
  } else {
    // PRESET mode settings
    switch (state.settingsPage) {
      case 0:  // Play Mode
        display.setTextSize(2);
        display.setCursor(16, 20);
        display.print("PRESET");
        label = "PLAY MODE";
        break;
      case 1:  // Preset Bank
        display.setTextSize(2);
        display.setCursor(16, 22);
        display.print(presetBankInfo[settings.presetBank].name);
        label = "PRESET BANK";
        break;
      case 2:  // MIDI Channel
        display.setTextSize(4);
        display.setCursor((settings.midiOutputAChannel + 1 < 10) ? 52 : 36, 18);
        display.print(settings.midiOutputAChannel + 1);
        label = "MIDI CHANNEL";
        break;
      case 3:  // Clock Sync
        display.setTextSize(3);
        display.setCursor(32, 22);
        display.print(settings.midiClockSync ? "ON" : "OFF");
        label = "CLOCK SYNC";
        break;
    }
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
