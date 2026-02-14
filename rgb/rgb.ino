#define R 2
#define G 3
#define B 4

void setup() {
  // put your setup code here, to run once:
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  digitalWrite(R, HIGH);
  digitalWrite(G, HIGH);
  digitalWrite(B, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(R, LOW);
  digitalWrite(G, LOW);
  digitalWrite(B, LOW);
}
