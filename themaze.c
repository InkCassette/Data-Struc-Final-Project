#include <ncurses.h>

int main() {
    int x, y;
    char key;
    initscr();            // Initialize ncurses mode
    cbreak();            // Disable line buffering
    noecho();            // Disable echoing of input
    keypad(stdscr, TRUE);

    printw("Hello, ncurses!"); // Print text on the screen
    refresh();            // Refresh the screen to show changes
    key = getch();
    while(key != 'q'){
        clear();
        move(0,0);
        printw("press arrow key to move");
        refresh();

        move(y,x);
        printw("O");
        refresh();
        key = getch();
        if(key == KEY_LEFT){
            x--;
            printw("x = %c",x);
            if(x < 0)
                x = 0;
        }
        else if(key == KEY_RIGHT){
            x--;
            printw("x = %c",x);
            if(x > 30)
                x = 30;
        }
        else if(key == KEY_UP){
            y--;
            printw("y = %c",y);
            if(y < 0)
                y = 0;
        }
        else if(key == KEY_DOWN){
            y++;
            printw("y = %c",y);
            if(y > 10)
                y = 10;
        }

    }
    printw("x = %c, y = %c", x, y);
    getch();
    endwin();             // End ncurses mode
    return 0;
}

