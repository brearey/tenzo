 #include "HX711.h"

// ---------------------- Hardware ----------------------
#define DT_PIN A0
#define SCK_PIN A1
#define R 2
#define G 3
#define B 4
#define VCC 13

HX711 scale;

// ---------------------- Timing ------------------------
const unsigned long SERIAL_PERIOD_MS = 50;   // ~20 Hz for Serial Plotter
const unsigned long FULL_HOLD_MS = 220;      // full break must hold this long
const unsigned long PARTIAL_MIN_MS = 260;    // partial drop candidate time
const unsigned long PARTIAL_STABLE_MS = 300; // stabilization at new level
const unsigned long RECOVER_MS = 900;        // recovery time after alarm

// ------------------ Adaptive logic --------------------
const float ALPHA_FAST = 0.22f;   // fast EMA on filtered signal
const float ALPHA_BASE = 0.004f;  // slow baseline tracking (zero drift)
const float ALPHA_NOISE = 0.03f;  // noise envelope update

const float PARTIAL_DROP_BASE = 0.22f; // 22% base threshold
const float FULL_DROP_BASE = 0.88f;    // 88% base threshold

const float PARTIAL_NOISE_GAIN = 3.0f; // dynamic margin from noise
const float FULL_NOISE_GAIN = 1.5f;

const float BASELINE_FREEZE_DROP = 0.10f; // do not adapt baseline on deep drop
const float RECOVER_DROP = 0.08f;         // considered recovered below this drop

const float MIN_BASELINE = 1500.0f; // protects ratio if load is too small
const float MIN_STABLE_BAND = 120.0f;

// -------------------- Event codes ---------------------
const uint8_t EVENT_NONE = 0;
const uint8_t EVENT_FULL = 1;
const uint8_t EVENT_PARTIAL = 2;

enum DetectorState {
  ST_INIT = 0,
  ST_NORMAL = 1,
  ST_ALARM = 2
};

DetectorState state = ST_INIT;

// Median-5 buffer
float ring5[5] = {0, 0, 0, 0, 0};
uint8_t ringIndex = 0;
bool ringFilled = false;

// Signal stats
float fast = 0.0f;
float baseline = 0.0f; // baseline of |signal|
float noise = 0.0f;
float partialLevel = 0.0f;

// Timers / counters
unsigned long tPrev = 0;
unsigned long tLastPrint = 0;
unsigned long tStart = 0;
unsigned long fullMs = 0;
unsigned long partialMs = 0;
unsigned long stableMs = 0;
unsigned long recoverMs = 0;

uint8_t eventCode = EVENT_NONE;

float median5(float x) {
  ring5[ringIndex] = x;
  ringIndex = (ringIndex + 1) % 5;
  if (ringIndex == 0) {
    ringFilled = true;
  }

  float tmp[5];
  uint8_t n = ringFilled ? 5 : ringIndex;
  if (n == 0) {
    return x;
  }

  for (uint8_t i = 0; i < n; i++) {
    tmp[i] = ring5[i];
  }

  // Insertion sort for very small n
  for (uint8_t i = 1; i < n; i++) {
    float key = tmp[i];
    int8_t j = i - 1;
    while (j >= 0 && tmp[j] > key) {
      tmp[j + 1] = tmp[j];
      j--;
    }
    tmp[j + 1] = key;
  }

  return tmp[n / 2];
}

float clampf(float x, float lo, float hi) {
  if (x < lo) return lo;
  if (x > hi) return hi;
  return x;
}

void triggerEvent(uint8_t code, float absSignal) {
  eventCode = code;
  state = ST_ALARM;
  recoverMs = 0;

  // One-line marker for Serial Monitor (Plotter ignores non-numeric columns)
  Serial.print("EVENT,");
  Serial.print((unsigned long)((millis() - tStart) / 1000UL));
  Serial.print(",");
  Serial.print(code);
  Serial.print(",");
  Serial.println((long)absSignal);
}

void setup() {
  Serial.begin(115200);
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale();
  scale.tare();

  tStart = millis();
  tPrev = millis();

  // Warmup baseline from first ~1.5 s
  float sumAbs = 0.0f;
  const int warmupN = 30;
  for (int i = 0; i < warmupN; i++) {
    float raw = scale.get_units(1);
    float a = raw >= 0 ? raw : -raw;
    sumAbs += a;
    delay(50);
  }

  baseline = sumAbs / warmupN;
  if (baseline < MIN_BASELINE) baseline = MIN_BASELINE;
  fast = baseline;
  noise = baseline * 0.01f;
  partialLevel = baseline;

  state = ST_NORMAL;
  
  // RGB LED
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(VCC, OUTPUT);

  digitalWrite(R, HIGH);
  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(VCC, HIGH);
}

void lightEvent(uint8_t code) {
  switch (code)
  {
  case EVENT_FULL:
    // RED LED
    digitalWrite(R, LOW);
    digitalWrite(G, HIGH);
    digitalWrite(B, HIGH);
    break;
  
  case EVENT_PARTIAL:
    // BLUE LED
    digitalWrite(R, HIGH);
    digitalWrite(G, HIGH);
    digitalWrite(B, LOW);
    break;
  
  case EVENT_NONE:
    // GREEN LED
    digitalWrite(R, HIGH);
    digitalWrite(G, LOW);
    digitalWrite(B, HIGH);
    break;
  
  default:
    break;
  }
}

void loop() {
  unsigned long now = millis();
  unsigned long dt = now - tPrev;
  tPrev = now;

  // 1) Read + median filter + fast EMA
  float raw = scale.get_units(1);
  float med = median5(raw);
  float absMed = med >= 0 ? med : -med;
  fast += ALPHA_FAST * (absMed - fast);

  // 2) Relative drop against adaptive baseline
  float baseSafe = baseline;
  if (baseSafe < MIN_BASELINE) baseSafe = MIN_BASELINE;
  float relDrop = (baseSafe - fast) / baseSafe; // 0..1 is expected
  relDrop = clampf(relDrop, 0.0f, 1.2f);

  // 3) Noise envelope
  float dev = fast - baseline;
  if (dev < 0) dev = -dev;
  noise += ALPHA_NOISE * (dev - noise);

  // 4) Adaptive thresholds
  float noiseNorm = noise / baseSafe;
  float thrPartial = PARTIAL_DROP_BASE + PARTIAL_NOISE_GAIN * noiseNorm;
  float thrFull = FULL_DROP_BASE + FULL_NOISE_GAIN * noiseNorm;
  thrPartial = clampf(thrPartial, 0.18f, 0.45f);
  thrFull = clampf(thrFull, 0.80f, 0.96f);

  // 5) Baseline adaptation (tracks zero drift, but frozen on likely авария)
  if (state == ST_NORMAL && relDrop < BASELINE_FREEZE_DROP) {
    baseline += ALPHA_BASE * (fast - baseline);
    if (baseline < MIN_BASELINE) baseline = MIN_BASELINE;
  }

  // 6) Event detection
  if (state == ST_NORMAL) {
    // Full break candidate
    if (relDrop > thrFull) {
      fullMs += dt;
      if (fullMs >= FULL_HOLD_MS) {
        triggerEvent(EVENT_FULL, fast);
      }
    } else {
      fullMs = 0;
    }

    // Partial break candidate: between partial and full zones
    if (relDrop > thrPartial && relDrop < thrFull) {
      partialMs += dt;

      // Track the new candidate level and check stabilization around it
      partialLevel += 0.08f * (fast - partialLevel);
      float band = noise * 2.2f + MIN_STABLE_BAND;
      float e = fast - partialLevel;
      if (e < 0) e = -e;

      if (e < band) {
        stableMs += dt;
      } else {
        stableMs = 0;
      }

      if (partialMs >= PARTIAL_MIN_MS && stableMs >= PARTIAL_STABLE_MS) {
        triggerEvent(EVENT_PARTIAL, fast);
      }
    } else {
      partialMs = 0;
      stableMs = 0;
      partialLevel = fast;
    }

  } else if (state == ST_ALARM) {
    // Wait for recovery before re-arming detector
    if (relDrop < RECOVER_DROP) {
      recoverMs += dt;
      if (recoverMs >= RECOVER_MS) {
        state = ST_NORMAL;
        eventCode = EVENT_NONE;
        fullMs = 0;
        partialMs = 0;
        stableMs = 0;
      }
    } else {
      recoverMs = 0;
    }
  }

  // 7) Plotter output (single numeric frame)
  if (now - tLastPrint >= SERIAL_PERIOD_MS) {
    tLastPrint = now;

    float partialLine = baseline * (1.0f - thrPartial);
    float fullLine = baseline * (1.0f - thrFull);
    float dropPct = relDrop * 100.0f;

    Serial.print("sig:");
    Serial.print(fast, 1);
    Serial.print("\tbase:");
    Serial.print(baseline, 1);
    Serial.print("\tpartial_line:");
    Serial.print(partialLine, 1);
    Serial.print("\tfull_line:");
    Serial.print(fullLine, 1);
    Serial.print("\tdrop_pct:");
    Serial.print(dropPct, 1);
    Serial.print("\tthr_partial_pct:");
    Serial.print(thrPartial * 100.0f, 1);
    Serial.print("\tthr_full_pct:");
    Serial.print(thrFull * 100.0f, 1);
    Serial.print("\tevent:");
    Serial.print(eventCode);
    Serial.print("\tstate:");
    Serial.println((int)state);

    lightEvent(eventCode);
  }
}
