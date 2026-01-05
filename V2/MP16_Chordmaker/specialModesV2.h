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
  int lastRootNote = -1;            // Last chord's root note
  int lastPad = -1;                 // Last pad played
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
// Starfield animation when idle

#define SCREENSAVER_NUM_STARS 20

struct Star {
  float x;       // Position from center (-1 to 1)
  float y;       // Position from center (-1 to 1)
  float speed;   // Movement speed multiplier
};

struct ScreensaverState {
  bool active = false;
  unsigned long lastInputTime = 0;
  Star stars[SCREENSAVER_NUM_STARS];
  bool initialized = false;
};

// Initialize starfield
void initStarfield(ScreensaverState& ss) {
  for (int i = 0; i < SCREENSAVER_NUM_STARS; i++) {
    // Random position near center
    ss.stars[i].x = ((float)random(-100, 100)) / 1000.0f;
    ss.stars[i].y = ((float)random(-100, 100)) / 1000.0f;
    ss.stars[i].speed = 0.5f + ((float)random(0, 100)) / 100.0f;
  }
  ss.initialized = true;
}

// Reset a star to center
void resetStar(Star& star) {
  star.x = ((float)random(-50, 50)) / 1000.0f;
  star.y = ((float)random(-50, 50)) / 1000.0f;
  star.speed = 0.5f + ((float)random(0, 100)) / 100.0f;
}

// Update starfield positions
void updateStarfield(ScreensaverState& ss) {
  for (int i = 0; i < SCREENSAVER_NUM_STARS; i++) {
    Star& star = ss.stars[i];

    // Move outward from center (speed increases with distance)
    float dist = sqrt(star.x * star.x + star.y * star.y);
    float speedMult = 1.0f + dist * 2.0f;  // Slower acceleration

    if (dist > 0.001f) {
      star.x += (star.x / dist) * star.speed * speedMult * 0.008f;  // Much slower
      star.y += (star.y / dist) * star.speed * speedMult * 0.008f;
    } else {
      // Nudge stars at exact center
      star.x += 0.005f;
    }

    // Reset if off screen
    if (abs(star.x) > 1.2f || abs(star.y) > 1.2f) {
      resetStar(star);
    }
  }
}

// Draw starfield on display (pass display reference)
// Screen coords: 0,0 to 127,63, center at 64,32
void drawStarfield(Adafruit_SSD1306& display, ScreensaverState& ss) {
  display.clearDisplay();

  for (int i = 0; i < SCREENSAVER_NUM_STARS; i++) {
    Star& star = ss.stars[i];

    // Convert -1..1 to screen coords
    int screenX = 64 + (int)(star.x * 64);
    int screenY = 32 + (int)(star.y * 32);

    // Only draw if on screen
    if (screenX >= 0 && screenX < 128 && screenY >= 0 && screenY < 64) {
      // Size based on distance from center (further = bigger)
      float dist = sqrt(star.x * star.x + star.y * star.y);
      if (dist > 0.5f) {
        display.fillRect(screenX, screenY, 2, 2, WHITE);
      } else {
        display.drawPixel(screenX, screenY, WHITE);
      }
    }
  }

  display.display();
}

#endif // SPECIAL_MODES_V2_H
