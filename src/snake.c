#include <raylib.h>
#include <stdlib.h>
#include <stdio.h>

// #define SQUARE_SIZE 20
#define GRID_SIZE 20

#define SPEED 14

enum {UP, DOWN, LEFT, RIGHT}; // Directions
enum {PLAYING, GAMEOVER, START}; // Game screens

static const int screenWidth = 800;
static const int screenHeight = 450;

static int sW;
static int sH;

int getRelativePos(int n, char axis);
Vector2 midPoint(Vector2 v1, Vector2 v2);

int main(void)
{
  sW = screenWidth;
  sH = screenHeight;
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);    // Window configuration flags
  InitWindow(screenWidth, screenHeight, "Snake");
  SetTargetFPS(120);
    
 /* Grass is 0
  * Snake head is equal to the score
  * Anything between grass and head is body
  * Apple is -1
  */

  int grid[GRID_SIZE][GRID_SIZE];
  /* seed the random number generator using garbage data from initializing the grid */
  int seed = 420;
  for (int i = 0; i < GRID_SIZE; ++i)
    for (int j = 0; j < GRID_SIZE; ++j)
      if (grid[i][j] != 0)
      {
        seed += grid[i][j];
        grid[i][j] = 0;
      }
  srand(seed);
  // Place an apple in the grid
  grid[rand() % GRID_SIZE][rand() % GRID_SIZE] = -1;
  int score = 4; // Is 4 greater than actual score
  int direction = UP;
  int nextDir = UP;
  int headx = GRID_SIZE / 2;
  int heady = GRID_SIZE / 2;

  int frameSinceMove = 0;
  int gameScreen = START;
  int textScale = screenHeight / 100 * 5;
  
  Camera2D camera = { 0 };
  camera.target = (Vector2){ 0.f, 0.f };
  camera.offset = (Vector2){ 0.f, 0.f };
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  while(!WindowShouldClose())
  {
    // Fullscreen toggle
    if (IsKeyPressed(KEY_ENTER) && (IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)))
    {
      int display = GetCurrentMonitor();
      if (IsWindowFullscreen())
      {
        sW = screenWidth;
        sH = screenHeight;
        SetWindowSize(sW, sH);
        ToggleFullscreen();
      } else {
        sW = GetMonitorWidth(display);
        sH = GetMonitorHeight(display);
        ToggleFullscreen();
        SetWindowSize(sW, sH);
      }
    }
    // Change Snake Direction
    if (IsKeyDown(KEY_UP) && direction != DOWN)
      nextDir = UP;
    if (IsKeyDown(KEY_DOWN) && direction != UP)
      nextDir = DOWN;
    if (IsKeyDown(KEY_LEFT) && direction != RIGHT)
      nextDir = LEFT;
    if (IsKeyDown(KEY_RIGHT) && direction != LEFT)
      nextDir = RIGHT;

    // Update game
    if (frameSinceMove++ >= SPEED && gameScreen == PLAYING)
    {
      frameSinceMove = 0;
      direction = nextDir;
      switch (direction)
      {
        case UP:
          heady--;
          break;
        case DOWN:
          heady++;
          break;
        case LEFT:
          headx--;
          break;
        case RIGHT:
          headx++;
          break;
      }
      // Check if game is lost
      if (heady < 0 || heady >= GRID_SIZE || headx < 0 || headx >= GRID_SIZE)
        gameScreen = GAMEOVER;
      if (grid[headx][heady] > 0)
        gameScreen = GAMEOVER;
      // Check if snake is eating Apple
      if (grid[headx][heady] == -1)
      {
        score++;
        int randx = rand() % GRID_SIZE;
        int randy = rand() % GRID_SIZE;
        while (grid[randx][randy])
        {
          randx = rand() % GRID_SIZE;
          randy = rand() % GRID_SIZE;
        }
        grid[randx][randy] = -1;
      }
      // Move snake
      grid[headx][heady] = score;
      // printf("%d %d\n", headx, heady);
    }

    if (IsWindowFullscreen())
    {
      int display = GetCurrentMonitor();
      sW = GetMonitorWidth(display);
      sH = GetMonitorHeight(display);
    } else {
      sW = GetScreenWidth();
      sH = GetScreenHeight();
    }
    // printf("%d %d\n", sW, sH);

    Color tileCol;
    int tileSize = sH / GRID_SIZE;

    BeginDrawing();
    // Draw the direction that the snake is going to move in
    // Variable for scaling ui stuff
    textScale = sH / 100 * 5;
    Vector2 directionArrowCorners[4] = {
      (Vector2) {10, textScale + 20},
      (Vector2) {textScale + 10, textScale + 20},
      (Vector2) {10, 2 * textScale + 20},
      (Vector2) {textScale + 10, 2 * textScale + 20}
    };
    switch (nextDir)
    {
      case UP:
        DrawTriangle(
          directionArrowCorners[2],
          directionArrowCorners[3],
          midPoint(directionArrowCorners[0], directionArrowCorners[1]),
          RED
        );
        break;
      case DOWN:
        DrawTriangle(
          directionArrowCorners[0],
          midPoint(directionArrowCorners[2], directionArrowCorners[3]),
          directionArrowCorners[1],
          RED
        );
        break;
       case LEFT:
        // printf("%f %f ", directionArrowCorners[1].x, directionArrowCorners[1].y);
        // printf("%f %f ", directionArrowCorners[3].x, directionArrowCorners[3].y);
        // printf("%f %f\n", midPoint(directionArrowCorners[0], directionArrowCorners[2]).x, midPoint(directionArrowCorners[0], directionArrowCorners[2]).y);
        DrawTriangle(
          directionArrowCorners[1],
          midPoint(directionArrowCorners[0], directionArrowCorners[2]),
          directionArrowCorners[3],
          RED
        );
        break;
       case RIGHT:
        DrawTriangle(
          directionArrowCorners[0],
          directionArrowCorners[2],
          midPoint(directionArrowCorners[1], directionArrowCorners[3]),
          RED
        );
        break;
    }
    //----------------------------------------
    ClearBackground(RAYWHITE);
    BeginMode2D(camera);
    // DrawRectangle(-50, -50, sW+50, sH+50, GRAY);
    for (int x = 0; x < GRID_SIZE; ++x)
      for (int y = 0; y < GRID_SIZE; ++y)
      {
        // Figure out what color the tile should be
        // Decrease snake body lifetime
        if (grid[x][y] == -1)
        {
          tileCol = RED;
        }
        else if (grid[x][y] == 0)
        {
          if ((x + y) % 2)
            tileCol = GREEN;
          else
            tileCol = LIME;
        }
        else if (grid[x][y] == score)
        {
          tileCol = BLUE;
          if (frameSinceMove == SPEED-1)
            --grid[x][y];
        }
        else if (grid[x][y] > 0 && grid[x][y] < score)
        {
          tileCol = SKYBLUE;
          if (frameSinceMove == SPEED-1)
            --grid[x][y];
        }
        else
        {
          tileCol = GRAY;
        }

        DrawRectangle(getRelativePos(x, 'x'), getRelativePos(y, 'y'), tileSize, tileSize, tileCol);
        // DrawText(TextFormat("%d", grid[x][y]), getRelativePos(x, 'x'), getRelativePos(y, 'y'), 10, RED);
      }
    // DrawRectangleLines(-100, -100, 200, 200, RED);
    // DrawRectangleLines(-100, 100, 200, 200, BLUE);
    EndMode2D();
    DrawText(TextFormat("Score: %d", score - 4), 10, 10, textScale, RED);
    DrawFPS(10, sH - 10 - textScale);
    // Draw menu for screens
    if (gameScreen != PLAYING)
    {
      Rectangle menu = {
          sW * 0.25,
          sH * 0.25,
          sW * 0.5,
        sH * 0.5
      };

      DrawRectangleRoundedLines(menu, textScale / 200.f, 0, textScale / 2.f, (Color){50, 50, 250, 200});
      DrawRectangleRounded(menu, textScale / 200.f, 0, (Color){245, 245, 245, 200});

      if (gameScreen == START)
      {
        int textWidth = MeasureText("PRESS ENTER TO START", textScale);
        DrawText("PRESS ENTER TO START", (sW - textWidth) / 2, (sH - textScale) / 2, textScale, RED);
        if (IsKeyDown(KEY_ENTER)) gameScreen = PLAYING;
      } else {
        int textWidth = MeasureText("PRESS ENTER TO START", textScale);
        DrawText("PRESS ENTER TO START", (sW - textWidth) / 2, sH / 2 - textScale - 5, textScale, RED);
        DrawText(TextFormat("Final Score: %d", score - 4), (sW - textWidth) / 2, sH / 2 + 5, textScale, RED);
        if (IsKeyDown(KEY_ENTER))
        {
          for (int i = 0; i < GRID_SIZE; ++i)
            for (int j = 0; j < GRID_SIZE; ++j)
              grid[i][j] = 0;
          grid[rand() % GRID_SIZE][rand() % GRID_SIZE] = -1;
          score = 4;
          direction = UP;
          nextDir = UP;
          headx = GRID_SIZE / 2;
          heady = GRID_SIZE / 2;
          gameScreen = PLAYING;
        }
      }
    }
    //----------------------------------------
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

int getRelativePos(int n, char axis)
{
  Vector2 gridOffset = {
    ( sW % (sH / GRID_SIZE) ) / 2.f,
    ( sH % (sH / GRID_SIZE) ) / 2.f
  };
  if (axis == 'x')
    return sH / GRID_SIZE * n + (sW - sH) / 2 + (int) gridOffset.x;
  else if (axis == 'y')
    return sH / GRID_SIZE * n + (int) gridOffset.y;
  return 0;
}

Vector2 midPoint(Vector2 v1, Vector2 v2)
{
  return (Vector2){(v1.x + v2.x) / 2, (v1.y + v2.y) / 2};
}
