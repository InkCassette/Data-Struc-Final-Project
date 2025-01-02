#include <ncurses.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

int main_menu(int time_in_main);
int pause_menu(int time_in_pause);
int maze(int time_in_maze);
void setting_menu();
void credits_menu();
void tutorial();
void tutor_animation(WINDOW *tutorial, int page_on);
void do_nothing();

int x = 1, y = 1;
int level_counter = 0;
int point_counter = 0;


void do_nothing() {
    // This function intentionally does nothing
}

int pause_menu(int time_in_pause){
    int choice = 0;
    int highlight = 0;
    const char* choices[] = {"Resume", "Main Menu"};
    WINDOW *pause = newwin(20, 60, 5, 5);
    keypad(pause, TRUE);
    
    box(pause, 0, 0);

    mvwprintw(pause, 2, 40, "Time Passed: ");
    wattron(pause, COLOR_PAIR(1));
    mvwprintw(pause, 2, 53, "%d", time_in_pause);
    wattroff(pause, COLOR_PAIR(1));

    mvwprintw(pause, 3, 46, "Level: ");
    wattron(pause, COLOR_PAIR(2));
    mvwprintw(pause, 3, 53, "%d", level_counter);
    wattroff(pause, COLOR_PAIR(2));

    mvwprintw(pause, 4, 45, "Points: ");
    mvwprintw(pause, 4, 53, "%d", point_counter);

    wrefresh(pause);

    while(1){
        for(int i = 0; i < 2; i++){
            if(i == highlight)
                wattron(pause, A_REVERSE);
            mvwprintw(pause, 3+2*i, 5, "%s", choices[i]);
            wattroff(pause, A_REVERSE);
        }
        choice = wgetch(pause);
        if(choice == '\n'){
            if(highlight == 0){
                return time_in_pause;
            }   
            else if(highlight == 1){
                wclear(pause);
                wrefresh(pause);
                main_menu(0);
            }
        }
        else if(choice == 27){
            return time_in_pause;
        }
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
}

int main_menu(int time_in_main){
    x = 1, y = 1;
    int choice = 0;
    int highlight = 0;
    const char* choices[] = {"How to Play", "Start", "Setting", "Credits", "Quit"};

    WINDOW *menu = newwin(20, 60, 5, 5);
    keypad(menu, TRUE);
    
    box(menu, 0, 0);
    
    mvwprintw(menu, 5, 18, "Welcome to the Maze Game!!");
    while(1){
        for(int i = 0; i < 5; i++){
            if(i == highlight)
                wattron(menu, A_REVERSE);
            
            if(i == 0)
                mvwprintw(menu, 13, 24, "%s", choices[i]);
            else
                mvwprintw(menu, 14, 7+8*i, "%-7s", choices[i]);
            wattroff(menu, A_REVERSE);
        }
        choice = wgetch(menu);
        if(choice == '\n'){
            if(highlight == 0){
                tutorial();
            }
            else if(highlight == 1){
                wclear(menu);
                wrefresh(menu);
                maze(time_in_main);
            }   
            else if(highlight == 2){
                wclear(menu);
                wrefresh(menu);
                setting_menu();
            }
            else if(highlight == 3){
                wclear(menu);
                wrefresh(menu);
                credits_menu();
            }
            else if(highlight == 4){
                endwin();
                exit(0);
            }
        }
        switch(choice)
        {
            case KEY_LEFT:
                highlight--;
                if(highlight < 1)
                    highlight = 1;
                break;
            case KEY_RIGHT:
                highlight++;
                if(highlight > 4)
                    highlight = 4;
                break;
            case KEY_UP:
                if(highlight != 0)
                    highlight = 0;
                break;
            case KEY_DOWN:
                if(highlight == 0)
                    highlight = 2;
                break;
            default:
                break;
        }
        wrefresh(menu);
    }
}

int maze(int time_in_maze){
    time_t start_time = time(NULL);  // Get the current time
    time_t current_time;
    int elapsed_time = 0;
    int key;

    if(time_in_maze != 0){  //Add the time passed in prev session
        elapsed_time = time_in_maze;
    }

    WINDOW *timer = newwin(3, 12, 1, 5);
    WINDOW *maze = newwin(20, 60, 5, 5);
    WINDOW *cog = newwin(3, 9, 1, 54);

    nodelay(maze, TRUE);
    keypad(maze, TRUE);       // Enable keypad for the maze window

    // Draw the initial box around the maze
    box(maze, 0, 0);
    box(cog, 0, 0);

    mvwprintw(cog, 1, 1, "ESC   *");

    wrefresh(cog);
    wrefresh(maze);           // Refresh the maze window to display the box

    while (key = wgetch(maze)) { // Read input from the maze window
        box(maze, 0, 0);
        mvwprintw(maze, y, x, "O");
        wrefresh(maze); // Refresh the maze window to show changes
        
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
                wclear(cog);
                wrefresh(cog);
                wclear(timer);
                wrefresh(timer);
                time_in_maze = pause_menu(time_in_maze+elapsed_time);
                start_time = time(NULL);
            }

            // Redraw the box and the updated 'O'
            box(maze, 0, 0);
            mvwprintw(maze, y, x, "O");
            wrefresh(maze); // Refresh the maze window to show changes
        }

        current_time = time(NULL);  // Get the current time
        elapsed_time = (int)difftime(current_time, start_time);

        char formatted_time[5];
        snprintf(formatted_time, sizeof(formatted_time), "%04d", elapsed_time + time_in_maze);

        box(timer, 0, 0);

        mvwprintw(timer, 1, 1, "Time: ");
        wattron(timer, COLOR_PAIR(1));
        mvwprintw(timer, 1, 6, "%s", formatted_time);
        wattroff(timer, COLOR_PAIR(1));
        
        wrefresh(timer);
    }
    wclear(timer);
    wclear(maze);
    wrefresh(timer);
    wrefresh(maze);
    endwin();
    exit(0);
}

void setting_menu(){
    int choice = 0;
    int highlight = 0;
    const char* choices[] = {"Difficulty", "Return"};
    
    WINDOW *setting = newwin(20, 60, 5, 5);
    keypad(setting, TRUE);
    box(setting, 0, 0);
    

    while(1){
        for(int i = 0; i < 2; i++){
            if(i == highlight)
                wattron(setting, A_REVERSE);
            mvwprintw(setting, 5+8*i, 17, "%s", choices[i]);
            wattroff(setting, A_REVERSE);
        }
        choice = wgetch(setting);
        if(choice == '\n'){
            if(highlight == 0){
                mvwprintw(setting, 7, 18, "Sorry, Difficulty is now WIP...");
                wrefresh(setting);
            }   
            else if(highlight == 1){
                wclear(setting);
                wrefresh(setting);
                main_menu(0);
            }
        }
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
        wrefresh(setting);
    }
}

void credits_menu(){
    int choice = 0;
    int highlight = 0;
    const char* choices[] = {"Credits", "Return"};
    
    WINDOW *credits = newwin(20, 60, 5, 5);
    keypad(credits, TRUE);
    box(credits, 0, 0);
    

    while(1){
        for(int i = 0; i < 2; i++){
            if(i == highlight)
                wattron(credits, A_REVERSE);
            mvwprintw(credits, 5+8*i, 17, "%s", choices[i]);
            wattroff(credits, A_REVERSE);
        }
        choice = wgetch(credits);
        if(choice == '\n'){
            if(highlight == 0){
                mvwprintw(credits, 7, 18, "Made by InkC and Frank");
                wrefresh(credits);
            }   
            else if(highlight == 1){
                wclear(credits);
                wrefresh(credits);
                main_menu(0);
            }
        }
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
        wrefresh(credits);
    }
}

void tutorial(){
    int choice = 0;
    int highlight = 1;
    const char* choices[] = {"<- Prev", "Return", "Next ->"};
    
    int page_on = 0;
    int pos[] = {3, 27, 50};

    WINDOW *tutorial = newwin(20, 60, 5, 5);
    keypad(tutorial, TRUE);
    nodelay(tutorial, TRUE);

    box(tutorial, 0, 0);
    

    while(1){
        mvwprintw(tutorial, 14, 26, "Page: %d", page_on);
        for(int i = 0; i < 3; i++){
            if(page_on == 0 && i == 0){
                mvwprintw(tutorial, 16, pos[i], "       ");
            }
            else if(page_on == 5 && i == 2){
                mvwprintw(tutorial, 16, pos[i], "       ");
            }
            else{
                if(i == highlight){
                    wattron(tutorial, A_REVERSE);
                }
                mvwprintw(tutorial, 16, pos[i], "%s", choices[i]);
                wattroff(tutorial, A_REVERSE);
            }
        }
        choice = wgetch(tutorial);
        if(choice == '\n'){
            if(highlight == 0){
                page_on--;
                tutor_animation(tutorial, page_on);
            }   
            else if(highlight == 1){
                wclear(tutorial);
                wrefresh(tutorial);
                main_menu(0);
            }
            else if(highlight == 2){
                page_on++;
                tutor_animation(tutorial, page_on);
            }   
        }
        switch(choice)
        {
            case KEY_LEFT:
                if(page_on == 0 && highlight == 1)
                    highlight = 1;
                else{
                    highlight--;
                    if(highlight < 0)
                        highlight = 0;
                }
                break;
            case KEY_RIGHT:
                if(page_on == 5 && highlight == 1)
                    highlight = 1;
                else{
                    highlight++;
                    if(highlight > 2)
                        highlight = 2;
                }
                break;
            default:
                break;
        }
        
        //消失後往中間
        if((page_on == 0 && highlight == 0)||(page_on == 5 && highlight == 2)){
                highlight = 1;
        }

        wrefresh(tutorial);
    }
}

void tutor_animation(WINDOW *tutorial, int page_on){
    
}
int main() {
    int time_in_main = 0;

    setlocale(LC_ALL, "");
    initscr();                // Initialize ncurses mode
    cbreak();                 // Disable line buffering
    noecho();                 // Disable echoing of input
    curs_set(0); // Hide the cursor
    ESCDELAY = 10; // 10 ms wait time for multibyte sequences
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    main_menu(time_in_main);
    
    endwin();     // End ncurses mode
    return 0;
}

