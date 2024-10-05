#include <iostream>
#include <raylib.h>
#include <deque>
#include <raymath.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
    #include <emscripten/html5.h>
#endif

using namespace std;

// Game constants
static bool allowMove = false;
Color green = {173, 204, 96, 255};
Color darkGreen = {43, 51, 24, 255};
int cellSize = 30;
int cellCount = 25;
int offset = 75;
double lastUpdateTime = 0;

// Function declarations
void UpdateDrawFrame(void);
bool ElementInDeque(Vector2 element, deque<Vector2> deque);
bool EventTriggered(double interval);

// Helper function to check if an element exists in a deque
bool ElementInDeque(Vector2 element, deque<Vector2> deque) {
    for (unsigned int i = 0; i < deque.size(); i++) {
        if (Vector2Equals(deque[i], element)) {
            return true;
        }
    }
    return false;
}

// Helper function to trigger events at specified intervals
bool EventTriggered(double interval) {
    double currentTime = GetTime();
    if (currentTime - lastUpdateTime >= interval) {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

// Snake class definition
class Snake {
public:
    deque<Vector2> body = {Vector2{6,9}, Vector2{5,9}, Vector2{4, 9}};
    Vector2 direction = {1, 0};
    bool addSegment = false;

    // Draw the snake on the screen
    void Draw() {
        for (unsigned int i = 0; i < body.size(); i++) {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{offset + x * cellSize, offset + y * cellSize, (float)cellSize, (float)cellSize};
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    // Update the snake's position
    void Update() {
        body.push_front(Vector2Add(body[0], direction));
        if (addSegment == true) {
            addSegment = false;
        }
        else {
            body.pop_back();
        }
    }

    // Reset the snake to its initial state
    void Reset() {
        body = {Vector2{6,9}, Vector2{5,9}, Vector2{4,9}};
        direction = {1,0};
    }
};

// Food class definition
class Food {
public:
    Vector2 position;
    Texture2D texture;
    bool textureLoaded;

    // Constructor: Initialize food position and load texture
    Food(deque<Vector2> snakeBody) {
        textureLoaded = false;
        position = GenerateRandomPos(snakeBody);

        Image image = LoadImage("resources/Graphics/food.png");
        if (image.data == NULL) {
            cout << "Failed to load food image" << endl;
        } else {
            texture = LoadTextureFromImage(image);
            UnloadImage(image);
            if (texture.id == 0) {
                cout << "Failed to create texture from food image" << endl;
            } else {
                textureLoaded = true;
                cout << "Food texture loaded successfully. ID: " << texture.id << ", Width: " << texture.width << ", Height: " << texture.height << endl;
            }
        }
    }

    // Destructor: Unload texture
    ~Food() {
        if (textureLoaded) {
            UnloadTexture(texture);
        }
    }

    // Draw the food on the screen
    void Draw() {
        if (textureLoaded) {
            DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
        } else {
            // Fallback: draw a red rectangle if texture failed to load
            DrawRectangle(offset + position.x * cellSize, offset + position.y * cellSize, cellSize, cellSize, RED);
        }
        cout << "Drawing food at " << position.x << ", " << position.y << endl;
    }

    // Generate a random cell position
    Vector2 GenerateRandomCell() {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{x,y};
    }

    // Generate a random position for food that doesn't overlap with the snake
    Vector2 GenerateRandomPos(deque<Vector2> snakeBody) {
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody)) {
            position = GenerateRandomCell();
        }
        return position;
    }
};

// Game class definition
class Game {
public:
    Snake snake = Snake();
    Food food = Food(snake.body);
    bool running = true;
    int score = 0;
    Sound eatSound;
    Sound wallSound;

    // Constructor: Initialize audio
    Game() {
        InitAudioDevice();
        eatSound = LoadSound("resources/Sounds/eat.wav");
        wallSound = LoadSound("resources/Sounds/wall.wav");
    }

    // Destructor: Unload audio resources
    ~Game() {
        UnloadSound(eatSound);
        UnloadSound(wallSound);
        CloseAudioDevice();
    }

    // Draw game elements
    void Draw() {
        food.Draw();
        snake.Draw();
    }

    // Update game state
    void Update() {
        if (running) {
            snake.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
    }

    // Check if snake has eaten the food
    void CheckCollisionWithFood() {
        if (Vector2Equals(snake.body[0], food.position)) {
            food.position = food.GenerateRandomPos(snake.body);
            snake.addSegment = true;
            score++;
            PlaySound(eatSound);
        }
    }

    // Check if snake has hit the edges
    void CheckCollisionWithEdges() {
        if (snake.body[0].x == cellCount || snake.body[0].x == -1) {
            GameOver();
        }
        if(snake.body[0].y == cellCount || snake.body[0].y == -1) {
            GameOver();
        }
    }

    // Handle game over state
    void GameOver() {
        snake.Reset();
        food.position = food.GenerateRandomPos(snake.body);
        running = false;
        score = 0;
        PlaySound(wallSound);
    }

    // Check if snake has collided with its own tail
    void CheckCollisionWithTail() {
        deque<Vector2> headlessBody = snake.body;
        headlessBody.pop_front();
        if (ElementInDeque(snake.body[0], headlessBody)) {
            GameOver();
        }
    }
};

Game* game = nullptr;

int main() {
    cout << "Starting the game ..." << endl;
    InitWindow(2*offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Retro Snake");

    // Initialize game
    game = new Game();

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);

    while (WindowShouldClose() == false) {
        UpdateDrawFrame();
    }
#endif

    // Clean up
    delete game;

    CloseWindow();
    return 0;
}

// Main game loop function
void UpdateDrawFrame(void) {
    // Update game state
    if (EventTriggered(0.2)) {
        allowMove = true;
        game->Update();
    }

    // Handle input
    if (IsKeyPressed(KEY_UP) && game->snake.direction.y != 1 && allowMove) {
        game->snake.direction = {0, -1};
        game->running = true;
        allowMove = false;
    }
    if (IsKeyPressed(KEY_DOWN) && game->snake.direction.y != -1 && allowMove) {
        game->snake.direction = {0 ,1};
        game->running = true;
        allowMove = false;
    }
    if (IsKeyPressed(KEY_LEFT) && game->snake.direction.x != 1 && allowMove) {
        game->snake.direction = {-1, 0};
        game->running = true;
        allowMove = false;
    }
    if (IsKeyPressed(KEY_RIGHT) && game->snake.direction.x != -1 && allowMove) {
        game->snake.direction = {1, 0};
        game->running = true;
        allowMove = false;
    }

    // Drawing
    BeginDrawing();

    ClearBackground(green);
    DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)cellSize * cellCount + 10, (float)cellSize * cellCount + 10}, 5, darkGreen);
    DrawText("Retro Snake", offset - 5, 20 , 40 , darkGreen);
    DrawText(TextFormat("%i", game->score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);
    game->Draw();

    EndDrawing();
}
