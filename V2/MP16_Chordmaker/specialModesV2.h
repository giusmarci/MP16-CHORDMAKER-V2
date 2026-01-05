#ifndef SPECIAL_MODES_V2_H
#define SPECIAL_MODES_V2_H

//================================ SPECIAL MODES ================================
// Special play modes that enhance the chord playing experience

// Mode definitions
#define SPECIAL_MODE_NORMAL     0
#define SPECIAL_MODE_GENERATIVE 1
#define SPECIAL_MODE_GLIDE      2

#define NUM_SPECIAL_MODES 3

const char* specialModeNames[NUM_SPECIAL_MODES] = {
  "Normal",
  "Generative",
  "Glide"
};

// Short names for display indicator
const char* specialModeShort[NUM_SPECIAL_MODES] = {
  "",      // Normal shows nothing
  "GEN",   // Generative shows GEN
  "GLI"    // Glide shows GLI
};

//================================ GENERATIVE MODE ================================
// Notes slowly evolve/mutate over time while playing

struct GenerativeState {
  unsigned long lastMutationTime = 0;
  bool active = false;  // Is generative mode currently running
};

//================================ GLIDE MODE ================================
// Pitch-bend based portamento - works with any synth
// Glides from previous note to new note using pitch bend

#define GLIDE_PITCH_BEND_CENTER 8192   // Center position (no bend)
#define GLIDE_PITCH_BEND_RANGE  2      // Semitones (±2 = standard synth default)

struct GlideState {
  bool active = false;              // Is a glide currently in progress
  unsigned long startTime = 0;      // When glide started
  int startBend = 8192;             // Starting pitch bend value
  int targetBend = 8192;            // Target pitch bend (usually center)
  // For chord mode
  int lastRootNote = -1;            // Target root note (what we're bending towards)
  int lastPad = -1;                 // Target pad (what we're bending towards)
  int sourcePad = -1;               // Pad that "owns" the notes for release purposes
  int noteSourcePad = -1;           // Pad whose actual MIDI notes are sounding (for stopChord)
  int oldPadToStop = -1;            // Old pad to stop when glide completes (for overlap)
  // For arp mode (note-by-note glide)
  int lastArpNote = -1;             // Last arp note played (for mono-style glide)
  // Settings page (0=Time, 1=Mono/Poly)
  int settingsPage = 0;
  // For CC84 polyphonic glide - store last chord's notes
  int lastChordNotes[8] = {-1, -1, -1, -1, -1, -1, -1, -1};
  int lastChordNoteCount = 0;
};

//================================ SCREENSAVER ================================
// "Cyber Rain" - Matrix-style digital rain animation

#define MATRIX_COLS 16      // 128px / 8px spacing
#define MATRIX_COL_SPACING 8

struct MatrixCol {
  float y;       // Current Y position of the "head"
  float speed;   // Fall speed
  int len;       // Length of the tail
  int type;      // 0=Standard, 1=Fast/Glitch
};

struct ScreensaverState {
  bool active = false;
  unsigned long lastInputTime = 0;
  MatrixCol cols[MATRIX_COLS];
  bool initialized = false;
};

// Initialize Cyber Rain
void initScreensaver(ScreensaverState& ss) {
  for (int i = 0; i < MATRIX_COLS; i++) {
    // Randomize start positions (some off-screen to stagger)
    ss.cols[i].y = random(-100, 0); 
    ss.cols[i].speed = random(5, 25) / 10.0f; // 0.5 to 2.5
    ss.cols[i].len = random(4, 25);
    ss.cols[i].type = (random(0, 10) > 8) ? 1 : 0; // 10% chance of glitch column
  }
  ss.initialized = true;
}

// Update Cyber Rain positions
void updateScreensaver(ScreensaverState& ss) {
  for (int i = 0; i < MATRIX_COLS; i++) {
    MatrixCol& col = ss.cols[i];
    
    // Move down
    col.y += col.speed;
    
    // Glitch effect: sometimes speed up randomly
    if (col.type == 1 && random(0, 20) == 0) {
      col.y += 2.0;
    }

    // Reset when tail leaves screen
    if (col.y - col.len > 64) {
      col.y = random(-50, -5);
      col.speed = random(5, 30) / 10.0f;
      col.len = random(4, 30);
      col.type = (random(0, 10) > 8) ? 1 : 0;
    }
  }
}

// Draw Cyber Rain on display
void drawScreensaver(Adafruit_SSD1306& display, ScreensaverState& ss) {
  display.clearDisplay();

  for (int i = 0; i < MATRIX_COLS; i++) {
    MatrixCol& col = ss.cols[i];
    int x = i * MATRIX_COL_SPACING + 2; // Center in column
    int headY = (int)col.y;
    
    // Don't draw if completely off screen
    if (headY - col.len > 64 || headY < 0) continue;

    // Draw Drop
    for (int j = 0; j < col.len; j++) {
      int pixelY = headY - j;
      if (pixelY >= 0 && pixelY < 64) {
        // Head is solid
        if (j == 0) {
          display.drawPixel(x, pixelY, WHITE);
          display.drawPixel(x+1, pixelY, WHITE); // Thicker head
        }
        // Tail is dithered (skip pixels to simulate fade)
        else if (j < col.len / 3) {
          // Solid top part of tail
          display.drawPixel(x, pixelY, WHITE);
        }
        else if (j < col.len * 2 / 3) {
          // 50% dither
          if (pixelY % 2 == 0) display.drawPixel(x, pixelY, WHITE);
        }
        else {
          // 25% dither (sparse tail)
          if (pixelY % 4 == 0) display.drawPixel(x, pixelY, WHITE);
        }
      }
    }
  }

  display.display();
}

#endif // SPECIAL_MODES_V2_H
