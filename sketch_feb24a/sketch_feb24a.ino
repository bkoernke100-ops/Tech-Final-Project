/*
Dev: Liam Treat, Brennen Koernke
Resources: Chat Gpt
*/

#include <Arduino_LED_Matrix.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

int score = 0;

ArduinoLEDMatrix matrix;

// -------- SCORE LEDS --------
#define LED100 2
#define LED200 3
#define LED300 4
#define LED400 5
#define LED500 6
#define LED600 7

#define WIDTH 12
#define HEIGHT 8

#define JOY_X A0
#define JOY_Y A1
#define JOY_SW A2

// ---------------- SPEAKERS ----------------
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
#define E4 330
#define D4 294
#define C4 262
#define G3 196

// Song structure
struct Note{
  int m;
  int h;
  int b;
  int dur;
};

Note song[]={
{E5,B4,E4,180},
{D5,B4,D4,180},
{C5,G4,C4,180},
{D5,G4,D4,180},
{E5,B4,E4,250},
{G5,D5,G4,350},
{E5,B4,E4,300},
{D5,B4,D4,180},
{C5,G4,C4,180},
{D5,G4,D4,180},
{E5,B4,E4,250},
{G5,D5,G4,350},
{E5,B4,E4,400}
};

int songLength=sizeof(song)/sizeof(song[0]);
int currentNote=0;
unsigned long lastNoteTime=0;

// ---------------- GAME VARIABLES ----------------

uint8_t board[HEIGHT][WIDTH];
uint8_t frame[HEIGHT][WIDTH];

int pieceX,pieceY;
int currentPiece;
int rotationState=0;

unsigned long lastDrop=0;
unsigned long dropInterval=1000;

unsigned long lastMove=0;

// 7 Tetris pieces
const uint8_t pieces[7][4][4][4]={

{{{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}},
 {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
 {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}},
 {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}},

{{{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
 {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
 {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
 {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}},

{{{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
 {{0,1,0,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
 {{0,0,0,0},{1,1,1,0},{0,1,0,0},{0,0,0,0}},
 {{0,1,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}},

{{{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
 {{0,1,1,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}},
 {{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,0,0}},
 {{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}}},

{{{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
 {{0,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,0,0}},
 {{0,0,0,0},{1,1,1,0},{1,0,0,0},{0,0,0,0}},
 {{1,1,0,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}}},

{{{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
 {{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}},
 {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
 {{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}}},

{{{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
 {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
 {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
 {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}}}
};

// ---------------- GAME FUNCTIONS ----------------

void clearBoard(){
for(int y=0;y<HEIGHT;y++)
for(int x=0;x<WIDTH;x++)
board[y][x]=0;
}

void spawnPiece(){
currentPiece=random(0,7);
rotationState=0;
pieceX=WIDTH/2-2;
pieceY=0;
}

bool collision(int newX,int newY,int newRot){
for(int y=0;y<4;y++)
for(int x=0;x<4;x++)
if(pieces[currentPiece][newRot][y][x]){
int bx=newX+x;
int by=newY+y;
if(bx<0||bx>=WIDTH||by>=HEIGHT) return true;
if(by>=0 && board[by][bx]) return true;
}
return false;
}

void lockPiece(){
for(int y=0;y<4;y++)
for(int x=0;x<4;x++)
if(pieces[currentPiece][rotationState][y][x])
board[pieceY+y][pieceX+x]=1;
}

void clearLines(){
for(int y=0;y<HEIGHT;y++){

bool full=true;

for(int x=0;x<WIDTH;x++){
if(!board[y][x]) full=false;
}

if(full){

// add score
score += 100;

lcd.setCursor(7,1);
lcd.print("     "); // clear old digits
lcd.setCursor(7,1);
lcd.print(score);

// move lines down
for(int yy=y;yy>0;yy--){
for(int x=0;x<WIDTH;x++){
board[yy][x]=board[yy-1][x];
}
}

for(int x=0;x<WIDTH;x++)
board[0][x]=0;
}
}
}

void draw(){
for(int y=0;y<HEIGHT;y++)
for(int x=0;x<WIDTH;x++)
frame[y][x]=board[y][x];

for(int y=0;y<4;y++)
for(int x=0;x<4;x++)
if(pieces[currentPiece][rotationState][y][x])
frame[pieceY+y][pieceX+x]=1;

matrix.renderBitmap(frame,HEIGHT,WIDTH);
}

// ----------- JOYSTICK MOVEMENT -----------

// ----------- JOYSTICK MOVEMENT -----------
void readJoystick() {
  static bool buttonHeld = false;

  int xVal = analogRead(JOY_X);
  int yVal = analogRead(JOY_Y);
  int buttonState = digitalRead(JOY_SW);

  // --- LEFT / RIGHT MOVEMENT ---
  if (xVal < 300 && !collision(pieceX - 1, pieceY, rotationState)) {
    pieceX--;
    delay(150);
  }

  if (xVal > 700 && !collision(pieceX + 1, pieceY, rotationState)) {
    pieceX++;
    delay(150);
  }

  // --- ROTATION WITH BUTTON PRESS ---
  if (buttonState == LOW && !buttonHeld) {
    int newRot = (rotationState + 1) % 4;
    if (!collision(pieceX, pieceY, newRot)) {
      rotationState = newRot;
    }
    buttonHeld = true; // lock until button released
  }

  if (buttonState == HIGH) {
    buttonHeld = false;
  }
}

// ---------------- MUSIC ----------------

void playMusic(){
if(millis()-lastNoteTime>=song[currentNote].dur){

tone(SPK1,song[currentNote].m);
tone(SPK2,song[currentNote].h);
tone(SPK3,song[currentNote].b);

lastNoteTime=millis();
currentNote++;

if(currentNote>=songLength)
currentNote=0;
}
}
void updateScoreLEDs(){

if(score >= 100) digitalWrite(LED100, HIGH);
else digitalWrite(LED100, LOW);

if(score >= 200) digitalWrite(LED200, HIGH);
else digitalWrite(LED200, LOW);

if(score >= 300) digitalWrite(LED300, HIGH);
else digitalWrite(LED300, LOW);

if(score >= 400) digitalWrite(LED400, HIGH);
else digitalWrite(LED400, LOW);

if(score >= 500) digitalWrite(LED500, HIGH);
else digitalWrite(LED500, LOW);

if(score >= 600) digitalWrite(LED600, HIGH);
else digitalWrite(LED600, LOW);

}
// ---------------- SETUP ----------------

void setup(){

matrix.begin();

pinMode(JOY_SW,INPUT_PULLUP);

pinMode(SPK1,OUTPUT);
pinMode(SPK2,OUTPUT);
pinMode(SPK3,OUTPUT);

randomSeed(analogRead(0));

clearBoard();
spawnPiece();
lcd.init();
lcd.backlight();

lcd.setCursor(0,0);
lcd.print("TETRIS");

lcd.setCursor(0,1);
lcd.print("Score: 0");

pinMode(LED100, OUTPUT);
pinMode(LED200, OUTPUT);
pinMode(LED300, OUTPUT);
pinMode(LED400, OUTPUT);
pinMode(LED500, OUTPUT);
pinMode(LED600, OUTPUT);

}

// ---------------- LOOP ----------------

void loop(){

readJoystick();

playMusic();

updateScoreLEDs();

if(millis()-lastDrop>dropInterval){

if(!collision(pieceX,pieceY+1,rotationState)){
pieceY++;
}else{
lockPiece();
clearLines();
spawnPiece();

if(collision(pieceX,pieceY,rotationState)){
clearBoard();
score = 0;

lcd.setCursor(7,1);
lcd.print("0    ");
}
}

lastDrop=millis();
}

draw();
}