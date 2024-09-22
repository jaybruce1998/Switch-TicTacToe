#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

// Constants for TicTacToe grid
#define CELL_WIDTH 128
#define CELL_HEIGHT 72

// TicTacToe board
char board[3][3];

int turns;
char turn, winner;

void drawBoard() {
    consoleClear();
    printf("\x1b[0;0H");
    if (winner == 'T')
        printf("Tie!\n");
    else if (winner != 'w')
        printf("%c won!\n", winner);
    else
        printf("      \n");
    for (int i = 0; i < 3; i++) {
        printf("\n-------\n");
        for (int j = 0; j < 3; j++)
            printf("|%c", board[i][j]);
        printf("|");
    }
    printf("\n-------\n");
}

void initializeGame() {
    turn = 'X';
    winner = 'w';
    turns = 1;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            board[i][j] = ' ';
    drawBoard();
}

// Function to map touch input to the grid
void handleTouchInput(int x, int y) {
    int r = (y-40)/32;
	int c = (x-16)/28;
    // Print out the x and y coordinates for debugging
    printf("\x1b[10;1HTouch detected at: x=%03d, y=%03d", x, y);

    // Ensure the touch is within the grid
    if (r >= 0 && r < 3 && c >= 0 && c < 3) {
        // Check if the cell is empty
        if (board[r][c] == ' ') {
            // Place the current player's mark and switch to the next player
            board[r][c] = turn;
            if ((board[0][c] == turn && board[1][c] == turn && board[2][c] == turn) ||
                (board[r][0] == turn && board[r][1] == turn && board[r][2] == turn) ||
                (board[1][1] == turn &&
                 ((board[0][0] == turn && board[2][2] == turn) ||
                  (board[0][2] == turn && board[2][0] == turn))))
                winner = turn;
            else if (turns++ == 9)
                winner = 'T';
            else
                turn = (turn == 'X') ? 'O' : 'X';
            drawBoard();
        }
    }
}

// Main program entrypoint
int main(int argc, char* argv[]) {
    consoleInit(NULL);

    // Configure input and touchscreen
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);
    hidInitializeTouchScreen();

    initializeGame();

    s32 prev_touchcount = 0;

    // Main loop
    while (appletMainLoop()) {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);

        if (kDown & HidNpadButton_Plus && winner != 'w')
			initializeGame();
        // Handle touch screen input
        HidTouchScreenState state = {0};
        if (hidGetTouchScreenStates(&state, 1)) {
            if (state.count != prev_touchcount) {
                prev_touchcount = state.count;

                if (state.count > 0 && winner == 'w') {
                    // Handle the first touch point (ignoring multi-touch)
                    handleTouchInput(state.touches[0].x, state.touches[0].y);
                }
            }
        }

        // Update the console display
        consoleUpdate(NULL);
    }

    consoleExit(NULL);
    return 0;
}

