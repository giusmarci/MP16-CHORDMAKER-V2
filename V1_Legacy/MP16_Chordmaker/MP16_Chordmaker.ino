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


//Pixel & Display objects
Adafruit_NeoPixel pixels(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//USB MIDI object
Adafruit_USBD_MIDI usb_midi;

// Key state arrays
bool shiftState = false;
bool previousShiftState = false;
bool encoderState = false;
bool previousEncoderState = false;
bool keyStates[16] = { false };
bool previousKeyStates[16] = { false };
bool shiftPressedAtKeyDown[16] = { false };
bool padStates[16] = { false };
bool previousPadStates[16] = { false };
bool midiStates[16] = { false };
bool previousMidiStates[16] = { false };

// Variables for encoder handling
volatile int encoderValue = 0;
volatile int lastEncoded = 0;
volatile int stepCounter = 0;
const int encoderStates[16] = { 0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0 };

//Menu and selection indices
int screenIndex = -1;  //Which screen is shown?
int selectIndex = 0;   //Which variable is selected?
int noteIndex = 0;     //Which of the 8 notes in a chord to select
int selectedPad = 0;   //Currently selected pad
int slotSelect = 0;
int copyIndex = -1;
bool recording = false;

//How much to dim the LED colors
float dimFactor = 0.3;

//MIDI state machine enum for Serial reading
enum MIDIState {
  WAITING_FOR_STATUS,  // Waiting for the first byte (status byte)
  WAITING_FOR_DATA1,   // Waiting for the first data byte (note number)
  WAITING_FOR_DATA2    // Waiting for the second data byte (velocity)
};

//Serial Interrupt variables
volatile uint8_t status = 0;
volatile uint8_t data1 = 0;
volatile uint8_t data2 = 0;
volatile MIDIState currentState = WAITING_FOR_STATUS;  // Start by waiting for the status byte
volatile bool midiMessageReady = false;                // Flag to indicate when a complete message has been received




// Integer array to track how many keys are referencing each MIDI note on each channel
int noteCountA[128] = { 0 };
int noteCountB[128] = { 0 };
int noteCountC[128] = { 0 };
int noteCountD[128] = { 0 };

//Global settings
struct Settings {
  int rootNote = 48;  // Root note (e.g., 48)
  int scaleType = 0;  // Scale type (e.g., 0)
  int midiRecChannel = 0;
  int midiTrigChannel = 1;
  int midiOutputAChannel = 0;
  int midiOutputBChannel = 2;
  int midiOutputCChannel = 3;
  int midiOutputDChannel = 15;
  bool midiThru = false;
  bool tempoSync = true;
  float tempo = 120;
  float velocityScaling = 1.0;
  int defaultVelocity = 65;
  uint32_t shiftColor = 0xFF0000;
  int ledBrightness = 100;
};

//Chord struct
struct Chord {
  int degree = 0;                    // Chord degree (e.g., 0, 1, 2, etc.)
  int intervals[8] = { 0 };          // Intervals for the chord
  int octaveModifiers[8] = { 0 };    // Octave modifications for each note
  int semitoneModifiers[8] = { 0 };  // Semitone modifications for each note
  bool isActive[8] = { false };      // Whether each note is active or not
  int velocityModifiers[8] = { 0 };  // Velocity values for each note modifying the main pad velocity
  unsigned long timing[8] = { 0 };   // Delay timing for each note
  int channel[8] = { 0 };
};

//Pad settings
struct Pad {
  uint32_t color = 0xFFAA00;
  int triggerNote = 0;
  int chokeGroup = 0;
  bool hold = false;
  int arpType = 0;  //0 = off, 1 = up, 2 = down, 3 = both up & down, 4 = random, 5 = brownian
  int arpSpeed = 1;
  float arpGate = 1.0;
  int arpOctaves = 1;
  int padVelocity = 65;
  int velocityVariation = 0;
  unsigned long timingVariation = 0;
  Chord chord;
};

Settings settings;
Pad pads[16];

#include "preset.h"
#include "musicTheory.h"

//Setup function
void setup() {
  initHardware();

  usb_midi.begin();

  pinMode(1, INPUT_PULLUP);
  Serial1.begin(31250);

  if (!LittleFS.begin()) {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("LittleFS failed");
    display.display();
    delay(1000);
  }

  loadSettings();
}

//Loop function
void loop() {
  checkKeys();
  updateMenu();
  updateMIDI();
  updateVisuals();
}


//********************************** Setup Functions ************************************

//Hardware initialization for keymatrix, shift key, encoder, neopixels and OLED display
void initHardware() {
  pinMode(ROW0_PIN, OUTPUT);
  pinMode(ROW1_PIN, OUTPUT);
  pinMode(ROW2_PIN, OUTPUT);
  pinMode(ROW3_PIN, OUTPUT);
  pinMode(COL0_PIN, INPUT_PULLUP);
  pinMode(COL1_PIN, INPUT_PULLUP);
  pinMode(COL2_PIN, INPUT_PULLUP);
  pinMode(COL3_PIN, INPUT_PULLUP);
  pinMode(SHIFT_PIN, INPUT_PULLUP);
  pinMode(ENCODER_S, INPUT_PULLUP);
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENCODER_A), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RX_PIN), midiInterruptHandler, FALLING);

  pixels.begin();
  pixels.setBrightness(100);
  pixels.clear();
  pixels.show();

  display.setRotation(2);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    for (;;)
      ;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();
}

//Interrupt function for the encoder
void updateEncoder() {
  // Read both A and B pin states
  int MSB = digitalRead(ENCODER_A);  // Most Significant Bit
  int LSB = digitalRead(ENCODER_B);  // Least Significant Bit

  // Create a binary representation of the state
  int encoded = (MSB << 1) | LSB;          // Combine A and B signal states
  int sum = (lastEncoded << 2) | encoded;  // Create a unique sum to match the state transitions

  // Update step counter based on encoder state transition
  stepCounter += encoderStates[sum];

  // Check if full step (detent) is reached (4 steps per detent)
  if (abs(stepCounter) >= 4) {
    if (stepCounter > 0) {
      encoderValue++;  // Clockwise rotation
    } else {
      encoderValue--;  // Counter-clockwise rotation
    }
    stepCounter = 0;  // Reset counter after full step
  }

  lastEncoded = encoded;  // Store the current state for next transition
}

void midiInterruptHandler() {
  uint8_t incomingByte = Serial1.read();  // Read incoming byte

  switch (currentState) {
    case WAITING_FOR_STATUS:
      if ((incomingByte >= 128 && incomingByte <= 143) || (incomingByte >= 144 && incomingByte <= 159)) {
        status = incomingByte;
        currentState = WAITING_FOR_DATA1;  // Next, expect data1 (note number)
      }
      break;

    case WAITING_FOR_DATA1:
      data1 = incomingByte;              // Store the note number
      currentState = WAITING_FOR_DATA2;  // Next, expect data2 (velocity)
      break;

    case WAITING_FOR_DATA2:
      data2 = incomingByte;               // Store the velocity
      midiMessageReady = true;            // Complete message received
      currentState = WAITING_FOR_STATUS;  // Reset to waiting for the next status byte
      break;
  }
}


//Load in all the data for the chords from flash memory, or initialize from the preset
void loadSettings() {
  if (!loadFromFlash(0)) {
    // No saved data available, initialize chords from preset
    initFromPreset();
  }
}

//Load a memory slot from flash memory, returns true is succesful, otherwise returns false
bool loadFromFlash(int slot) {
  String filePath = "/slot" + String(slot) + ".txt";  // Use String for concatenation
  File file = LittleFS.open(filePath, "r");           // Pass the constructed file path
  if (!file) {
    return false;
  } else {
    file.read((uint8_t*)&settings, sizeof(Settings));
    file.read((uint8_t*)&pads, sizeof(pads));
    file.close();
    return true;
  }
}

//initialize the chords in all pads from the preset.h file
void initFromPreset() {
  // Initialize the global `chords` array from the `presets` array
  for (int i = 0; i < 16; i++) {
    // Copy the preset chord data to the global chords array
    for (int j = 0; j < 8; j++) {
      pads[i].chord.intervals[j] = preset[i].intervals[j];
      pads[i].chord.octaveModifiers[j] = preset[i].octaveModifiers[j];
      pads[i].chord.semitoneModifiers[j] = preset[i].semitoneModifiers[j];
      pads[i].chord.isActive[j] = preset[i].isActive[j];
      pads[i].chord.velocityModifiers[j] = preset[i].velocityModifiers[j];
      pads[i].chord.timing[j] = preset[i].timing[j];
      pads[i].chord.channel[j] = preset[i].channel[j];
    }
    pads[i].chord.degree = preset[i].degree;  // Copy the degree value
    pads[i].triggerNote = settings.rootNote + i;
  }
}

// ********************************** Loop Functions ************************************

//Checking all keys to update their input
void checkKeys() {
  //Set all previous states to current states
  previousShiftState = shiftState;
  previousEncoderState = encoderState;
  for (int i = 0; i < 16; i++) {
    previousKeyStates[i] = keyStates[i];
  }

  //Now update all current states
  shiftState = !digitalRead(SHIFT_PIN);
  encoderState = !digitalRead(ENCODER_S);

  //Read through the keymatrix to update key states
  for (int row = 0; row < 4; row++) {
    digitalWrite(ROW0_PIN, row == 0 ? LOW : HIGH);
    digitalWrite(ROW1_PIN, row == 1 ? LOW : HIGH);
    digitalWrite(ROW2_PIN, row == 2 ? LOW : HIGH);
    digitalWrite(ROW3_PIN, row == 3 ? LOW : HIGH);
    for (int col = 0; col < 4; col++) {
      int keyIndex = row * 4 + col;
      bool currentKeyState = !digitalRead(COL0_PIN + col);
      keyStates[keyIndex] = currentKeyState;
    }
  }
  //set last row high again after scanning
  digitalWrite(ROW3_PIN, HIGH);
}




//Main function to update the menu
void updateMenu() {
  switch (screenIndex) {
    case -2:  //Load preset
      menuLoad();
      break;
    case -1:  // Default screen
      menuDefault();
      break;
    case 0:  // Edit Root Note
      menuRoot();
      break;
    case 1:  // Edit Scale
      menuScale();
      break;
    case 2:  // Edit Degree
      menuDegree();
      break;
    case 3:  //Edit Active Notes
      menuNotes();
      break;
    case 4:  // Edit Velocity Randomness
      menuVariation();
      break;
    case 5:  // Edit Velocity
      menuVelocity();
      break;
    case 6:  //Edit Interval Mods
      menuIntervals();
      break;
    case 7:  //Edit Octave
      menuOctaves();
      break;
    case 14:  //Edit MIDI Settings
      menuMidi();
      break;
    case 15:  //Saving
      menuSave();
      break;
  }
}

void menuLoad() {
  if (encoderValue > 0) {
    slotSelect = (slotSelect + 1) % 4;
    encoderValue = 0;
    stepCounter = 0;
  } else if (encoderValue < 0) {
    slotSelect = (slotSelect + 3) % 4;
    encoderValue = 0;
    stepCounter = 0;
  }
  if (encoderState && !previousEncoderState) {
    killAllNotes();
    screenIndex = -1;
    if (loadFromFlash(slotSelect)) {
      display.clearDisplay();
      display.setCursor(22, 28);
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.print("Loaded Slot ");
      display.print(slotSelect + 1);
      display.display();
      delay(1000);
    } else {
      display.clearDisplay();
      display.setCursor(22, 28);
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.print("Loading Failed");
      display.display();
      delay(1000);
    }
  }
}

void menuDefault() {
  if (stepCounter > 0) {
    settings.velocityScaling = constrain(settings.velocityScaling + 0.01, 0.05, 2.0);
    encoderValue = 0;
    stepCounter = 0;
  } else if (stepCounter < 0) {
    settings.velocityScaling = constrain(settings.velocityScaling - 0.01, 0.05, 2.0);
    encoderValue = 0;
    stepCounter = 0;
  }
  if (encoderState && !previousEncoderState) {
    screenIndex = -2;
  }
}

void menuRoot() {
  if (encoderValue > 0) {
    killAllNotes();
    settings.rootNote = constrain(settings.rootNote + 1, 12, 107);
    encoderValue = 0;
    stepCounter = 0;
  } else if (encoderValue < 0) {
    killAllNotes();
    settings.rootNote = constrain(settings.rootNote - 1, 12, 107);
    encoderValue = 0;
    stepCounter = 0;
  }
}

void menuScale() {
  if (encoderValue > 0) {
    killAllNotes();
    settings.scaleType = (settings.scaleType + 1) % 10;
    setAllChordIntervals();
    encoderValue = 0;
    stepCounter = 0;
  } else if (encoderValue < 0) {
    killAllNotes();
    settings.scaleType = (settings.scaleType + 9) % 10;
    setAllChordIntervals();
    encoderValue = 0;
    stepCounter = 0;
  }
}

void menuDegree() {
  if (encoderValue > 0) {
    killAllNotes();
    pads[selectedPad].chord.degree = constrain(pads[selectedPad].chord.degree + 1, 0, 6);
    setChordIntervals(selectedPad);
    encoderValue = 0;
    stepCounter = 0;
  } else if (encoderValue < 0) {
    killAllNotes();
    pads[selectedPad].chord.degree = constrain(pads[selectedPad].chord.degree - 1, 0, 6);
    setChordIntervals(selectedPad);
    encoderValue = 0;
    stepCounter = 0;
  }
}

void menuNotes() {
  if (encoderValue > 0) {
    noteIndex = (noteIndex + 1) % 8;
    encoderValue = 0;
    stepCounter = 0;
  } else if (encoderValue < 0) {
    noteIndex = (noteIndex + 7) % 8;
    encoderValue = 0;
    stepCounter = 0;
  }
  if (encoderState && !previousEncoderState) {
    killAllNotes();
    pads[selectedPad].chord.isActive[noteIndex] = !pads[selectedPad].chord.isActive[noteIndex];
  }
}

void menuVariation() {
  if (encoderValue > 0) {
    pads[selectedPad].velocityVariation = constrain(pads[selectedPad].velocityVariation + 1, 0, 99);
    encoderValue = 0;
    stepCounter = 0;
  } else if (encoderValue < 0) {
    pads[selectedPad].velocityVariation = constrain(pads[selectedPad].velocityVariation - 1, 0, 99);
    encoderValue = 0;
    stepCounter = 0;
  }
}

void menuVelocity() {
  if (selectIndex == 0) {
    if (encoderValue > 0) {
      noteIndex = (noteIndex + 1) % 8;
      encoderValue = 0;
      stepCounter = 0;
    } else if (encoderValue < 0) {
      noteIndex = (noteIndex + 7) % 8;
      encoderValue = 0;
      stepCounter = 0;
    }
    if (encoderState && !previousEncoderState) {
      selectIndex = noteIndex + 1;
    }
  } else if (selectIndex - 1 < 8) {
    if (encoderValue > 0) {
      killAllNotes();
      pads[selectedPad].chord.velocityModifiers[selectIndex - 1] = constrain(pads[selectedPad].chord.velocityModifiers[selectIndex - 1] + 1, 1, 128);
      encoderValue = 0;
      stepCounter = 0;
    } else if (encoderValue < 0) {
      killAllNotes();
      pads[selectedPad].chord.velocityModifiers[selectIndex - 1] = constrain(pads[selectedPad].chord.velocityModifiers[selectIndex - 1] - 1, 1, 128);
      encoderValue = 0;
      stepCounter = 0;
    }
    if (encoderState && !previousEncoderState) {
      selectIndex = 0;
    }
  }
}

void menuIntervals() {
  if (selectIndex == 0) {
    if (encoderValue > 0) {
      noteIndex = (noteIndex + 1) % 8;
      encoderValue = 0;
      stepCounter = 0;
    } else if (encoderValue < 0) {
      noteIndex = (noteIndex + 7) % 8;
      encoderValue = 0;
      stepCounter = 0;
    }
    if (encoderState && !previousEncoderState) {
      selectIndex = noteIndex + 1;
    }
  } else if (selectIndex - 1 < 8) {
    if (encoderValue > 0) {
      killAllNotes();
      pads[selectedPad].chord.semitoneModifiers[selectIndex - 1] = constrain(pads[selectedPad].chord.semitoneModifiers[selectIndex - 1] + 1, -7, 7);
      encoderValue = 0;
      stepCounter = 0;
    } else if (encoderValue < 0) {
      killAllNotes();
      pads[selectedPad].chord.semitoneModifiers[selectIndex - 1] = constrain(pads[selectedPad].chord.semitoneModifiers[selectIndex - 1] - 1, -7, 7);
      encoderValue = 0;
      stepCounter = 0;
    }
    if (encoderState && !previousEncoderState) {
      selectIndex = 0;
    }
  }
}

void menuOctaves() {
  if (selectIndex == 0) {
    if (encoderValue > 0) {
      noteIndex = (noteIndex + 1) % 8;
      encoderValue = 0;
      stepCounter = 0;
    } else if (encoderValue < 0) {
      noteIndex = (noteIndex + 7) % 8;
      encoderValue = 0;
      stepCounter = 0;
    }
    if (encoderState && !previousEncoderState) {
      selectIndex = noteIndex + 1;
    }
  } else if (selectIndex - 1 < 8) {
    if (encoderValue > 0) {
      killAllNotes();
      pads[selectedPad].chord.octaveModifiers[selectIndex - 1] = constrain(pads[selectedPad].chord.octaveModifiers[selectIndex - 1] + 1, -3, 3);
      encoderValue = 0;
      stepCounter = 0;
    } else if (encoderValue < 0) {
      killAllNotes();
      pads[selectedPad].chord.octaveModifiers[selectIndex - 1] = constrain(pads[selectedPad].chord.octaveModifiers[selectIndex - 1] - 1, -3, 3);
      encoderValue = 0;
      stepCounter = 0;
    }
    if (encoderState && !previousEncoderState) {
      selectIndex = 0;
    }
  }
}

void menuMidi() {
  if (selectIndex == 0) {
    if (encoderValue > 0) {
      settings.midiRecChannel = (settings.midiRecChannel + 1) % 16;
      encoderValue = 0;
      stepCounter = 0;
    } else if (encoderValue < 0) {
      settings.midiRecChannel = (settings.midiRecChannel + 15) % 16;
      encoderValue = 0;
      stepCounter = 0;
    }
    if (encoderState && !previousEncoderState) {
      selectIndex = 1;
    }
  } else if (selectIndex == 1) {
    if (encoderValue > 0) {
      killAllNotes();
      settings.midiTrigChannel = (settings.midiTrigChannel + 1) % 16;
      encoderValue = 0;
      stepCounter = 0;
    } else if (encoderValue < 0) {
      killAllNotes();
      settings.midiTrigChannel = (settings.midiTrigChannel + 15) % 16;
      encoderValue = 0;
      stepCounter = 0;
    }
    if (encoderState && !previousEncoderState) {
      selectIndex = 2;
    }
  } else if (selectIndex == 2) {
    if (encoderValue > 0) {
      killAllNotes();
      settings.midiOutputAChannel = (settings.midiOutputAChannel + 1) % 16;
      encoderValue = 0;
      stepCounter = 0;
    } else if (encoderValue < 0) {
      killAllNotes();
      settings.midiOutputAChannel = (settings.midiOutputAChannel + 15) % 16;
      encoderValue = 0;
      stepCounter = 0;
    }
    if (encoderState && !previousEncoderState) {
      selectIndex = 3;
    }
  } else if (selectIndex == 3) {
    if (encoderValue > 0) {
      killAllNotes();
      settings.midiOutputBChannel = (settings.midiOutputBChannel + 1) % 16;
      encoderValue = 0;
      stepCounter = 0;
    } else if (encoderValue < 0) {
      killAllNotes();
      settings.midiOutputBChannel = (settings.midiOutputBChannel + 15) % 16;
      encoderValue = 0;
      stepCounter = 0;
    }
    if (encoderState && !previousEncoderState) {
      selectIndex = 4;
    }
  } else if (selectIndex == 4) {
    if (encoderValue > 0) {
      killAllNotes();
      settings.midiOutputCChannel = (settings.midiOutputCChannel + 1) % 16;
      encoderValue = 0;
      stepCounter = 0;
    } else if (encoderValue < 0) {
      killAllNotes();
      settings.midiOutputCChannel = (settings.midiOutputCChannel + 15) % 16;
      encoderValue = 0;
      stepCounter = 0;
    }
    if (encoderState && !previousEncoderState) {
      selectIndex = 5;
    }
  } else if (selectIndex == 5) {
    if (encoderValue > 0) {
      killAllNotes();
      settings.midiOutputDChannel = (settings.midiOutputDChannel + 1) % 16;
      encoderValue = 0;
      stepCounter = 0;
    } else if (encoderValue < 0) {
      killAllNotes();
      settings.midiOutputDChannel = (settings.midiOutputDChannel + 15) % 16;
      encoderValue = 0;
      stepCounter = 0;
    }
    if (encoderState && !previousEncoderState) {
      selectIndex = 0;
    }
  }
}

void menuSave() {
  if (encoderValue > 0) {
    slotSelect = (slotSelect + 1) % 4;
    encoderValue = 0;
    stepCounter = 0;
  } else if (encoderValue < 0) {
    slotSelect = (slotSelect + 3) % 4;
    encoderValue = 0;
    stepCounter = 0;
  }
  if (encoderState && !previousEncoderState) {
    saveToFlash(slotSelect);
    screenIndex = -1;
    selectIndex = 0;
    display.clearDisplay();
    display.setCursor(22, 28);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.print("Settings Saved");
    display.display();
    delay(1000);
  }
}

//Menu helper function to kill all notes in reference
void killAllNotes() {
  for (int i = 0; i < 128; i++) {
    if (noteCountA[i] > 0) {
      sendNoteOff(i, 0, settings.midiOutputAChannel);
    }
    noteCountA[i] = 0;
    if (noteCountB[i] > 0) {
      sendNoteOff(i, 0, settings.midiOutputBChannel);
    }
    noteCountB[i] = 0;
    if (noteCountC[i] > 0) {
      sendNoteOff(i, 0, settings.midiOutputCChannel);
    }
    noteCountC[i] = 0;
    if (noteCountD[i] > 0) {
      sendNoteOff(i, 0, settings.midiOutputDChannel);
    }
    noteCountD[i] = 0;
  }
}

//Menu helper function for updating all chord intervals after a scale change
void setAllChordIntervals() {
  for (int i = 0; i < 16; i++) {
    setChordIntervals(i);
  }
}

//Menu helper function for updating chord intervals after a scale or degree change
void setChordIntervals(int i) {
  for (int j = 0; j < 7; j++) {
    pads[i].chord.intervals[j] = degreeToScaleInterval(pads[i].chord.degree + chordDegrees[j], scaleIntervals[settings.scaleType], 7);
  }
  pads[i].chord.intervals[7] = pads[i].chord.intervals[0];
}

//Menu helper function to calculate interval within the scale, accounting for octave shifts
int degreeToScaleInterval(int degree, int scale[], int scaleLength) {
  int baseDegree = degree % scaleLength;
  int octaveShift = (degree / scaleLength) * 12;
  return scale[baseDegree] + octaveShift;
}

//Save current settings to a memory slot
void saveToFlash(int slot) {
  String filePath = "/slot" + String(slot) + ".txt";  // Use String for concatenation
  File file = LittleFS.open(filePath, "w");           // Pass the constructed file path
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  file.write((uint8_t*)&settings, sizeof(Settings));
  file.write((uint8_t*)&pads, sizeof(pads));
  file.close();
}

//Main MIDI update function
void updateMIDI() {
  for (int i = 0; i < 16; i++) {
    previousMidiStates[i] = midiStates[i];
    previousPadStates[i] = padStates[i];
  }

  if (midiMessageReady) {
    midiMessageReady = false;  // Reset the flag
    // Process the MIDI message (status, data1, data2)
    processIncomingMIDI(status, data1, data2);
  }

  // Check for incoming USB MIDI messages
  if (usb_midi.available()) {
    uint32_t packet = usb_midi.read();
    uint8_t status = (packet >> 8) & 0xFF;  // Status byte
    uint8_t data1 = (packet >> 16) & 0xFF;  // Data1 (e.g., note)
    uint8_t data2 = (packet >> 24) & 0xFF;  // Data2 (e.g., velocity)
    processIncomingMIDI(status, data1, data2);
  }


  if (shiftState && !previousShiftState) {  //Reset shortcut screen index is shift is pressed
    screenIndex = -1;
  }

  for (int i = 0; i < 16; i++) {  //Go through all pads

    //Check for rising or falling edges in the keys
    if (keyStates[i] && !previousKeyStates[i]) {  // Key is pressed
      if (shiftState) {                           // Check if we're using a SHIFT shortcut
        shiftPressedAtKeyDown[i] = true;
        screenIndex = i;
        if (i == 13) {
          copyIndex = selectedPad;
        }
        selectIndex = 0;
      } else if (screenIndex == 13) {  // Check whether we're copying a pad
        killAllNotes();
        if (copyIndex != -1 && copyIndex != i) {
          copyPad(i, copyIndex);
          copyIndex = -1;
        }
        screenIndex = -1;
      } else {  //Change pad state
        shiftPressedAtKeyDown[i] = false;
        padStates[i] = true;
        selectedPad = i;
      }
    } else if (!keyStates[i] && previousKeyStates[i]) {  // Key is released
      if (!shiftPressedAtKeyDown[i]) {
        for (int j = 0; j < 8; j++) {
          if (pads[i].chord.isActive[j]) {
            if (!midiStates[i]) {
              padStates[i] = false;
            }
          }
        }
      }
    }

    //Check for rising and falling edges in the midiTriggers
    if (midiStates[i] && !previousMidiStates[i]) {
      padStates[i] = true;
    } else if (!midiStates[i] && previousMidiStates[i]) {
      if (!keyStates[i]) {
        padStates[i] = false;
      }
    }

    //Finally check rising or falling edges in pad playing status
    if (padStates[i] && !previousPadStates[i]) {
      playChord(i);
    } else if (!padStates[i] && previousPadStates[i]) {
      stopChord(i);
    }
  }
}


void processIncomingMIDI(uint8_t status, uint8_t data1, uint8_t data2) {
  uint8_t command = status & 0xF0;  // Mask out the channel bits
  uint8_t channel = status & 0x0F;  // Get the channel
  if (settings.midiThru) {
    forwardMIDI(status, data1, data2);
  }

  if (channel == settings.midiRecChannel && recording) {
    //store incoming notes into a buffer for structuring into a chord later?
  }

  if (channel == settings.midiTrigChannel && (command == 144 || command == 128)) {
    for (int i = 0; i < 16; i++) {
      if (data1 == pads[i].triggerNote) {
        if (data2 > 0 && command == 144) {
          midiStates[i] = true;
        } else {
          midiStates[i] = false;
        }
      }
    }
  }
}

void forwardMIDI(uint8_t status, uint8_t data1, uint8_t data2) {
  // Forward to USB MIDI
  uint8_t usb_packet[] = { status, data1, data2 };
  usb_midi.write(usb_packet, 3);

  // Forward to Serial MIDI
  Serial1.write(status);
  Serial1.write(data1);
  Serial1.write(data2);
}

void playChord(int i) {
  for (int j = 0; j < 8; j++) {
    if (pads[i].chord.isActive[j]) {
      playNote(i, j);
    }
  }
}

void playNote(int pad, int j) {
  int note = settings.rootNote + pads[pad].chord.intervals[j] + (pads[pad].chord.octaveModifiers[j] * 12) + pads[pad].chord.semitoneModifiers[j];
  int velocity = constrain(settings.velocityScaling * (pads[pad].padVelocity + pads[pad].chord.velocityModifiers[j] + random(-pads[pad].velocityVariation, pads[pad].velocityVariation)), 1, 128);

  // Send NoteOff to stop any existing note in the correct channel, then increment reference count and play new note in that channel
  switch (pads[pad].chord.channel[j]) {
    case 0:
      if (noteCountA[note] > 0) { sendNoteOff(note, velocity, settings.midiOutputAChannel); }
      noteCountA[note]++;
      sendNoteOn(note, velocity, settings.midiOutputAChannel);
      break;
    case 1:
      if (noteCountB[note] > 0) { sendNoteOff(note, velocity, settings.midiOutputBChannel); }
      noteCountB[note]++;
      sendNoteOn(note, velocity, settings.midiOutputBChannel);
      break;
    case 2:
      if (noteCountC[note] > 0) { sendNoteOff(note, velocity, settings.midiOutputCChannel); }
      noteCountC[note]++;
      sendNoteOn(note, velocity, settings.midiOutputCChannel);
      break;
    case 3:
      if (noteCountD[note] > 0) { sendNoteOff(note, velocity, settings.midiOutputDChannel); }
      noteCountD[note]++;
      sendNoteOn(note, velocity, settings.midiOutputDChannel);
      break;
  }
}

void stopChord(int i) {
  for (int j = 0; j < 8; j++) {
    if (pads[i].chord.isActive[j]) {
      stopNote(i, j);
    }
  }
}

void stopNote(int pad, int j) {
  int note = settings.rootNote + pads[pad].chord.intervals[j] + (pads[pad].chord.octaveModifiers[j] * 12) + pads[pad].chord.semitoneModifiers[j];

  // Decrement the reference count and send NoteOff if it's the last reference
  switch (pads[pad].chord.channel[j]) {
    case 0:
      if (noteCountA[note] > 0) {
        noteCountA[note]--;
        if (noteCountA[note] == 0) { sendNoteOff(note, 0, settings.midiOutputAChannel); }
      }
      break;
    case 1:
      if (noteCountB[note] > 0) {
        noteCountB[note]--;
        if (noteCountB[note] == 0) { sendNoteOff(note, 0, settings.midiOutputBChannel); }
      }
      break;
    case 2:
      if (noteCountC[note] > 0) {
        noteCountC[note]--;
        if (noteCountC[note] == 0) { sendNoteOff(note, 0, settings.midiOutputCChannel); }
      }
      break;
    case 3:
      if (noteCountD[note] > 0) {
        noteCountD[note]--;
        if (noteCountD[note] == 0) { sendNoteOff(note, 0, settings.midiOutputDChannel); }
      }
      break;
  }
}

void sendNoteOn(int note, int velocity, int channel) {
  if (channel < 0 || channel > 15) return;
  uint8_t status = 0x90 | (channel);  // 0x90 is "Note On", and channel is adjusted to 0-based
  uint8_t usb_packet[] = { status, (uint8_t)note, (uint8_t)velocity };
  usb_midi.write(usb_packet, 3);
  Serial1.write(status);
  Serial1.write(note);
  Serial1.write(velocity);
}

void sendNoteOff(int note, int velocity, int channel) {
  if (channel < 0 || channel > 15) return;
  uint8_t status = 0x80 | (channel);
  uint8_t usb_packet[] = { status, (uint8_t)note, (uint8_t)velocity };
  usb_midi.write(usb_packet, 3);
  Serial1.write(status);
  Serial1.write(note);
  Serial1.write(velocity);
}


void copyPad(int target, int source) {
  pads[target].chord = pads[source].chord;
  display.clearDisplay();
  display.setCursor(22, 28);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("Copied ");
  display.print(source + 1);
  display.print(" to ");
  display.print(target + 1);
  display.display();
  delay(1000);
}

//Updating the visuals!
void updateVisuals() {
  updatePixels();
  updateDisplay();
}

//main function for updating the neopixels
void updatePixels() {
  pixels.clear();

  if (shiftState) {
    pixels.setPixelColor(0, settings.shiftColor);  // Shift key lighting
  } else if (screenIndex != -1) {
    pixels.setPixelColor(0, dimColor(settings.shiftColor, dimFactor));  // Shift key lighting dimly
  }

  for (int i = 0; i < 16; i++) {
    if (i == selectedPad && !shiftState) {
      pixels.setPixelColor(i + 1, pads[i].color);
    } else {
      pixels.setPixelColor(i + 1, dimColor(pads[i].color, dimFactor));
    }

    if (keyStates[i] || midiStates[i]) {
      pixels.setPixelColor(i + 1, 0xFFFFFF);
    }
  }
  pixels.show();
}

//Helper function for dimming a pixel color into a dimmer color
uint32_t dimColor(uint32_t color, float factor) {
  // Extract the red, green, and blue components
  uint8_t red = (color >> 16) & 0xFF;   // Extract red
  uint8_t green = (color >> 8) & 0xFF;  // Extract green
  uint8_t blue = color & 0xFF;          // Extract blue

  // Scale each component by the factor
  red = static_cast<uint8_t>(red * factor);
  green = static_cast<uint8_t>(green * factor);
  blue = static_cast<uint8_t>(blue * factor);

  // Reassemble the dimmed color
  uint32_t dimmedColor = (red << 16) | (green << 8) | blue;
  return dimmedColor;
}

//Main function for updating the OLED display
void updateDisplay() {
  display.clearDisplay();
  switch (screenIndex) {
    case -2:
      drawLoadScreen();
      break;
    case -1:
      drawMain();
      break;
    case 0:
      drawRootMenu();
      break;
    case 1:
      drawScaleMenu();
      break;
    case 2:
      drawDegreeMenu();
      break;
    case 3:
      drawNoteMenu();
      break;
    case 4:
      drawVariationMenu();
      break;
    case 5:
      drawVelocityMenu();
      break;
    case 6:
      drawModsMenu();
      break;
    case 7:
      drawOctaveMenu();
      break;
    case 13:
      drawCopyMenu();
      break;
    case 14:
      drawMidiMenu();
      break;
    case 15:
      drawSaveScreen();
      break;
  }

  display.display();
}

void drawLoadScreen() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(40, 6);
  display.print("Load:");
  drawSlots();
}

void drawMain() {
  drawSelectedPad();
  display.setCursor(66, 5);
  display.print("Degree: ");
  display.print(pads[selectedPad].chord.degree + 1);
  drawChordNotes(selectedPad);
  drawNoteVelocities(selectedPad);
}

void drawRootMenu() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(37, 28);
  display.print("Root: ");
  display.print(midiNoteNames[settings.rootNote]);
}

void drawScaleMenu() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(18, 28);
  display.print("Scale: ");
  display.print(scaleNames[settings.scaleType]);
}

void drawDegreeMenu() {
  drawSelectedPad();
  display.setCursor(66, 5);
  display.print("Degree: ");
  display.print(pads[selectedPad].chord.degree + 1);
  display.drawLine(66, 14, 120, 14, WHITE);
  drawChordNotes(selectedPad);
  drawNoteVelocities(selectedPad);
}

void drawNoteMenu() {
  drawSelectedPad();
  display.setCursor(62, 5);
  display.print("Edit Notes");
  drawChordNotes(selectedPad);
  drawNoteVelocities(selectedPad);
  drawNoteSelect();
}

void drawVariationMenu() {
  drawSelectedPad();
  display.setCursor(4, 28);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Velocity Random: ");
  display.print(pads[selectedPad].velocityVariation);
}

void drawVelocityMenu() {
  drawSelectedPad();
  display.setCursor(50, 5);
  display.print("Edit Velocity");
  drawChordNotes(selectedPad);
  drawNoteVelocities(selectedPad);
  drawVelocitySelect();
}

void drawModsMenu() {
  drawSelectedPad();
  display.setCursor(44, 5);
  display.print("Edit Intervals");
  drawChordNotes(selectedPad);
  drawModSelect();
}

void drawOctaveMenu() {
  drawSelectedPad();
  display.setCursor(50, 5);
  display.print("Edit Octaves");
  drawChordNotes(selectedPad);
  drawNoteOctaves(selectedPad);
  drawNoteVelocities(selectedPad);
  drawVelocitySelect();
}

void drawCopyMenu() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(31, 20);
  display.print("Copy pad ");
  display.print(selectedPad + 1);
  display.setCursor(10, 40);
  display.print("Select destination");
}

void drawMidiMenu() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(5, 0);
  display.print("MIDI In Rec:  ");
  display.println(settings.midiRecChannel + 1);
  display.setCursor(5, 11);
  display.print("MIDI In Trig: ");
  display.println(settings.midiTrigChannel + 1);
  display.setCursor(5, 22);
  display.print("MIDI Out A:   ");
  display.println(settings.midiOutputAChannel + 1);
  display.setCursor(5, 33);
  display.print("MIDI Out B:   ");
  display.println(settings.midiOutputBChannel + 1);
  display.setCursor(5, 44);
  display.print("MIDI Out C:   ");
  display.println(settings.midiOutputCChannel + 1);
  display.setCursor(5, 55);
  display.print("MIDI Out D:   ");
  display.println(settings.midiOutputDChannel + 1);

  switch (selectIndex) {
    case 0:
      display.drawLine(5, 8, 124, 8, WHITE);
      break;
    case 1:
      display.drawLine(5, 19, 124, 19, WHITE);
      break;
    case 2:
      display.drawLine(5, 30, 124, 30, WHITE);
      break;
    case 3:
      display.drawLine(5, 41, 124, 41, WHITE);
      break;
    case 4:
      display.drawLine(5, 52, 124, 52, WHITE);
      break;
    case 5:
      display.drawLine(5, 63, 124, 63, WHITE);
      break;
  }
}

void drawSaveScreen() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(40, 6);
  display.print("Save to:");
  drawSlots();
}


//****************** display helper functions ******************

void drawSlots() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  for (int i = 0; i < 4; i++) {
    display.setCursor(44, 17 + 11 * i);
    display.print("Slot ");
    display.print(i + 1);
    if (i == slotSelect) {
      display.drawRect(42, 15 + 11 * i, 40, 11, WHITE);
    }
  }
}

void drawSelectedPad() {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(2, 5);
  display.print("Pad ");
  display.print(selectedPad + 1);
}

void drawChordNotes(int pad) {
  for (int i = 0; i < 8; i++) {
    int x = (i % 4) * 32;
    int y = 0;
    if (i < 4) {
      y = 20;
    } else {
      y = 42;
    }

    display.fillRect(x, y, 13, 17, WHITE);
    display.setTextColor(BLACK);
    switch (i) {
      case 0:
        display.setCursor(x + 4, y + 1);
        display.print("1");
        break;
      case 1:
        display.setCursor(x + 4, y + 1);
        display.print("3");
        break;
      case 2:
        display.setCursor(x + 4, y + 1);
        display.print("5");
        break;
      case 3:
        display.setCursor(x + 4, y + 1);
        display.print("7");
        break;
      case 4:
        display.setCursor(x + 4, y + 1);
        display.print("9");
        break;
      case 5:
        display.setCursor(x + 1, y + 1);
        display.print("11");
        break;
      case 6:
        display.setCursor(x + 1, y + 1);
        display.print("13");
        break;
      case 7:
        display.setCursor(x + 4, y + 1);
        display.print("B");
        break;
    }

    display.setCursor(x + 1, y + 9);
    if (pads[pad].chord.semitoneModifiers[i] != 0) {
      if (pads[pad].chord.semitoneModifiers[i] < 0) {
        display.print(pads[pad].chord.semitoneModifiers[i]);
      } else {
        display.print("+");
        display.print(pads[pad].chord.semitoneModifiers[i]);
      }
    }

    display.setTextColor(WHITE);
    if (pads[pad].chord.isActive[i]) {
      display.setCursor(x + 14, y + 1);
      display.print(midiNoteNames[settings.rootNote + pads[pad].chord.intervals[i] + (pads[pad].chord.octaveModifiers[i] * 12) + pads[pad].chord.semitoneModifiers[i]]);
    } else {
      display.setCursor(x + 14, y + 1);
      display.print("-");
    }
  }
}

void drawNoteVelocities(int pad) {
  for (int i = 0; i < 8; i++) {
    int x = (i % 4) * 32;
    int y = 0;
    if (i < 4) {
      y = 20;
    } else {
      y = 42;
    }
    if (pads[pad].chord.isActive[i]) {
      display.setCursor(x + 14, y + 9);
      display.print((int)constrain(settings.velocityScaling * (pads[pad].padVelocity + pads[pad].chord.velocityModifiers[i]), 1, 128));
    }
  }
}

void drawNoteOctaves(int pad) {
  for (int i = 0; i < 8; i++) {
    int x = (i % 4) * 32;
    int y = 0;
    if (i < 4) {
      y = 20;
    } else {
      y = 42;
    }

    display.setCursor(x + 14, y + 9);
    if (pads[pad].chord.octaveModifiers[i] < 1) {
      display.print(pads[pad].chord.octaveModifiers[i]);
    } else {
      display.print("+");
      display.print(pads[pad].chord.octaveModifiers[i]);
    }

    if (i == noteIndex) {
      display.drawLine(x, y - 2, x + 31, y - 2, WHITE);
      if (selectIndex - 1 == i) {
        display.drawLine(x + 13, y + 18, x + 31, y + 18, WHITE);
        display.drawLine(x, y + 20, x + 31, y + 20, WHITE);
      } else {
        display.drawLine(x, y + 18, x + 31, y + 18, WHITE);
      }
    }
  }
}

void drawNoteSelect() {
  for (int i = 0; i < 8; i++) {
    int x = (i % 4) * 32;
    int y = 0;
    if (i < 4) {
      y = 20;
    } else {
      y = 42;
    }
    if (i == noteIndex) {
      display.drawLine(x, y - 2, x + 31, y - 2, WHITE);
      display.drawLine(x, y + 18, x + 31, y + 18, WHITE);
    }
  }
}

void drawVelocitySelect() {
  for (int i = 0; i < 8; i++) {
    int x = (i % 4) * 32;
    int y = 0;
    if (i < 4) {
      y = 20;
    } else {
      y = 42;
    }
    if (i == noteIndex) {
      display.drawLine(x, y - 2, x + 31, y - 2, WHITE);
      if (selectIndex - 1 == i) {
        display.drawLine(x + 13, y + 18, x + 31, y + 18, WHITE);
        display.drawLine(x, y + 20, x + 31, y + 20, WHITE);
      } else {
        display.drawLine(x, y + 18, x + 31, y + 18, WHITE);
      }
    }
  }
}

void drawModSelect() {
  for (int i = 0; i < 8; i++) {
    int x = (i % 4) * 32;
    int y = 0;
    if (i < 4) {
      y = 20;
    } else {
      y = 42;
    }
    if (i == noteIndex) {
      display.drawLine(x, y - 2, x + 31, y - 2, WHITE);
      if (selectIndex - 1 == i) {
        display.drawLine(x, y + 18, x + 12, y + 18, WHITE);
        display.drawLine(x, y + 20, x + 31, y + 20, WHITE);
      } else {
        display.drawLine(x, y + 18, x + 31, y + 18, WHITE);
      }
    }
  }
}