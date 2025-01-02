#include <ncurses.h>
/*
int main() {
    int x, y;
    int key;
    initscr();            // Initialize ncurses mode
    WINDOW *maze = newwin(10,30,3,5);
    cbreak();            // Disable line buffering
    noecho();            // Disable echoing of input
    keypad(maze, TRUE);

    printw("Hello, ncurses!"); // Print text on the screen
    refresh();            // Refresh the screen to show changes
    wrefresh(maze);
    key = getch();
    while(key != 'q'){
        clear();
        box(maze,0,0);
        move(0,0);
        printw("press arrow key to move");
        wrefresh(maze);
        move(y,x);
        printw("O");
        wrefresh(maze);
        key = getch();
        if(key == KEY_LEFT){
            x--;
            if(x < 0)
                x = 0;
        }
        else if(key == KEY_RIGHT){
            x++;
            if(x > 30)
                x = 30;
        }
        else if(key == KEY_UP){
            y--;
            if(y < 0)
                y = 0;
        }
        else if(key == KEY_DOWN){
            y++;
            if(y > 10)
                y = 10;
        }

    }
    endwin();             // End ncurses mode
    return 0;
}
*/
#include <ncurses.h>

int main() {
    int x = 1, y = 1; // Initial position of 'O' inside the maze
    int key;

    initscr();                // Initialize ncurses mode
    cbreak();                 // Disable line buffering
    noecho();                 // Disable echoing of input
    curs_set(0); // Hide the cursor


    // Create a new window for the maze
    WINDOW *maze = newwin(10, 30, 3, 5);
    keypad(maze, TRUE);       // Enable keypad for the maze window

    // Draw the initial box around the maze
    box(maze, 0, 0);
    wrefresh(maze);           // Refresh the maze window to display the box

    while ((key = wgetch(maze)) != 'q') { // Read input from the maze window
        // Erase the previous 'O' from the maze
        mvwprintw(maze, y, x, " ");

        // Update position based on keypress
        if (key == KEY_LEFT) {
            x--;
            if (x < 1) x = 1; // Keep within left bound
        } else if (key == KEY_RIGHT) {
            x++;
            if (x > 28) x = 28; // Keep within right bound
        } else if (key == KEY_UP) {
            y--;
            if (y < 1) y = 1; // Keep within top bound
        } else if (key == KEY_DOWN) {
            y++;
            if (y > 8) y = 8; // Keep within bottom bound
        }

        // Redraw the box and the updated 'O'
        box(maze, 0, 0);
        mvwprintw(maze, y, x, "O");
        wrefresh(maze); // Refresh the maze window to show changes
    }

    delwin(maze); // Delete the maze window
    endwin();     // End ncurses mode
    return 0;
}

