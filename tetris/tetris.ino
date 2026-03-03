/*
Dev: Liam Treat, Brennen Koernke
Resources: Chat Gpt
*/

// Multiplexed 12x8 LED Matrix Tetris

#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

#define WIDTH 8
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
unsigned long dropInterval = 2000; // slower fall

// 4 pieces: I, Z, T, L
const uint8_t pieces[4][4][4] = {
  { // I
    {0,1,0,0},
    {0,1,0,0},
    {0,1,0,0},
    {0,1,0,0}
  },
  { // Z
    {1,1,0,0},
    {0,1,1,0},
    {0,0,0,0},
    {0,0,0,0}
  },
  { // T
    {0,1,0,0},
    {1,1,1,0},
    {0,0,0,0},
    {0,0,0,0}
  },
  { // L
    {1,0,0,0},
    {1,0,0,0},
    {1,1,0,0},
    {0,0,0,0}
  }
};

// -------------------- Functions --------------------

void clearBoard(){
  for(int y=0;y<HEIGHT;y++)
    for(int x=0;x<WIDTH;x++)
      board[y][x]=0;
}

void spawnPiece(){
  currentPiece = random(0,4);
  pieceX = 2;
  pieceY = 0;
  rotationState = 0; // reset rotation
}

bool collision(int nx,int ny,int rot){
  for(int y=0;y<4;y++){
    for(int x=0;x<4;x++){
      int rx = x, ry = y;
      for(int r=0;r<rot;r++){
        int tmp = rx;
        rx = 3 - ry;
        ry = tmp;
      }
      if(pieces[currentPiece][ry][rx]){
        int bx = nx + x;
        int by = ny + y;
        if(bx<0 || bx>=WIDTH || by>=HEIGHT) return true;
        if(by>=0 && board[by][bx]) return true;
      }
    }
  }
  return false;
}

void lockPiece(){
  for(int y=0;y<4;y++){
    for(int x=0;x<4;x++){
      int rx = x, ry = y;
      for(int r=0;r<rotationState;r++){
        int tmp = rx;
        rx = 3 - ry;
        ry = tmp;
      }
      if(pieces[currentPiece][ry][rx]){
        int bx = pieceX + x;
        int by = pieceY + y;
        if(by>=0 && bx>=0 && bx<WIDTH)
          board[by][bx] = 1;
      }
    }
  }
}

void clearLines(){
  for(int y=0;y<HEIGHT;y++){
    bool full = true;
    for(int x=0;x<WIDTH;x++)
      if(board[y][x] == 0) full = false;
    if(full){
      for(int yy=y; yy>0; yy--)
        for(int x=0;x<WIDTH;x++)
          board[yy][x] = board[yy-1][x];
      for(int x=0;x<WIDTH;x++)
        board[0][x] = 0;
    }
  }
}

void updateFrame(){
  // copy board
  for(int y=0;y<HEIGHT;y++)
    for(int x=0;x<WIDTH;x++)
      frame[y][x] = board[y][x];

  // overlay piece with rotation
  for(int y=0;y<4;y++){
    for(int x=0;x<4;x++){
      int rx = x, ry = y;
      for(int r=0;r<rotationState;r++){
        int tmp = rx;
        rx = 3 - ry;
        ry = tmp;
      }
      if(pieces[currentPiece][ry][rx]){
        int bx = pieceX + x;
        int by = pieceY + y;
        if(by>=0 && by<HEIGHT && bx>=0 && bx<WIDTH)
          frame[by][bx] = 1;
      }
    }
  }
}

void readJoystick(){
  int xVal = analogRead(JOY_X);
  int yVal = analogRead(JOY_Y);

  if(xVal < 300 && !collision(pieceX-1,pieceY,rotationState))
    pieceX--;

  if(xVal > 700 && !collision(pieceX+1,pieceY,rotationState))
    pieceX++;

  if(yVal > 700 && !collision(pieceX,pieceY+1,rotationState))
    pieceY++;

  // rotation on button press
  if(analogRead(JOY_SW) < 100){
    int newRot = (rotationState + 1) % 4;
    if(!collision(pieceX,pieceY,newRot))
      rotationState = newRot;
    delay(200); // simple debounce
  }
}

// -------------------- Setup & Loop --------------------

void setup(){
  matrix.begin();
  randomSeed(analogRead(0));
  clearBoard();
  spawnPiece();
  pinMode(JOY_SW, INPUT_PULLUP);
}

void loop(){
  readJoystick();

  if(millis()-lastDrop > dropInterval){
    if(!collision(pieceX,pieceY+1,rotationState)){
      pieceY++;
    } else {
      lockPiece();
      clearLines();
      spawnPiece();
      if(collision(pieceX,pieceY,rotationState))
        clearBoard(); // Game Over
    }
    lastDrop = millis();
  }

  updateFrame();
  matrix.renderBitmap(frame, 8, 8);
}