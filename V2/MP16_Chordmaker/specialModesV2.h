#ifndef SPECIAL_MODES_V2_H
#define SPECIAL_MODES_V2_H

//================================ SPECIAL MODES ================================
// Special play modes that enhance the chord playing experience

// Mode definitions
#define SPECIAL_MODE_NORMAL     0
#define SPECIAL_MODE_GENERATIVE 1
// Future: SPECIAL_MODE_SEQUENCE 2

#define NUM_SPECIAL_MODES 2

const char* specialModeNames[NUM_SPECIAL_MODES] = {
  "Normal",
  "Generative"
};

// Short names for display indicator
const char* specialModeShort[NUM_SPECIAL_MODES] = {
  "",      // Normal shows nothing
  "GEN"    // Generative shows GEN
};

//================================ GENERATIVE MODE ================================
// Notes slowly evolve/mutate over time while playing

struct GenerativeState {
  unsigned long lastMutationTime = 0;
  bool active = false;  // Is generative mode currently running
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
    float speedMult = 1.0f + dist * 3.0f;  // Accelerate as they get further

    if (dist > 0.001f) {
      star.x += (star.x / dist) * star.speed * speedMult * 0.02f;
      star.y += (star.y / dist) * star.speed * speedMult * 0.02f;
    } else {
      // Nudge stars at exact center
      star.x += 0.01f;
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
