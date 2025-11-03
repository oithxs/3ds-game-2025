#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#define BOARD_WIDTH 30
#define BOARD_HEIGHT 18
#define MAX_SNAKE_LENGTH 100

typedef struct {
    int x, y;
} Point;

typedef struct {
    Point body[MAX_SNAKE_LENGTH];
    int length;
    int direction; // 0: up, 1: right, 2: down, 3: left
} Snake;

char board[BOARD_HEIGHT][BOARD_WIDTH + 1]; // +1 for null terminator
Snake snake;
Point food;
int score = 0;
int gameRunning = 1;

void initGame() {
    // Initialize board
    memset(board, ' ', sizeof(board));
    
    // Initialize snake
    snake.length = 3;
    snake.direction = 1; // Start moving right
    snake.body[0].x = 5;  snake.body[0].y = 10; // Head
    snake.body[1].x = 4;  snake.body[1].y = 10;
    snake.body[2].x = 3;  snake.body[2].y = 10; // Tail
    
    // Place initial food
    food.x = 15;
    food.y = 10;
    
    score = 0;
    gameRunning = 1;
}

void generateFood() {
    int validPosition = 0;
    while (!validPosition) {
        food.x = rand() % (BOARD_WIDTH - 2) + 1;
        food.y = rand() % (BOARD_HEIGHT - 2) + 1;
        
        validPosition = 1;
        // Check if food spawns on snake
        for (int i = 0; i < snake.length; i++) {
            if (snake.body[i].x == food.x && snake.body[i].y == food.y) {
                validPosition = 0;
                break;
            }
        }
    }
}

void updateSnake() {
    // Calculate new head position
    Point newHead = snake.body[0];
    
    switch (snake.direction) {
        case 0: newHead.y--; break; // Up
        case 1: newHead.x++; break; // Right
        case 2: newHead.y++; break; // Down
        case 3: newHead.x--; break; // Left
    }
    
    // Check wall collision
    if (newHead.x <= 0 || newHead.x >= BOARD_WIDTH - 1 || 
        newHead.y <= 0 || newHead.y >= BOARD_HEIGHT - 1) {
        gameRunning = 0;
        return;
    }
    
    // Check self collision
    for (int i = 0; i < snake.length; i++) {
        if (snake.body[i].x == newHead.x && snake.body[i].y == newHead.y) {
            gameRunning = 0;
            return;
        }
    }
    
    // Move snake body
    for (int i = snake.length - 1; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
    }
    snake.body[0] = newHead;
    
    // Check food collision
    if (newHead.x == food.x && newHead.y == food.y) {
        score += 10;
        if (snake.length < MAX_SNAKE_LENGTH - 1) {
            snake.length++;
        }
        generateFood();
    }
}

void drawGameOver() {
    // Clear console and position cursor
    consoleClear();
    // printf("\x1b[H");   // Move cursor to home position
    
    // Display game over screen with ASCII only
    printf("\n");
    printf("    +---------------------------+\n");
    printf("    |                           |\n");
    printf("    |        GAME OVER!         |\n");
    printf("    |                           |\n");
    printf("    |      Final Score: %3d     |\n", score);
    printf("    |                           |\n");
    printf("    |    A: Restart Game        |\n");
    printf("    |    START: Quit            |\n");
    printf("    |                           |\n");
    printf("    +---------------------------+\n");
    printf("\n");
    
    // Force flush output
    fflush(stdout);
}

void drawGame() {
    // Clear board and null terminate each line
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        memset(board[y], ' ', BOARD_WIDTH);
        board[y][BOARD_WIDTH] = '\0'; // Null terminate each line
    }
    
    // Draw borders
    for (int x = 0; x < BOARD_WIDTH; x++) {
        board[0][x] = '#';
        board[BOARD_HEIGHT - 1][x] = '#';
    }
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        board[y][0] = '#';
        board[y][BOARD_WIDTH - 1] = '#';
    }
    
    // Draw snake
    for (int i = 0; i < snake.length; i++) {
        if (i == 0) {
            board[snake.body[i].y][snake.body[i].x] = '@'; // Head
        } else {
            board[snake.body[i].y][snake.body[i].x] = 'O'; // Body
        }
    }
    
    // Draw food
    board[food.y][food.x] = '*';
    
    // Clear console and position cursor
    consoleClear();
    // printf("\x1b[H");   // Move cursor to home position

    // Print board line by line
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        printf("%s\n", board[y]);
    }
    
    printf("\nScore: %d\n", score);
    printf("Controls: Arrow Keys = Move\n");
    printf("          START = Quit\n");
    
    // Force flush output
    fflush(stdout);
}

int main(int argc, char* argv[])
{
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    
    srand(svcGetSystemTick());
    initGame();
    
    int frameCounter = 0;
    int gameOverDisplayed = 0; // Track if game over screen is displayed
    
    // Initial draw
    drawGame();
    
    // Main loop
    while (aptMainLoop())
    {
        hidScanInput();
        
        u32 kDown = hidKeysDown();
        
        if (kDown & KEY_START)
            break; // Exit game
        
        if (gameRunning) {
            gameOverDisplayed = 0; // Reset flag when game is running
            
            // Handle input
            if (kDown & KEY_DUP && snake.direction != 2) {
                snake.direction = 0; // Up
            } else if (kDown & KEY_DRIGHT && snake.direction != 3) {
                snake.direction = 1; // Right
            } else if (kDown & KEY_DDOWN && snake.direction != 0) {
                snake.direction = 2; // Down
            } else if (kDown & KEY_DLEFT && snake.direction != 1) {
                snake.direction = 3; // Left
            }
            
            // Update game logic every 15 frames (slower for better visibility)
            frameCounter++;
            if (frameCounter >= 15) {
                updateSnake();
                drawGame();
                frameCounter = 0;
            }
        } else {
            // Game over state - only draw once
            if (!gameOverDisplayed) {
                drawGameOver();
                gameOverDisplayed = 1;
            }
            
            if (kDown & KEY_A) {
                initGame(); // Restart game
                frameCounter = 0;
                gameOverDisplayed = 0;
                drawGame(); // Draw initial game state
            }
        }
        
        gspWaitForVBlank();
        gfxSwapBuffers();
    }
    
    gfxExit();
    return 0;
}
