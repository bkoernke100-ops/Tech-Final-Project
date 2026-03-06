/*
Dev: Liam Treat, Brennen Koernke
Resources: Chat Gpt
*/

#include <Arduino_LED_Matrix.h>

ArduinoLEDMatrix matrix;

#define WIDTH 12
#define HEIGHT 8

#define JOY_X A0
#define JOY_Y A1
#define JOY_SW A2

uint8_t board[HEIGHT][WIDTH];
uint8_t frame[HEIGHT][WIDTH];

int pieceX, pieceY;
int currentPiece;
int rotationState = 0;

unsigned long lastDrop = 0;
unsigned long dropInterval = 2000;

// 7 Tetris pieces (4 rotations each)
const uint8_t pieces[7][4][4][4] = {

  // I
  {
    {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}},
    {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
    {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}},
    {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}}
  },

  // O
  {
    {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
    {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
    {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
    {{0,1,1,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}
  },

  // T
  {
    {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
    {{0,1,0,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
    {{0,0,0,0},{1,1,1,0},{0,1,0,0},{0,0,0,0}},
    {{0,1,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}
  },

  // L
  {
    {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
    {{0,1,1,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}},
    {{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,0,0}},
    {{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}}
  },

  // J
  {
    {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
    {{0,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,0,0}},
    {{0,0,0,0},{1,1,1,0},{1,0,0,0},{0,0,0,0}},
    {{1,1,0,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}}
  },

  // S
  {
    {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
    {{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}},
    {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}},
    {{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}}
  },

  // Z
  {
    {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
    {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
    {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}},
    {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}}
  }
};

void clearBoard() {
  for(int y=0;y<HEIGHT;y++)
    for(int x=0;x<WIDTH;x++)
      board[y][x]=0;
}

void spawnPiece() {
  currentPiece = random(0,7);
  rotationState = 0;
  pieceX = WIDTH/2 - 2;
  pieceY = 0;
}

bool collision(int newX,int newY,int newRot) {
  for(int y=0;y<4;y++){
    for(int x=0;x<4;x++){
      if(pieces[currentPiece][newRot][y][x]){
        int bx=newX+x;
        int by=newY+y;

        if(bx<0||bx>=WIDTH||by>=HEIGHT) return true;
        if(by>=0 && board[by][bx]) return true;
      }
    }
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
    for(int x=0;x<WIDTH;x++)
      if(!board[y][x]) full=false;

    if(full){
      for(int yy=y;yy>0;yy--)
        for(int x=0;x<WIDTH;x++)
          board[yy][x]=board[yy-1][x];

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

void setup(){
  matrix.begin();
  pinMode(JOY_SW,INPUT_PULLUP);
  randomSeed(analogRead(0));
  clearBoard();
  spawnPiece();
}

void loop(){

  readJoystick();

  if(millis()-lastDrop>dropInterval){

    if(!collision(pieceX,pieceY+1,rotationState)){
      pieceY++;
    } else {
      lockPiece();
      clearLines();
      spawnPiece();

      if(collision(pieceX,pieceY,rotationState))
        clearBoard();
    }

    lastDrop=millis();
  }

  draw();
}