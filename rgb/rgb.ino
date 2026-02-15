#define R 2
#define G 3
#define B 4
#define VCC 13

const uint8_t EVENT_NONE = 0;
const uint8_t EVENT_FULL = 1;
const uint8_t EVENT_PARTIAL = 2;

uint8_t eventCode = EVENT_NONE;

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

void setup() {
  // put your setup code here, to run once:
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(VCC, OUTPUT);

  digitalWrite(R, HIGH);
  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(VCC, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  lightEvent(EVENT_PARTIAL);
}
