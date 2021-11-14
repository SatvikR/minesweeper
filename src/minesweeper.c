// Copyright (c) 2021, Satvik Reddy

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "raylib.h"

#define BOARD_WIDTH  18
#define BOARD_HEIGHT 14

#define CELL_WIDTH    30
#define SCREEN_WIDTH  (BOARD_WIDTH * CELL_WIDTH)
#define SCREEN_HEIGHT (BOARD_HEIGHT * CELL_WIDTH + 40)

#define TOTAL_MINES 40

typedef struct CellCoordinate
{
    int col;
    int row;
} CellCoordinate;

typedef struct Cell
{
    bool isMine;
    uint8_t num;

    bool isDiscovered;
    bool isFlag;
} Cell;

typedef struct ApplicationData
{
    bool firstClick;

    Cell cells[BOARD_HEIGHT][BOARD_WIDTH];

    Texture2D flagTexture;

    Texture2D oneTexture;
    Texture2D twoTexture;
    Texture2D threeTexture;
    Texture2D fourTexture;
    Texture2D fiveTexture;
    Texture2D sixTexture;
    Texture2D sevenTexture;
    Texture2D eightTexture;

    Font font;

    int numFlags;

    bool hasWon;
    int winTime;
    double initTime;
} ApplicationData;

static ApplicationData state;

void printBoard(void)
{
    int i, j;
    for (i = 0; i < BOARD_HEIGHT; i++)
    {
        for (j = 0; j < BOARD_WIDTH; j++)
        {
            if (state.cells[i][j].isMine)
                printf(" f ");
            else if (state.cells[i][j].num == 0)
                printf(" n ");
            else
                printf(" %u ", state.cells[i][j].num);
        }
        printf("\n");
    }
}

// row and col of the first click
void initBoard(int row, int col)
{
    int i, j;

placeMines:

    int minesPlaced = 0;
    for (i = 0; i < BOARD_HEIGHT; i++)
    {
        for (j = 0; j < BOARD_WIDTH; j++)
        {
            if (abs(i - row) <= 1 && abs(j - col) <= 1)
                continue;

            if (rand() % ((BOARD_HEIGHT * BOARD_WIDTH) - 9) <= TOTAL_MINES)
            {
                state.cells[i][j].isMine = true;
                if (++minesPlaced >= TOTAL_MINES)
                    goto countMines;
            }
        }
    }
    goto placeMines;


countMines:
    // Counter inner mines
    for (i = 1; i < BOARD_HEIGHT-1; i++)
    {
        for (j = 1; j < BOARD_WIDTH-1; j++)
        {
            uint8_t numMines = 0;

            int f, k;
            for (f = i-1; f < i+2; f++)
            {
                for (k = j-1; k < j+2; k++)
                {
                    if (f == i && k == j)
                        continue;

                    if (state.cells[f][k].isMine)
                        numMines++;
                }
            }
            state.cells[i][j].num = numMines;
        }
    }

    // Counting outer mines

    // Top
    for (i = 1; i < BOARD_WIDTH-1; i++)
    {
        if (state.cells[0][i].isMine)
            continue;

        uint8_t numMines = 0;
        int f, k;
        for (f = 0; f < 2; f++)
        {
            for (k = i-1; k < i+2; k++)
            {
                if (f == 0 && k == i)
                    continue;

                if (state.cells[f][k].isMine)
                    numMines++;
            }
        }
        state.cells[0][i].num = numMines;
    }

    // Bottom
    for (i = 1; i < BOARD_WIDTH-1; i++)
    {
        if (state.cells[BOARD_HEIGHT-1][i].isMine)
            continue;

        uint8_t numMines = 0;
        int f, k;
        for (f = BOARD_HEIGHT-2; f < BOARD_HEIGHT; f++)
        {
            for (k = i-1; k < i+2; k++)
            {
                if (f == BOARD_HEIGHT-1 && k == i)
                    continue;

                if (state.cells[f][k].isMine)
                    numMines++;
            }
        }
        state.cells[BOARD_HEIGHT-1][i].num = numMines;
    }

    // Left
    for (i = 1; i < BOARD_HEIGHT-1; i++)
    {
        if (state.cells[i][0].isMine)
            continue;

        uint8_t numMines = 0;
        int f, k;
        for (f = i-1; f < i+2; f++)
        {
            for (k = 0; k < 2; k++)
            {
                if (f == i && k == 0)
                    continue;

                if (state.cells[f][k].isMine)
                    numMines++;
            }
        }
        state.cells[i][0].num = numMines;
    }

    // Right
    for (i = 1; i < BOARD_HEIGHT-1; i++)
    {
        if (state.cells[i][BOARD_WIDTH-1].isMine)
            continue;

        uint8_t numMines = 0;
        int f, k;
        for (f = i-1; f < i+2; f++)
        {
            for (k = BOARD_WIDTH-2; k < BOARD_WIDTH; k++)
            {
                if (f == i && k == BOARD_WIDTH-1)
                    continue;

                if (state.cells[f][k].isMine)
                    numMines++;
            }
        }
        state.cells[i][BOARD_WIDTH-1].num = numMines;
    }

    // Corners
    if (!state.cells[0][0].isMine)
    {
        uint8_t numMines = 0;
        int f, k;
        for (f = 0; f < 2; f++)
        {
            for (k = 0; k < 2; k++)
            {
                if (f == 0 && k == 0)
                    continue;

                if (state.cells[f][k].isMine)
                    numMines++;
            }
        }
        state.cells[0][0].num = numMines;
    }

    if (!state.cells[0][BOARD_WIDTH-1].isMine)
    {
        uint8_t numMines = 0;
        int f, k;
        for (f = 0; f < 2; f++)
        {
            for (k = BOARD_WIDTH-2; k < BOARD_WIDTH; k++)
            {
                if (f == 0 && k == BOARD_WIDTH-1)
                    continue;

                if (state.cells[f][k].isMine)
                    numMines++;
            }
        }
        state.cells[0][BOARD_WIDTH-1].num = numMines;
    }

    if (!state.cells[BOARD_HEIGHT-1][0].isMine)
    {
        uint8_t numMines = 0;
        int f, k;
        for (f = BOARD_HEIGHT-2; f < BOARD_HEIGHT; f++)
        {
            for (k = 0; k < 2; k++)
            {
                if (f == BOARD_HEIGHT-1 && k == 0)
                    continue;

                if (state.cells[f][k].isMine)
                    numMines++;
            }
        }
        state.cells[BOARD_HEIGHT-1][0].num = numMines;
    }

    if (!state.cells[BOARD_HEIGHT-1][BOARD_WIDTH-1].isMine)
    {
        uint8_t numMines = 0;
        int f, k;
        for (f = BOARD_HEIGHT-2; f < BOARD_HEIGHT; f++)
        {
            for (k = BOARD_WIDTH-2; k < BOARD_WIDTH; k++)
            {
                if (f == BOARD_HEIGHT-1 && k == BOARD_WIDTH-1)
                    continue;

                if (state.cells[f][k].isMine)
                    numMines++;
            }
        }
        state.cells[BOARD_HEIGHT-1][BOARD_WIDTH-1].num = numMines;
    }
}

void initState(void)
{
    state.firstClick = false;

    int i, j;
    for (i = 0; i < BOARD_HEIGHT; i++)
    {
        for (j = 0; j < BOARD_WIDTH; j++)
        {
            state.cells[i][j].isMine = false;
            state.cells[i][j].num = 0;

            state.cells[i][j].isDiscovered = false;
            state.cells[i][j].isFlag = false;
        }
    }

    state.numFlags = TOTAL_MINES;
    state.hasWon = false;
    state.initTime = -1.0;
}

CellCoordinate calculateCellClicked(void)
{
    Vector2 mousePos = GetMousePosition();

    return (CellCoordinate) {
        .row = ((int) mousePos.x) / CELL_WIDTH,
        .col = ((int) mousePos.y) / CELL_WIDTH
    };
}

void discoverCell(int col, int row)
{
    if (
        col < 0 || col >= BOARD_HEIGHT ||
        row < 0 || row >= BOARD_WIDTH
    )
    {
        return;
    }

    if (state.cells[col][row].isDiscovered || state.cells[col][row].isFlag)
        return;

    state.cells[col][row].isDiscovered = true;

    if (state.cells[col][row].num == 0)
    {
        int i, j;
        for (i = col-1; i < col+2; i++)
        {
            for (j = row-1; j < row+2; j++)
            {
                if (i == col && j == row)
                    continue;

                discoverCell(i, j);
            }
        }
    }
}

bool checkWin(void)
{
    int discovered = 0;

    int i, j;
    for (i = 0; i < BOARD_HEIGHT; i++)
    {
        for (j = 0; j < BOARD_WIDTH; j++)
        {
            if (state.cells[i][j].isDiscovered)
            {
                if (++discovered >= (BOARD_WIDTH * BOARD_HEIGHT) - TOTAL_MINES)
                    return true;
            }
        }
    }
    return false;
}

void init(void)
{
    srand((unsigned int)time(NULL));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [core] example - basic window");

    state.flagTexture = LoadTexture("assets/flag.png");

    state.oneTexture = LoadTexture("assets/one.png");
    state.twoTexture = LoadTexture("assets/two.png");
    state.threeTexture = LoadTexture("assets/three.png");
    state.fourTexture = LoadTexture("assets/four.png");
    state.fiveTexture = LoadTexture("assets/five.png");
    state.sixTexture = LoadTexture("assets/six.png");
    state.sevenTexture = LoadTexture("assets/seven.png");
    state.eightTexture = LoadTexture("assets/eight.png");

    state.font = LoadFont("assets/Consolas.ttf");

    initState();
}

void close(void)
{
    CloseWindow();

    UnloadTexture(state.oneTexture);
    UnloadTexture(state.twoTexture);
    UnloadTexture(state.threeTexture);
    UnloadTexture(state.fourTexture);
    UnloadTexture(state.fiveTexture);
    UnloadTexture(state.sixTexture);
    UnloadTexture(state.sevenTexture);
    UnloadTexture(state.eightTexture);

    UnloadFont(state.font);
}

bool draw(void)
{
    ClearBackground(RAYWHITE);

    int i, j;
    bool color = true;
    for (i = 0; i < BOARD_HEIGHT; i++)
    {
        color = !color;
        for (j = 0; j < BOARD_WIDTH; j++)
        {
            Color c;
            color = !color;

            if (state.cells[i][j].isDiscovered)
            {
                c = color ? BEIGE : BROWN;
                DrawRectangle(
                    j * CELL_WIDTH, i * CELL_WIDTH, CELL_WIDTH, CELL_WIDTH, c
                );

                switch (state.cells[i][j].num)
                {
                case 1:
                    DrawTexture(
                        state.oneTexture, j * CELL_WIDTH, i * CELL_WIDTH, WHITE
                    );
                    break;
                case 2:
                    DrawTexture(
                        state.twoTexture, j * CELL_WIDTH, i * CELL_WIDTH, WHITE
                    );
                    break;
                case 3:
                    DrawTexture(
                        state.threeTexture, j * CELL_WIDTH, i * CELL_WIDTH, WHITE
                    );
                    break;
                case 4:
                    DrawTexture(
                        state.fourTexture, j * CELL_WIDTH, i * CELL_WIDTH, WHITE
                    );
                    break;
                case 5:
                    DrawTexture(
                        state.fiveTexture, j * CELL_WIDTH, i * CELL_WIDTH, WHITE
                    );
                    break;
                case 6:
                    DrawTexture(
                        state.sixTexture, j * CELL_WIDTH, i * CELL_WIDTH, WHITE
                    );
                    break;
                case 7:
                    DrawTexture(
                        state.sevenTexture, j * CELL_WIDTH, i * CELL_WIDTH, WHITE
                    );
                    break;
                case 8:
                    DrawTexture(
                        state.eightTexture, j * CELL_WIDTH, i * CELL_WIDTH, WHITE
                    );
                    break;
                default:
                    break;
                }
            }
            else
            {
                c = color ? LIME : GREEN;
                DrawRectangle(
                    j * CELL_WIDTH, i * CELL_WIDTH, CELL_WIDTH, CELL_WIDTH, c
                );

                if (state.cells[i][j].isFlag)
                {
                    DrawTexture(
                        state.flagTexture, j * CELL_WIDTH, i * CELL_WIDTH, WHITE
                    );
                }
            }
        }
    }

    DrawTextEx(
        state.font, TextFormat("Flags: %d", state.numFlags),
        (Vector2) { .x = 10, .y = SCREEN_HEIGHT - 30 }, 16, 1.6f, BLACK
    );

    if (state.hasWon)
    {
        int width = (int) MeasureTextEx(
            state.font, "You Won!", 16, 1.6f
        ).x;

        DrawTextEx(
            state.font, "You Won!",
            (Vector2) { .x = (float) (SCREEN_WIDTH - width) / 2.0f, .y = (float) (SCREEN_HEIGHT - 30) },
            16, 1.6f, BLACK
        );

        DrawTextEx(
            state.font, TextFormat("Time: %d", state.winTime),
            (Vector2) { .x = (SCREEN_WIDTH - 100), .y = SCREEN_HEIGHT - 30 },
            16, 1.6f, BLACK
        );

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            return true;
    }
    else
    {
        int dt = state.firstClick ? (int) (GetTime() - state.initTime) : 0;
        dt = dt > 999 ? 999 : dt;
        DrawTextEx(
            state.font, TextFormat("Time: %d", dt),
            (Vector2) { .x = (SCREEN_WIDTH - 100), .y = SCREEN_HEIGHT - 30 },
            16, 1.6f, BLACK
        );

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            if (state.firstClick)
            {
                CellCoordinate cellClicked = calculateCellClicked();
                int row = cellClicked.row, col = cellClicked.col;

                state.cells[col][row].isFlag = !state.cells[col][row].isFlag;

                if (state.cells[col][row].isFlag)
                    state.numFlags--;
                else
                    state.numFlags++;
            }
        }
        else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            CellCoordinate cellClicked = calculateCellClicked();
            int row = cellClicked.row, col = cellClicked.col;

            if (!state.firstClick)
            {
                initBoard(row, col);
                state.firstClick = true;
                state.initTime = GetTime();
                printBoard();
            }
            else if (state.cells[col][row].isFlag)
                return false;
            else if (state.cells[col][row].isMine)
                return true;
            else if (state.cells[col][row].isDiscovered)
                return false;

            discoverCell(col, row);
            if (checkWin())
            {
                state.hasWon = true;
                state.winTime = dt;
            }
        }
    }

    return false;
}

int main(void)
{
    init();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        bool restart = draw();
        EndDrawing();

        if (restart)
            initState();
    }

    CloseWindow();

    return 0;
}