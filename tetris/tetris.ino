/*
Dev: Liam Treat, Brennen Koernke
Resources: Chat Gpt
*/

// Multiplexed 12x8 LED Matrix Tetris

#define WIDTH 8
#define HEIGHT 12

// Columns D2-D9
const byte colPins[WIDTH] = {2,3,4,5,6,7,8,9};

// Rows D10-D13 + A3-A5 (total 12 rows)
const byte rowPins[HEIGHT] = {10,11,12,13,A3,A4,A5, -1, -1, -1, -1, -1}; // unused rows as -1

// Joystick
#define JOY_X A0
#define JOY_Y A1
#define JOY_SW A2

byte board[HEIGHT][WIDTH];

unsigned long lastDrop = 0;
unsigned long dropInterval = 600;

int pieceX, pieceY;
byte currentPiece;
byte rotationState;

// 7 Tetris pieces
const byte pieces[7][4][4] = {
  {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}, // I
  {{1,1,0,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, // O
  {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // T
  {{1,0,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // L
  {{0,0,1,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}}, // J
  {{0,1,1,0},{1,1,0,0},{0,0,0,0},{0,0,0,0}}, // S
  {{1,1,0,0},{0,1,1,0},{0,0,0,0},{0,0,0,0}}  // Z
};

// ---------- Setup ----------
void setup() {
  for(byte i=0;i<WIDTH;i++) pinMode(colPins[i], OUTPUT);
  for(byte i=0;i<HEIGHT;i++) if(rowPins[i]!=-1) pinMode(rowPins[i], OUTPUT);
  
  pinMode(JOY_SW, INPUT_PULLUP);
  randomSeed(analogRead(0));
  
  clearBoard();
  spawnPiece();
}

// ---------- Game Logic ----------
void clearBoard() {
  for(int y=0;y<HEIGHT;y++)
    for(int x=0;x<WIDTH;x++)
      board[y][x]=0;
}

void spawnPiece() {
  currentPiece=random(0,7);
  rotationState=0;
  pieceX=2;
  pieceY=0;
}

bool collision(int nx,int ny,int rot){
  for(int y=0;y<4;y++){
    for(int x=0;x<4;x++){
      int rx=x,ry=y;
      for(int r=0;r<rot;r++){
        int tmp=rx; rx=3-ry; ry=tmp;
      }
      if(pieces[currentPiece][ry][rx]){
        int bx=nx+x;
        int by=ny+y;
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
      int rx=x, ry=y;
      for(int r=0;r<rotationState;r++){
        int tmp=rx; rx=3-ry; ry=tmp;
      }
      if(pieces[currentPiece][ry][rx]){
        int px=pieceX+x;
        int py=pieceY+y;
        if(py>=0 && py<HEIGHT && px>=0 && px<WIDTH)
          board[py][px]=1;
      }
    }
  }
}

void clearLines(){
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

// ---------- Draw Function ----------
void draw(){
  // Multiplexing
  for(byte r=0;r<HEIGHT;r++){
    if(rowPins[r]==-1) continue;
    digitalWrite(rowPins[r], LOW); // turn row on (common cathode)
    for(byte c=0;c<WIDTH;c++){
      digitalWrite(colPins[c], !board[r][c]); // LOW to turn on LED
    }
    delayMicroseconds(1000); // row display time
    digitalWrite(rowPins[r], HIGH); // turn row off
  }
}

// ---------- Joystick Input ----------
void readJoystick(){
  int xVal=analogRead(JOY_X);
  int yVal=analogRead(JOY_Y);
  
  if(xVal<300 && !collision(pieceX-1,pieceY,rotationState)){ pieceX--; delay(150);}
  if(xVal>700 && !collision(pieceX+1,pieceY,rotationState)){ pieceX++; delay(150);}
  if(yVal>700 && !collision(pieceX,pieceY+1,rotationState)){ pieceY++; delay(100);}
  
  if(analogRead(JOY_SW)<100){
    int newRot=(rotationState+1)%4;
    if(!collision(pieceX,pieceY,newRot)) rotationState=newRot;
    delay(200);
  }
}

// ---------- Main Loop ----------
void loop(){
  readJoystick();
  
  if(millis()-lastDrop>dropInterval){
    if(!collision(pieceX,pieceY+1,rotationState)) pieceY++;
    else{
      lockPiece();
      clearLines();
      spawnPiece();
      if(collision(pieceX,pieceY,rotationState)) clearBoard(); // Game Over
    }
    lastDrop=millis();
  }
  
  draw();
}