#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <string>
#include "../headers/sudoku.h"

using namespace std;

#define GAMES_STARTED = 0;
#define GAMES_WON = 0;
#define WIN_RATE = 0;

int gameMode = 0;
int maxHeight;
int maxWidth;

Sudoku *sudoku_to_check;    // Sudoku array that is correctly and fully filled with numbers
Sudoku *sudoku_to_fill;     // Sudoku that stores empty fields
Sudoku *sudoku_to_play;     // Sudoku that the player can modify

/**
 * --------------------------- INIT NCURSES FUNCTIONS ---------------------------
*/
void start_ncurses() {
    initscr();
    noecho();
    raw();
    curs_set(0);    // cursor visibility
}

// Terminal info
void getTerminalInfo() {
    getmaxyx(stdscr, maxHeight, maxWidth);

    move(1, 1);
    printw("max-Y: %d max-X: %d", maxHeight, maxWidth);

    if(!has_colors()) {
        move(2, 1);
        printw("Terminal doesn't support colors.");
        move(3, 1);
        printw("You can still play the game but your experience might not be as good as it gets ://");
    }
}

void info() {
    move(maxHeight - 2, 1);
    printw("To exit press E.");
}

/**
 * --------------------------- TEXT OUTPUT HELPER FUNCTIONS ---------------------------
*/
void highlightText(WINDOW* window, int positionY, int positionX, const char* text) {
    wattron(window, A_REVERSE);
    mvwprintw(window, positionY, positionX, text);
    wattroff(window, A_REVERSE);
}

void clearScreen(WINDOW *window, int fromX, int toX, int fromY, int toY) {
    for(int j = fromY; j < toY; j++) {
        for(int i = fromX; i < toX; i++) {
            mvwprintw(window, j, i, " ");
        }
    }
}

void toolTipMessage(string message, string message1) {
    move(14, 12);
    printw("                                                             ");
    move(14, 12);
    printw(message.c_str());
    move(14 + 1, 12);
    printw("                                                             ");
    move(14 + 1, 12);
    printw(message1.c_str());

    refresh();
}

/**
 * --------------------------- WINDOWS HELPER FUNCTIONS ---------------------------
*/
void gameMenuWindow(WINDOW* window, int highlighted);   // WINDOW_CODE: 0
void gameModeWindow(WINDOW* window);    // WINDOW_CODE: 1

void printGameMode() {
    move(2, 1);
    printw("Gamemode: ");
    if(gameMode == 0) {
        attron(COLOR_PAIR(1));
        printw("Easy        ");
        attroff(COLOR_PAIR(1));
    }
    if(gameMode == 1) {
        attron(COLOR_PAIR(2));
        printw("Medium        ");
        attroff(COLOR_PAIR(2));
    }
    if(gameMode == 2) {
        attron(COLOR_PAIR(3));
        printw("Hard        ");
        attroff(COLOR_PAIR(3));
    }
    refresh();
}

void statsWindow(WINDOW* window) {
    mvwprintw(window, 0, 2, " Statistics ");
    
    if(gameMode == 0) {
        mvwprintw(window, 2, 5, "Games started: ");
        mvwprintw(window, 2, 42, "18");
        mvwprintw(window, 3, 4, "-----------------------------------------");
        mvwprintw(window, 4, 5, "Games won: ");
        mvwprintw(window, 4, 42, "09");
        mvwprintw(window, 5, 4, "-----------------------------------------");
        mvwprintw(window, 6, 5, "Win rate:");
        mvwprintw(window, 6, 40, "50 %%");
    }

    wrefresh(window);
}

void exitPrompt(WINDOW* window, int highlighted, int returnWindow) {
    keypad(stdscr, true);
    
    int c;

    curs_set(1);
    echo();
    mvwprintw(stdscr, maxHeight - 2, 18, "Do you really want to leave? [y/n]");

    c = wgetch(stdscr);

    if(c == 121 || c == 89) {
        // y || Y
        return;
    } else {
        // Any other key
        mvwprintw(stdscr, maxHeight - 2, 18, "                                      ");
        refresh();
        keypad(stdscr, false);
        curs_set(0);
        noecho();
        if(returnWindow == 0) {
            gameMenuWindow(window, highlighted);
        }
        if(returnWindow == 1) {
            gameModeWindow(window);
        }
    }
}

// Columns
int coordinatesX[9] = {
    3, 6, 9,  15, 18, 21,  27, 30, 33
};

// Lines
int coordinatesY[9] = {
    1, 3, 5,  7, 9, 11,  13, 15, 17
};

void renderSudoku(WINDOW *window, Sudoku *sudoku, int selectedX, int selectedY) {

    mvwhline(window, 6, 1, ACS_HLINE, 35);
    mvwhline(window, 12, 1, ACS_HLINE, 35);
    mvwvline(window, 1, 12, ACS_VLINE, 17);
    mvwvline(window, 1, 24, ACS_VLINE, 17);

    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            if(sudoku->getItem(j, i) == 0) {
                if(selectedX == i && selectedY == j) {
                    wattron(window, A_UNDERLINE);
                    mvwprintw(window, coordinatesY[i], coordinatesX[j], "*");
                    wattroff(window, A_UNDERLINE);
                } else {
                    wattron(window, A_UNDERLINE);
                    mvwprintw(window, coordinatesY[i], coordinatesX[j], " ");
                    wattroff(window, A_UNDERLINE);
                    wrefresh(window);
                }
            } else {
                mvwprintw(window, coordinatesY[i], coordinatesX[j], "%d", sudoku->getItem(j, i));
                wrefresh(window);
            }
            usleep(10000);
        }
    }
}

void setNumber(WINDOW *window, int selectedX, int selectedY, char number) {
    if(sudoku_to_fill->getItem(selectedX, selectedY) == 0) {
        sudoku_to_play->setItem(selectedX, selectedY, number - '0');
        mvwprintw(window, coordinatesY[selectedY], coordinatesX[selectedX], "%d", sudoku_to_play->getItem(selectedX, selectedY));
    }
}

void startGame() {

    int height, width, start_y, start_x;
    height = 19;
    width = 37;
    start_y = 5;
    start_x = 10;

    int selectedX, selectedY;
    selectedX = 0;
    selectedY = 0;

    char choice = '0';

    for(int i = 0; i < maxWidth; i++) {
        for(int j = 0; j < maxHeight; j++) {
            move(j, i);
            printw(" ");
            refresh();
        }
    }

    printGameMode();
    getTerminalInfo();
    refresh();

    WINDOW *sudoku_window = newwin(height, width, start_y, start_x);
    box(sudoku_window, 0, 0);
    wrefresh(sudoku_window);

    sudoku_to_check = new Sudoku();
    sudoku_to_check->solveSudoku(0, 0);

    sudoku_to_fill = sudoku_to_check->copySudoku();
    sudoku_to_fill->generateSudoku(gameMode);

    sudoku_to_play = sudoku_to_fill->copySudoku();

    info();

    refresh();

    renderSudoku(sudoku_window, sudoku_to_play, selectedX, selectedY);

    while(1) {
        if(sudoku_to_play->getItem(selectedX, selectedY) == 0) {
            wattron(sudoku_window, A_UNDERLINE);
            mvwprintw(sudoku_window, coordinatesY[selectedY], coordinatesX[selectedX], "*");
            wattroff(sudoku_window, A_UNDERLINE);
            wrefresh(sudoku_window);
        } else {
            if(sudoku_to_play->getItem(selectedX, selectedY) != 0 && sudoku_to_play->getItem(selectedX, selectedY) == sudoku_to_check->getItem(selectedX, selectedY)) {
                wattron(sudoku_window, A_BOLD);
                mvwprintw(sudoku_window, coordinatesY[selectedY], coordinatesX[selectedX], "%d", sudoku_to_play->getItem(selectedX, selectedY));
                wattroff(sudoku_window, A_BOLD);
            } else {
                wattron(sudoku_window, COLOR_PAIR(4));
                wattron(sudoku_window, A_BOLD);
                mvwprintw(sudoku_window, coordinatesY[selectedY], coordinatesX[selectedX], "%d", sudoku_to_play->getItem(selectedX, selectedY));
                wattroff(sudoku_window, COLOR_PAIR(4));
                wattroff(sudoku_window, A_BOLD);
            }
            wrefresh(sudoku_window);
        }

        choice = wgetch(sudoku_window);

        if(choice == 'w') {
            if(selectedY > 0) {
                selectedY--;
                if(sudoku_to_play->getItem(selectedX, selectedY + 1) != 0) {
                    if(sudoku_to_play->getItem(selectedX, selectedY + 1) == sudoku_to_check->getItem(selectedX, selectedY + 1)) {
                        mvwprintw(sudoku_window, coordinatesY[selectedY + 1], coordinatesX[selectedX], "%d", sudoku_to_play->getItem(selectedX, selectedY + 1));
                        wrefresh(sudoku_window);
                    }
                } else {
                    wattron(sudoku_window, A_UNDERLINE);
                    mvwprintw(sudoku_window, coordinatesY[selectedY + 1], coordinatesX[selectedX], " ");
                    wattroff(sudoku_window, A_UNDERLINE);
                    wrefresh(sudoku_window);
                }
            }
        }

        if(choice == 's') {
            if(selectedY < 8) {
                selectedY++;
                if(sudoku_to_play->getItem(selectedX, selectedY - 1) != 0) {
                    if(sudoku_to_play->getItem(selectedX, selectedY - 1) == sudoku_to_check->getItem(selectedX, selectedY - 1)) {
                        mvwprintw(sudoku_window, coordinatesY[selectedY - 1], coordinatesX[selectedX], "%d", sudoku_to_play->getItem(selectedX, selectedY - 1));
                        wrefresh(sudoku_window);
                    }
                } else {
                    wattron(sudoku_window, A_UNDERLINE);
                    mvwprintw(sudoku_window, coordinatesY[selectedY - 1], coordinatesX[selectedX], " ");
                    wattroff(sudoku_window, A_UNDERLINE);
                    wrefresh(sudoku_window);
                }
            }
        }

        if(choice == 'a') {
            if(selectedX > 0) {
                selectedX--;
                if(sudoku_to_play->getItem(selectedX + 1, selectedY) != 0) {
                    if(sudoku_to_play->getItem(selectedX + 1, selectedY) == sudoku_to_check->getItem(selectedX + 1, selectedY)) {
                        mvwprintw(sudoku_window, coordinatesY[selectedY], coordinatesX[selectedX + 1], "%d", sudoku_to_play->getItem(selectedX + 1, selectedY));
                        wrefresh(sudoku_window);
                    }
                } else {
                    wattron(sudoku_window, A_UNDERLINE);
                    mvwprintw(sudoku_window, coordinatesY[selectedY], coordinatesX[selectedX + 1], " ");
                    wattroff(sudoku_window, A_UNDERLINE);
                    wrefresh(sudoku_window);
                }
            }
        }

        if(choice == 'd') {
            if(selectedX < 8) {
                selectedX++;
                if(sudoku_to_play->getItem(selectedX - 1, selectedY) != 0) {
                    if(sudoku_to_play->getItem(selectedX - 1, selectedY) == sudoku_to_check->getItem(selectedX - 1, selectedY)) {
                        mvwprintw(sudoku_window, coordinatesY[selectedY], coordinatesX[selectedX - 1], "%d", sudoku_to_play->getItem(selectedX - 1, selectedY));
                        wrefresh(sudoku_window);
                    }
                } else {
                    wattron(sudoku_window, A_UNDERLINE);
                    mvwprintw(sudoku_window, coordinatesY[selectedY], coordinatesX[selectedX - 1], " ");
                    wattroff(sudoku_window, A_UNDERLINE);
                    wrefresh(sudoku_window);
                }
            }
        }

        if(choice == 'e' || choice == 'E') {
            // User pressed 'E' or 'e' key
            return;
        }

        if(choice > '1' && choice <= '9') {
            setNumber(sudoku_window, selectedX, selectedY, choice);
        }
    }
}

void gameModeWindow(WINDOW* window) {
    string modes[3] = {"Easy", "Medium", "Hard"};
    
    int choice;
    int highlighted = gameMode;

    keypad(window, true);
    wattron(window, A_BOLD);
    wattron(window, A_UNDERLINE);
    mvwprintw(window, 5, 5, "Game mode");
    wattroff(window, A_BOLD);
    wattroff(window, A_UNDERLINE);

    toolTipMessage("Select an option with ENTER", "Press LEFT_ARROW to close the submenu");

    wrefresh(window);
    while(1) {
        for(int i = 0; i < 3; i++) {
            if(i == highlighted) {
                highlightText(window, 2 * (i + 1), 28, modes[i].c_str());
            } else {
                mvwprintw(window, 2 * (i + 1), 28, modes[i].c_str());
            }
        }

        choice = wgetch(window);

        switch (choice) {
            case KEY_LEFT:
                // Exit submenu on click left but do not select the game mode
                choice = -10;
                break;

            case KEY_UP:
                if(highlighted > 0) { highlighted--; }
                break;

            case KEY_DOWN:
                if(highlighted < 2) { highlighted++; }
                break;
            
            default:
                break;
            }

        if(choice == 10) {
            // User pressed Enter key
            clearScreen(window, 28, 38, 2, 7);
            gameMode = highlighted;
            break;
        }

        if(choice == -10) {
            clearScreen(window, 28, 38, 2, 7);
            break;
        }

        if(choice == 101 || choice == 69) {
            // User pressed 'E' or 'e' key
            break;
        }
    }

    if(choice == 101 || choice == 69) {
        exitPrompt(window, highlighted, 1);
    } else {
        gameMenuWindow(window, 1);
    }

}

void gameMenuWindow(WINDOW* window, int highlighted) {
    int choice = 0;

    printGameMode();

    string choices[3] = {"New game", "Game mode"};

    keypad(window, true);   // So we can use arrow keys

    while(1) {
        mvwprintw(window, 0, 2, " Sudoku puzzle main menu ");

        for(int i = 0; i < 2; i++) {
            if(i == highlighted) {
                wattron(window, A_REVERSE);
                mvwprintw(window, 2 * (i + 1) + 1, 5, choices[i].c_str());
                wattroff(window, A_REVERSE);

                if(i == 0) {
                    toolTipMessage("Press ENTER to start a new game.", "");
                } else {
                    toolTipMessage("Press RIGHT_ARROW or ENTER to open submenu.", "");
                }

            } else {
                mvwprintw(window, 2 * (i + 1) + 1, 5, choices[i].c_str());
            }
        }

        choice = wgetch(window);

        switch (choice) {
            case KEY_UP:
                if(highlighted > 0) { highlighted--; }
                break;
            case KEY_DOWN:
                if(highlighted < 1) { highlighted++; }
                break;
            case KEY_RIGHT:
                choice = 10;
                break;

            default:
                break;
        }

        if(choice == 10) {
            // User pressed Enter key
            break;
        }

        if(choice == 101 || choice == 69) {
            // User pressed 'E' or 'e' key
            break;
        }
    }

    if(choice == 101 || choice == 69) {
        exitPrompt(window, highlighted, 0);
    } else {
        if(highlighted == 0) {
            startGame();
        }
        
        if(highlighted == 1) {
            gameModeWindow(window);
        }
    }
}

/**
 * --------------------------- START PROGRAM ---------------------------
*/
int main(int argc, char * argv[]) {

    int height, width, start_y, start_x;
    height = 9;
    width = 50;
    start_y = 5;
    start_x = 10;
    
    start_ncurses();

    WINDOW *start_menu_window = newwin(height, width, start_y, start_x);
    WINDOW *stats_window = newwin(height, width, start_y, start_x + width + 10);

    getTerminalInfo();
    info();
    refresh();

    box(start_menu_window, 0, 0);
    box(stats_window, 0, 0);

    wrefresh(start_menu_window);
    wrefresh(stats_window);

    start_color();
    // init_pair(0, COLOR_GREEN, COLOR_BLACK);
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);

    statsWindow(stats_window);
    gameMenuWindow(start_menu_window, 0);

    endwin();
    std::cout << "\n";
    std::cout << "Program ended.\n";
    return 0;
}
