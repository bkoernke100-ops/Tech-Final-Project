/*
Dev: Liam Treat, Brennen Koernke
Resources: Chat Gpt
*/

// ===== 3 SPEAKER DR MARIO "FEVER" =====
// Melody = pin 9
// Harmony = pin 10
// Bass = pin 11

#define SPK1 9
#define SPK2 10
#define SPK3 11

// Note Frequencies
#define E5 659
#define D5 587
#define C5 523
#define G5 784
#define B4 494
#define G4 392
#define C4 262

volatile unsigned long phase1 = 0;
volatile unsigned long phase2 = 0;
volatile unsigned long phase3 = 0;

volatile unsigned long freq1 = 0;
volatile unsigned long freq2 = 0;
volatile unsigned long freq3 = 0;

void setupTimer() {
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 159;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

ISR(TIMER1_COMPA_vect) {
  if (freq1) {
    phase1 += freq1;
    digitalWrite(SPK1, (phase1 & 0x8000) ? HIGH : LOW);
  }
  if (freq2) {
    phase2 += freq2;
    digitalWrite(SPK2, (phase2 & 0x8000) ? HIGH : LOW);
  }
  if (freq3) {
    phase3 += freq3;
    digitalWrite(SPK3, (phase3 & 0x8000) ? HIGH : LOW);
  }
}

void play(unsigned int f1, unsigned int f2, unsigned int f3, int duration) {
  freq1 = f1;
  freq2 = f2;
  freq3 = f3;
  delay(duration);
  freq1 = freq2 = freq3 = 0;
  delay(20);
}

void setup() {
  pinMode(SPK1, OUTPUT);
  pinMode(SPK2, OUTPUT);
  pinMode(SPK3, OUTPUT);
  setupTimer();
}

void loop() {

  // ---- Main Fever Riff ----
  play(E5, B4, E4, 180);
  play(D5, B4, D4, 180);
  play(C5, G4, C4, 180);
  play(D5, G4, D4, 180);

  play(E5, B4, E4, 250);
  play(G5, D5, G4, 350);
  play(E5, B4, E4, 300);

  play(D5, B4, D4, 180);
  play(C5, G4, C4, 180);
  play(D5, G4, D4, 180);
  play(E5, B4, E4, 250);
  play(G5, D5, G4, 350);
  play(E5, B4, E4, 400);

  delay(1000);
}