#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <string>

using namespace std;

int gameMode = 0;
int maxHeight;
int maxWidth;

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

/**
 * --------------------------- WINDOWS HELPER FUNCTIONS ---------------------------
*/

void gameMenuWindow(WINDOW* window, int highlighted);

void statsWindow(WINDOW* window) {
    
}

void startGame(WINDOW* window) {

}

void exitPrompt(WINDOW* window) {
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
        mvwprintw(stdscr, maxHeight - 2, 18, "Aborting...                           ");
        refresh();
        keypad(stdscr, false);
        curs_set(0);
        noecho();
        gameMenuWindow(window, 0);
    }
}

void gameModeWindow(WINDOW* window) {
    string modes[3] = {"Easy", "Hard", "Impossible"};
    int choice;
    int highlighted = gameMode;
    keypad(window, true);
    wattron(window, A_BOLD);
    wattron(window, A_UNDERLINE);
    mvwprintw(window, 6, 5, "Game mode");
    wattroff(window, A_BOLD);
    wattroff(window, A_UNDERLINE);

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
                // Exit submenu on click left
                choice = 10;
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
    }

    gameMenuWindow(window, 2);

}

void gameMenuWindow(WINDOW* window, int highlighted) {
    int choice = 0;

    string choices[3] = {"New game", "Statistics", "Game mode"};

    keypad(window, true);   // So we can use arrow keys

    while(1) {
        mvwprintw(window, 0, 2, " Sudoku puzzle main menu ");

        for(int i = 0; i < 3; i++) {
            if(i == highlighted) {
                wattron(window, A_REVERSE);
                mvwprintw(window, 2 * (i + 1), 5, choices[i].c_str());
                wattroff(window, A_REVERSE);
            } else {
                mvwprintw(window, 2 * (i + 1), 5, choices[i].c_str());
            }
        }

        choice = wgetch(window);

        switch (choice) {
            case KEY_UP:
                if(highlighted > 0) { highlighted--; }
                break;
            case KEY_DOWN:
                if(highlighted < 2) { highlighted++; }
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

        if(choice == 101) {
            // User pressed Enter key
            break;
        }
    }

    if(choice == 101) {
        exitPrompt(window);
    } else {
        if(highlighted == 0) {
            startGame(window);
        }
        
        if(highlighted == 1) {
            statsWindow(window);
        }

        if(highlighted == 2) {
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

    getTerminalInfo();
    info();
    refresh();

    box(start_menu_window, 0, 0);

    wrefresh(start_menu_window);

    gameMenuWindow(start_menu_window, 0);

    endwin();
    cout << "\n";
    cout << "Program ended.\n";
    return 0;
}
