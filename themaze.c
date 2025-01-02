#include <ncurses.h>
#include <time.h>
#include <stdio.h>

void pause_menu(){
    int choice = 0;
    int highlight = 0;
    const char* choices[] = {"Resume", "Main Menu"};
    WINDOW *pause = newwin(20, 60, 5, 5);
    keypad(pause, TRUE);
    
    box(pause, 0, 0);

    while(1){
        for(int i = 0; i < 2; i++){
            if(i == highlight)
                wattron(pause, A_REVERSE);
            mvwprintw(pause, 12+2*i, 15, "%s", choices[i]);
            wattroff(pause, A_REVERSE);
        }
        choice = wgetch(pause);
        
        switch(choice)
        {
            case KEY_UP:
                highlight--;
                if(highlight < 0)
                    highlight = 0;
                break;
            case KEY_DOWN:
                highlight++;
                if(highlight > 1)
                    highlight = 1;
                break;
            default:
                break;
        }
        wrefresh(pause);
    }
    wrefresh(pause);
}

int main() {
    int x = 1, y = 1; // Initial position of 'O' inside the maze
    int key;

    time_t start_time = time(NULL);  // Get the current time
    time_t current_time;
    int elapsed_time = 0;

    initscr();                // Initialize ncurses mode
    cbreak();                 // Disable line buffering
    noecho();                 // Disable echoing of input
    curs_set(0); // Hide the cursor

    // Create a new window for the maze
    WINDOW *timer = newwin(3, 12, 1, 5);
    WINDOW *menu = newwin(20, 60, 5, 5);
    WINDOW *maze = newwin(20, 60, 5, 5);

    nodelay(maze, TRUE);
    keypad(menu, TRUE);
    keypad(maze, TRUE);       // Enable keypad for the maze window

    // Draw the initial box around the maze
    box(menu, 0, 0);
    box(maze, 0, 0);

    mvwprintw(menu, 3, 20, "Welcome to the Maze Game!!");
    mvwprintw(menu, 15, 20, "press any key to start the game...");
    wrefresh(menu);
    
    wgetch(menu);

    wrefresh(maze);           // Refresh the maze window to display the box

    while ((key = wgetch(maze)) != 'q') { // Read input from the maze window
        if(key != ERR){
            // Erase the previous 'O' from the maze
            mvwprintw(maze, y, x, " ");
            
            // Update position based on keypress
            if (key == KEY_LEFT) {
                x--;
                if (x < 1) x = 1; // Keep within left bound
            }
            else if (key == KEY_RIGHT) {
                x++;
                if (x > 58) x = 58; // Keep within right bound
            }
            else if (key == KEY_UP) {
                y--;
                if (y < 1) y = 1; // Keep within top bound
            }
            else if (key == KEY_DOWN) {
                y++;
                if (y > 18) y = 18; // Keep within bottom bound
            }
            else if (key == 27) {
                pause_menu();
            }

            // Redraw the box and the updated 'O'
            box(maze, 0, 0);
            mvwprintw(maze, y, x, "O");
            wrefresh(maze); // Refresh the maze window to show changes
        }

        current_time = time(NULL);  // Get the current time
        elapsed_time = (int)difftime(current_time, start_time);  // Calculate the elapsed time
        
        char formatted_time[5];
        snprintf(formatted_time, sizeof(formatted_time), "%04d", elapsed_time);

        box(timer, 0, 0);
        mvwprintw(timer, 1, 1, "Time: %s", formatted_time);
        wrefresh(timer);
    }
    delwin(timer);
    delwin(menu);
    delwin(maze); // Delete the maze window
    endwin();     // End ncurses mode
    return 0;
}

