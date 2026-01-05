#ifndef LOOPER_V2_H
#define LOOPER_V2_H

//================================ LOOPER DEFINES ================================
#define MAX_LOOP_EVENTS 256
#define LOOP_TICKS_PER_BEAT 24    // Match MIDI clock (24 PPQN)
#define LOOP_TICKS_PER_BAR (LOOP_TICKS_PER_BEAT * 4)  // 96 ticks per bar (4/4 time)

// Loop length options
#define LOOP_LENGTH_1_BAR   0
#define LOOP_LENGTH_2_BARS  1
#define LOOP_LENGTH_4_BARS  2   // Default
#define LOOP_LENGTH_FREE    3

// Looper LED color
#define COLOR_LOOPER_REC    0xFF0000  // Red for recording
#define COLOR_LOOPER_PLAY   0x00FF00  // Green for playing
#define COLOR_LOOPER_OVER   0xFF6600  // Orange for overdub
#define COLOR_LOOPER_IDLE   0xFF00FF  // Magenta when has content

//================================ DATA STRUCTURES ================================

// Single recorded MIDI event - 6 bytes each
// Bit 7 of velocityAndFlags: 0=noteOn, 1=noteOff
struct LoopEvent {
  uint32_t timestamp;       // Tick position (0 to loopLengthTicks-1)
  uint8_t note;             // MIDI note (0-127)
  uint8_t velocityAndFlags; // bits 0-6: velocity, bit 7: isNoteOff
};

// Helper macros for LoopEvent
#define LOOP_EVENT_IS_OFF(e) ((e).velocityAndFlags & 0x80)
#define LOOP_EVENT_VELOCITY(e) ((e).velocityAndFlags & 0x7F)
#define LOOP_EVENT_SET_ON(e, vel) ((e).velocityAndFlags = (vel) & 0x7F)
#define LOOP_EVENT_SET_OFF(e, vel) ((e).velocityAndFlags = 0x80 | ((vel) & 0x7F))

// Looper state structure
struct LooperState {
  // State flags
  bool recording = false;        // Currently recording (first pass)
  bool overdubbing = false;      // Overdub mode (record while playing)
  bool playing = false;          // Playback active
  bool hasContent = false;       // Loop has recorded content

  // Timing
  uint8_t loopLengthBars = LOOP_LENGTH_1_BAR;  // Default: 1 bar
  uint32_t loopLengthTicks = LOOP_TICKS_PER_BAR;  // 96 ticks
  uint32_t currentTick = 0;      // Current position (0 to loopLengthTicks-1)
  uint32_t recordStartTick = 0;  // For FREE mode: when recording started

  // Event storage
  LoopEvent events[MAX_LOOP_EVENTS];
  uint16_t eventCount = 0;       // Number of recorded events
  uint16_t playbackIndex = 0;    // Next event to play

  // For LED feedback during playback
  int8_t lastPlayedPad = -1;     // Pad index of last played note (-1 if none)
  unsigned long lastPlayedTime = 0;  // For LED timing

  // Prevent looper playback from being re-recorded
  bool isPlayingBack = false;    // True while sending playback notes

  // Animation for newly recorded notes
  unsigned long lastRecordTime = 0;  // When last note was recorded
  uint32_t lastRecordTick = 0;       // Tick position of last recorded note
  uint8_t lastRecordNote = 0;        // Note value of last recorded note
};

// Global looper state (declared in main sketch)
extern LooperState looper;

// External references - functions and display object
extern Adafruit_SSD1306 display;
extern void sendNoteOn(int note, int velocity, int channel);
extern void sendNoteOff(int note, int velocity, int channel);
extern void killAllNotes();
extern int getLooperOutputChannel();  // Returns settings.midiOutputAChannel

//================================ FUNCTION IMPLEMENTATIONS ================================

// Initialize looper (call from setup)
void initLooper() {
  looper.recording = false;
  looper.overdubbing = false;
  looper.playing = false;
  looper.hasContent = false;
  looper.eventCount = 0;
  looper.currentTick = 0;
  looper.playbackIndex = 0;
  looper.lastPlayedPad = -1;
  looper.loopLengthBars = LOOP_LENGTH_1_BAR;
  looper.loopLengthTicks = LOOP_TICKS_PER_BAR;
  looper.isPlayingBack = false;
}

// Calculate loop length in ticks based on bars setting
void looperCalculateLoopLength() {
  switch (looper.loopLengthBars) {
    case LOOP_LENGTH_1_BAR:
      looper.loopLengthTicks = LOOP_TICKS_PER_BAR;       // 96 ticks
      break;
    case LOOP_LENGTH_2_BARS:
      looper.loopLengthTicks = LOOP_TICKS_PER_BAR * 2;   // 192 ticks
      break;
    case LOOP_LENGTH_4_BARS:
      looper.loopLengthTicks = LOOP_TICKS_PER_BAR * 4;   // 384 ticks
      break;
    case LOOP_LENGTH_FREE:
      looper.loopLengthTicks = 0;  // Will be set when recording stops
      break;
  }
}

// Record a note-on event
void looperRecordNoteOn(uint8_t note, uint8_t velocity) {
  if (looper.eventCount >= MAX_LOOP_EVENTS) return;
  if (!looper.recording && !looper.overdubbing) return;
  if (looper.isPlayingBack) return;  // Don't re-record playback notes

  LoopEvent evt;
  evt.timestamp = looper.currentTick;
  evt.note = note;
  LOOP_EVENT_SET_ON(evt, velocity);

  // Insert sorted by timestamp
  int insertPos = looper.eventCount;
  while (insertPos > 0 && looper.events[insertPos - 1].timestamp > evt.timestamp) {
    looper.events[insertPos] = looper.events[insertPos - 1];
    insertPos--;
  }
  looper.events[insertPos] = evt;
  looper.eventCount++;

  // Track for animation
  looper.lastRecordTime = millis();
  looper.lastRecordTick = looper.currentTick;
  looper.lastRecordNote = note;
}

// Record a note-off event
void looperRecordNoteOff(uint8_t note, uint8_t velocity) {
  if (looper.eventCount >= MAX_LOOP_EVENTS) return;
  if (!looper.recording && !looper.overdubbing) return;
  if (looper.isPlayingBack) return;  // Don't re-record playback notes

  LoopEvent evt;
  evt.timestamp = looper.currentTick;
  evt.note = note;
  LOOP_EVENT_SET_OFF(evt, velocity);

  // Insert sorted by timestamp
  int insertPos = looper.eventCount;
  while (insertPos > 0 && looper.events[insertPos - 1].timestamp > evt.timestamp) {
    looper.events[insertPos] = looper.events[insertPos - 1];
    insertPos--;
  }
  looper.events[insertPos] = evt;
  looper.eventCount++;
}

// Find which pad corresponds to a given MIDI note (for LED feedback)
// This is implemented in the main sketch where pads[] is accessible
extern int looperFindPadForNoteImpl(uint8_t note);

int looperFindPadForNote(uint8_t note) {
  return looperFindPadForNoteImpl(note);
}

// Toggle between record/overdub/play states (Shift+Oct-)
void looperToggleRecordOverdub() {
  if (!looper.hasContent && !looper.recording) {
    // No content: Start fresh recording
    looper.recording = true;
    looper.overdubbing = false;
    looper.playing = false;
    looper.eventCount = 0;
    looper.currentTick = 0;
    looper.recordStartTick = 0;
    looper.playbackIndex = 0;
    looperCalculateLoopLength();
  } else if (looper.recording) {
    // Was recording first pass: Stop and start playback
    looper.recording = false;
    looper.hasContent = (looper.eventCount > 0);
    if (looper.hasContent) {
      // Finalize loop length for FREE mode
      if (looper.loopLengthBars == LOOP_LENGTH_FREE) {
        looper.loopLengthTicks = looper.currentTick > 0 ? looper.currentTick : LOOP_TICKS_PER_BAR;
      }
      looper.playing = true;
      looper.currentTick = 0;
      looper.playbackIndex = 0;
    }
  } else if (looper.overdubbing) {
    // Was overdubbing: Stop overdub, continue playback
    looper.overdubbing = false;
  } else if (looper.playing) {
    // Was playing: Start overdub
    looper.overdubbing = true;
  } else {
    // Has content but stopped: Start playback
    looper.playing = true;
    looper.currentTick = 0;
    looper.playbackIndex = 0;
  }
}

// Clear all loop content (Shift+Oct+)
void looperClear() {
  looper.recording = false;
  looper.overdubbing = false;
  looper.playing = false;
  looper.hasContent = false;
  looper.eventCount = 0;
  looper.currentTick = 0;
  looper.playbackIndex = 0;
  looper.lastPlayedPad = -1;
  looper.isPlayingBack = false;

  killAllNotes();  // Stop any hanging notes
}

// Called on each MIDI clock tick (24 PPQN) - advances looper timing
void looperClockTick() {
  if (!looper.playing && !looper.recording && !looper.overdubbing) {
    return;
  }

  // Playback: trigger events at current tick
  if (looper.playing || looper.overdubbing) {
    looper.isPlayingBack = true;  // Prevent re-recording playback
    // Play all events at current tick
    while (looper.playbackIndex < looper.eventCount &&
           looper.events[looper.playbackIndex].timestamp == looper.currentTick) {
      LoopEvent& evt = looper.events[looper.playbackIndex];

      if (LOOP_EVENT_IS_OFF(evt)) {
        sendNoteOff(evt.note, LOOP_EVENT_VELOCITY(evt), getLooperOutputChannel());
      } else {
        sendNoteOn(evt.note, LOOP_EVENT_VELOCITY(evt), getLooperOutputChannel());
        // LED feedback - find which pad plays this note
        looper.lastPlayedPad = looperFindPadForNote(evt.note);
        looper.lastPlayedTime = millis();
      }

      looper.playbackIndex++;
    }
    looper.isPlayingBack = false;
  }

  // Advance tick counter
  looper.currentTick++;

  // Handle loop wrap or FREE mode extension
  if (looper.loopLengthBars == LOOP_LENGTH_FREE && looper.recording) {
    // FREE mode during first recording: no limit, just keep going
    // (will be finalized when recording stops)
  } else if (looper.loopLengthTicks > 0 && looper.currentTick >= looper.loopLengthTicks) {
    // Fixed length or established FREE length: wrap around
    looper.currentTick = 0;
    looper.playbackIndex = 0;

    if (looper.recording) {
      // First loop complete - switch to overdub+play mode
      looper.recording = false;
      looper.overdubbing = true;
      looper.playing = true;
      looper.hasContent = true;
    }
  }
}

// Main update function (called from loop())
void updateLooper() {
  // Clear LED feedback after 100ms
  if (looper.lastPlayedPad >= 0 && millis() - looper.lastPlayedTime > 100) {
    looper.lastPlayedPad = -1;
  }
}

// Draw looper display screen
void drawLooperScreen() {
  // Centered status text at top
  display.setTextSize(1);
  const char* statusText = "";
  if (looper.recording) {
    if ((millis() / 250) % 2) statusText = "* REC *";
  } else if (looper.overdubbing) {
    if ((millis() / 300) % 2) statusText = "OVERDUB";
  } else if (looper.playing) {
    statusText = "PLAYING";
  } else if (looper.hasContent) {
    statusText = "STOPPED";
  }
  int textWidth = strlen(statusText) * 6;
  display.setCursor(64 - textWidth / 2, 0);
  display.print(statusText);

  // Beat markers at top (4 beats for 1 bar)
  int totalBeats = looper.loopLengthTicks / LOOP_TICKS_PER_BEAT;
  for (int b = 0; b <= totalBeats; b++) {
    int x = 4 + (b * 120) / totalBeats;
    display.drawFastVLine(x, 10, 3, WHITE);
  }

  // Note visualization area: y=14 to y=48 (34 pixels height)
  // X = time (4 to 124), Y = pitch mapped to display

  // Draw all recorded note-on events as dots
  unsigned long now = millis();
  bool isNewNoteFlash = (now - looper.lastRecordTime) < 400;

  for (int i = 0; i < looper.eventCount; i++) {
    LoopEvent& evt = looper.events[i];
    if (LOOP_EVENT_IS_OFF(evt)) continue;  // Only show note-ons

    // X position based on timestamp
    int x = 4 + ((evt.timestamp * 120) / looper.loopLengthTicks);
    x = constrain(x, 4, 124);

    // Y position based on note pitch (map MIDI note to 34 pixel range)
    // Use note modulo 36 to fit ~3 octaves, inverted so high notes are at top
    int noteOffset = evt.note % 36;  // 3 octaves range
    int y = 47 - (noteOffset * 33) / 35;  // Map to 14-47 range

    // Check if this is a recently added note
    bool isRecent = isNewNoteFlash &&
                    evt.timestamp == looper.lastRecordTick &&
                    evt.note == looper.lastRecordNote;

    if (isRecent) {
      // Animated expanding circle for new notes
      int pulseSize = 2 + ((now - looper.lastRecordTime) / 50) % 4;
      display.drawCircle(x, y, pulseSize, WHITE);
      display.fillCircle(x, y, 2, WHITE);
    } else {
      // Normal dot for existing notes
      display.fillCircle(x, y, 1, WHITE);
    }
  }

  // Playhead - simple vertical line with small triangle at top
  if (looper.loopLengthTicks > 0 && (looper.playing || looper.recording || looper.overdubbing)) {
    int playheadX = 4 + ((looper.currentTick * 120) / looper.loopLengthTicks);
    playheadX = constrain(playheadX, 4, 124);

    // Thin line
    display.drawFastVLine(playheadX, 14, 34, WHITE);
    // Small triangle at top
    display.fillTriangle(playheadX - 2, 13, playheadX + 2, 13, playheadX, 16, WHITE);
  }

  // Divider line
  display.drawFastHLine(0, 50, 128, WHITE);

  // Bottom: beat counter + note count
  display.setTextSize(1);
  int currentBeat = (looper.currentTick / LOOP_TICKS_PER_BEAT) + 1;

  // Left: beat
  display.setCursor(4, 54);
  display.print(currentBeat);
  display.print("/");
  display.print(totalBeats);

  // Right: note count (only note-ons)
  int noteOnCount = 0;
  for (int i = 0; i < looper.eventCount; i++) {
    if (!LOOP_EVENT_IS_OFF(looper.events[i])) noteOnCount++;
  }
  char noteStr[12];
  snprintf(noteStr, sizeof(noteStr), "%d notes", noteOnCount);
  int w = strlen(noteStr) * 6;
  display.setCursor(124 - w, 54);
  display.print(noteStr);
}

#endif // LOOPER_V2_H
