/*
Dev: Liam Treat, Brennen Koernke
Resources: Chat Gpt
*/

#include <LedControl.h>

#define DIN 11
#define CLK 13
#define CS 10

LedControl lc = LedControl(DIN, CLK, CS, 1);

// Joystick pins
#define JOY_X A0
#define JOY_Y A1
#define JOY_SW A2   // Button now on analog pin A2

#define WIDTH 8
#define HEIGHT 8

byte board[HEIGHT][WIDTH];

unsigned long lastDrop = 0;
unsigned long dropInterval = 600;

int pieceX, pieceY;
int currentPiece;
byte rotationState;

const byte pieces[7][4][4] = {
  {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}, // I
  {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, // O
  {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // T
  {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // L
  {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // J
  {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, // S
  {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}  // Z
};

void clearBoard() {
  for(int y=0;y<HEIGHT;y++)
    for(int x=0;x<WIDTH;x++)
      board[y][x]=0;
}

void spawnPiece() {
  currentPiece = random(0,7);
  rotationState = 0;
  pieceX = 2;
  pieceY = 0;
}

bool collision(int newX, int newY) {
  for(int y=0;y<4;y++){
    for(int x=0;x<4;x++){
      if(pieces[currentPiece][y][x]){
        int bx = newX + x;
        int by = newY + y;
        if(bx<0 || bx>=WIDTH || by>=HEIGHT) return true;
        if(by>=0 && board[by][bx]) return true;
      }
    }
  }
  return false;
}

void lockPiece() {
  for(int y=0;y<4;y++){
    for(int x=0;x<4;x++){
      if(pieces[currentPiece][y][x]){
        board[pieceY+y][pieceX+x]=1;
      }
    }
  }
}

void clearLines() {
  for(int y=0;y<HEIGHT;y++){
    bool full=true;
    for(int x=0;x<WIDTH;x++)
      if(board[y][x]==0) full=false;
    if(full){
      for(int yy=y;yy>0;yy--)
        for(int x=0;x<WIDTH;x++)
          board[yy][x]=board[yy-1][x];
      for(int x=0;x<WIDTH;x++)
        board[0][x]=0;
    }
  }
}

void draw() {
  lc.clearDisplay(0);

  for(int y=0;y<HEIGHT;y++)
    for(int x=0;x<WIDTH;x++)
      if(board[y][x])
        lc.setLed(0,y,x,true);

  for(int y=0;y<4;y++)
    for(int x=0;x<4;x++)
      if(pieces[currentPiece][y][x])
        lc.setLed(0,pieceY+y,pieceX+x,true);
}

void rotatePiece() {
  byte temp[4][4];
  for(int y=0;y<4;y++)
    for(int x=0;x<4;x++)
      temp[x][3-y]=pieces[currentPiece][y][x];

  byte backup[4][4];
  memcpy(backup,pieces[currentPiece],16);
  memcpy((void*)pieces[currentPiece],temp,16);

  if(collision(pieceX,pieceY))
    memcpy((void*)pieces[currentPiece],backup,16);
}

void readJoystick() {
  int xVal = analogRead(JOY_X);
  int yVal = analogRead(JOY_Y);

  if(xVal < 300 && !collision(pieceX-1,pieceY)){
    pieceX--;
    delay(150);
  }
  if(xVal > 700 && !collision(pieceX+1,pieceY)){
    pieceX++;
    delay(150);
  }
  if(yVal > 700 && !collision(pieceX,pieceY+1)){
    pieceY++;
    delay(100);
  }

  if(digitalRead(JOY_SW)==LOW){
    rotatePiece();
    delay(200);
  }
}

void setup() {
  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);

  pinMode(JOY_SW, INPUT_PULLUP);

  randomSeed(analogRead(0));

  clearBoard();
  spawnPiece();
}

void loop() {
  readJoystick();

  if(millis()-lastDrop>dropInterval){
    if(!collision(pieceX,pieceY+1)){
      pieceY++;
    } else {
      lockPiece();
      clearLines();
      spawnPiece();
      if(collision(pieceX,pieceY)){
        clearBoard();
      }
    }
    lastDrop=millis();
  }

  draw();
}