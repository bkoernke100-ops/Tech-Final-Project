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

int pieceX = 4;
int pieceY = 0;

unsigned long lastDrop = 0;
unsigned long dropInterval = 500;

void clearBoard() {
  for (int y = 0; y < HEIGHT; y++)
    for (int x = 0; x < WIDTH; x++)
      board[y][x] = 0;
}

void spawnPiece() {
  pieceX = random(0, WIDTH - 2);
  pieceY = 0;
}

bool collision(int newX, int newY) {
  if (newX < 0 || newX > WIDTH - 2) return true;
  if (newY > HEIGHT - 2) return true;

  if (board[newY][newX] ||
      board[newY][newX+1] ||
      board[newY+1][newX] ||
      board[newY+1][newX+1])
      return true;

  return false;
}

void lockPiece() {
  board[pieceY][pieceX] = 1;
  board[pieceY][pieceX+1] = 1;
  board[pieceY+1][pieceX] = 1;
  board[pieceY+1][pieceX+1] = 1;
}

void draw() {
  // Copy board to frame
  for (int y = 0; y < HEIGHT; y++)
    for (int x = 0; x < WIDTH; x++)
      frame[y][x] = board[y][x];

  // Draw falling 2x2 block
  frame[pieceY][pieceX] = 1;
  frame[pieceY][pieceX+1] = 1;
  frame[pieceY+1][pieceX] = 1;
  frame[pieceY+1][pieceX+1] = 1;

  matrix.renderBitmap(frame, HEIGHT, WIDTH);
}

void readJoystick() {
  int xVal = analogRead(JOY_X);
  int yVal = analogRead(JOY_Y);

  if (xVal < 300 && !collision(pieceX - 1, pieceY)) {
    pieceX--;
    delay(150);
  }

  if (xVal > 700 && !collision(pieceX + 1, pieceY)) {
    pieceX++;
    delay(150);
  }

  if (yVal > 700 && !collision(pieceX, pieceY + 1)) {
    pieceY++;
    delay(100);
  }
}

void setup() {
  matrix.begin();
  pinMode(JOY_SW, INPUT_PULLUP);
  randomSeed(analogRead(0));
  clearBoard();
  spawnPiece();
}

void loop() {

  readJoystick();

  if (millis() - lastDrop > dropInterval) {

    if (!collision(pieceX, pieceY + 1)) {
      pieceY++;
    } else {
      lockPiece();
      spawnPiece();

      if (collision(pieceX, pieceY)) {
        clearBoard(); // Game over
      }
    }

    lastDrop = millis();
  }

  draw();
}