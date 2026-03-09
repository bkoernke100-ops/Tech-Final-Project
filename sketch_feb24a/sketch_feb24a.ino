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
#define A4 440
#define G4 392
#define F4 349
#define E4 330
#define D4 294
#define C4 262
#define B3 247
#define G3 196
#define E3 165
#define C3 131

void setup() {
  pinMode(SPK1, OUTPUT);
  pinMode(SPK2, OUTPUT);
  pinMode(SPK3, OUTPUT);
}

void play(unsigned int f1, unsigned int f2, unsigned int f3, int duration) {
  if (f1) tone(SPK1, f1); else noTone(SPK1);
  if (f2) tone(SPK2, f2); else noTone(SPK2);
  if (f3) tone(SPK3, f3); else noTone(SPK3);
  delay(duration);
  noTone(SPK1);
  noTone(SPK2);
  noTone(SPK3);
  delay(20);
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

  // ---- Second Phrase ----
  play(G5, D5, G4, 200);
  play(E5, B4, E4, 200);
  play(D5, A4, D4, 200);
  play(C5, G4, C4, 200);

  play(D5, A4, D4, 200);
  play(E5, B4, E4, 300);
  play(G5, D5, G4, 350);
  play(E5, B4, E4, 300);

  // ---- Bridge Section ----
  play(C5, G4, C4, 200);
  play(D5, A4, D4, 200);
  play(E5, B4, E4, 200);
  play(G5, D5, G4, 300);

  play(E5, B4, E4, 200);
  play(D5, A4, D4, 200);
  play(C5, G4, C4, 200);
  play(G4, D4, G3, 300);

  // ---- Ending Turnaround ----
  play(E5, B4, E4, 200);
  play(D5, B4, D4, 200);
  play(C5, G4, C4, 200);
  play(D5, G4, D4, 200);
  play(E5, B4, E4, 400);

  delay(1200);
}