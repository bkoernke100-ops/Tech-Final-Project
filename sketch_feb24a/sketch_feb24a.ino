/*
Dev: Liam Treat, Brennen Koernke
Resources: Chat Gpt
Project Face Remodel
*/

#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

MCUFRIEND_kbv tft;

LiquidCrystal_I2C lcd(0x27,16,2);

#define BLACK 0x0000
#define WHITE 0xFFFF
#define BLUE  0x001F
#define RED   0xF800

#define WIDTH 12
#define HEIGHT 8
#define BLOCK 20

#define JOY_X A0
#define JOY_Y A1
#define JOY_SW A2

#define SPK1 A3
#define SPK2 A4
#define SPK3 A5

uint8_t board[HEIGHT][WIDTH];

int pieceX,pieceY;
int currentPiece;
int rotationState=0;

unsigned long lastDrop=0;
unsigned long dropInterval=700;

int score=0;

const uint8_t pieces[7][4][4][4] = {
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

void drawBlock(int x,int y,uint16_t color){
  tft.fillRect(x*BLOCK,y*BLOCK,BLOCK-2,BLOCK-2,color);
}

void drawBoard(){

  tft.fillScreen(BLACK);

  for(int y=0;y<HEIGHT;y++)
    for(int x=0;x<WIDTH;x++)
      if(board[y][x])
        drawBlock(x,y,BLUE);

  for(int y=0;y<4;y++)
    for(int x=0;x<4;x++)
      if(pieces[currentPiece][rotationState][y][x])
        drawBlock(pieceX+x,pieceY+y,RED);
}

void spawnPiece(){
  currentPiece=random(7);
  rotationState=0;
  pieceX=WIDTH/2-2;
  pieceY=0;
}

bool collision(int nx,int ny,int nr){

  for(int y=0;y<4;y++)
    for(int x=0;x<4;x++)
      if(pieces[currentPiece][nr][y][x]){

        int bx=nx+x;
        int by=ny+y;

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

void readJoystick(){

  int xVal=analogRead(JOY_X);
  int yVal=analogRead(JOY_Y);

  if(xVal<300 && !collision(pieceX-1,pieceY,rotationState)){
    pieceX--;
    delay(120);
  }

  if(xVal>700 && !collision(pieceX+1,pieceY,rotationState)){
    pieceX++;
    delay(120);
  }
}

void setup(){

  uint16_t ID=tft.readID();
  tft.begin(ID);
  tft.setRotation(1);
  tft.fillScreen(BLACK);

  pinMode(JOY_SW,INPUT_PULLUP);

  randomSeed(analogRead(0));

  spawnPiece();

  lcd.init();
  lcd.backlight();
  lcd.print("TETRIS");
}

void loop(){

  readJoystick();

  if(millis()-lastDrop>dropInterval){

    if(!collision(pieceX,pieceY+1,rotationState)){
      pieceY++;
    }
    else{

      lockPiece();
      spawnPiece();
    }

    lastDrop=millis();
  }

  drawBoard();
}