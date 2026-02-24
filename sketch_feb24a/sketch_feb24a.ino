/*
Dev: Liam Treat, Brennen Koernke
Resources: Chat Gpt
*/

// Assign each speaker to a digital pin
int spk1 = 8; // low
int spk2 = 9; // mid
int spk3 = 10; // high

void setup() {
  pinMode(spk1, OUTPUT);
  pinMode(spk2, OUTPUT);
  pinMode(spk3, OUTPUT);
}

void loop() {
  sayHello();
  delay(800); // short pause before repeating
}

// --------------------------------------------------
// slideTripleFast
// Slides three frequencies simultaneously, very fast
// --------------------------------------------------
void slideTripleFast(int startFreq1, int endFreq1,
                     int startFreq2, int endFreq2,
                     int startFreq3, int endFreq3,
                     int steps, int duration, int vibrato) {

  float freq1 = startFreq1;
  float freq2 = startFreq2;
  float freq3 = startFreq3;

  float step1 = (float)(endFreq1 - startFreq1) / steps;
  float step2 = (float)(endFreq2 - startFreq2) / steps;
  float step3 = (float)(endFreq3 - startFreq3) / steps;

  int delayTime = duration / steps;

  for(int i = 0; i <= steps; i++) {
    // Play all three at once
    tone(spk1, freq1 + random(-vibrato, vibrato));
    tone(spk2, freq2 + random(-vibrato, vibrato));
    tone(spk3, freq3 + random(-vibrato, vibrato));

    freq1 += step1;
    freq2 += step2;
    freq3 += step3;

    // Tiny delay to speed it up
    delay(delayTime);
  }

  noTone(spk1);
  noTone(spk2);
  noTone(spk3);
}

// --------------------------------------------------
// sayHello
// Full word in ~250ms
// --------------------------------------------------
void sayHello() {

  // H -> E (very fast)
  slideTripleFast(250, 400, 350, 600, 450, 700, 3, 40, 2);

  // E -> L
  slideTripleFast(400, 350, 600, 450, 700, 500, 3, 50, 2);

  // L -> O (falling)
  slideTripleFast(350, 600, 450, 800, 500, 900, 3, 60, 2);
}