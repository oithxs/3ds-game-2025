#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#define FIELD_HEIGHT 10
#define FIELD_WIDTH 10

int field[FIELD_HEIGHT][FIELD_WIDTH];

int initField();
int drawField();

// メイン関数
int main(int argc, char* argv[]) {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    initField();

    while (aptMainLoop()) {
        gspWaitForVBlank();
        
        gfxSwapBuffers();
        hidScanInput();

        u32 kDown = hidKeysDown();
        if (kDown & KEY_START)
            break;
        if (kDown & KEY_A)
            field[5][5] = 1;

        drawField();
    }

    gfxExit();
    return 0;
}

// フィールドを初期化する関数
int initField() {
    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            if (x == 0 || x == FIELD_WIDTH - 1
                || y == 0 || y == FIELD_HEIGHT - 1)
                field[y][x] = 1;
            else
                field[y][x] = 0;
        }
    }
    return 0;
}

// フィールドを描画する関数
int drawField() {
    consoleClear();

    for (int y = 0; y < FIELD_HEIGHT; y++) {
        for (int x = 0; x < FIELD_WIDTH; x++) {
            if (field[y][x] == 1)
                printf("#");
            else if (field[y][x] == 0)
                printf(" ");
        }
        printf("\n");
    }
    return 0;
}
