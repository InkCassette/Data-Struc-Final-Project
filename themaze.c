#include <ncurses.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <pthread.h>

int main_menu(int time_in_main);
int pause_menu(int time_in_pause);
int maze(int time_in_maze);
void setting_menu();
void credits_menu();
void tutorial();
void tutor_animation(WINDOW *tutorial, int page_on);
void do_nothing();
void erase_area(WINDOW *win, int start_y, int start_x, int height, int width);
void *pickup_coin(void *arg);
void level_advance(WINDOW *score, WINDOW *timer, WINDOW *maze);
void time_out();

int x = 1, y = 1;
int level_counter = 0;
int point_counter = 0;

//**********function of generating MAZE*************

#define WIDTH  59 // 單邊網格數量
#define HEIGHT 19 // 單邊網格數量
int maze_map[HEIGHT][WIDTH]; 
int visited[HEIGHT][WIDTH]; 

void dfs(int start_x, int start_y) {
    visited[start_y][start_x] = 1;

    // 方向：上、下、左、右
    int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
    
    // 洗牌
    for (int i = 0; i < 4; i++) {
        int random = rand() % 4;
        int tempx = directions[i][0], tempy = directions[i][1];
        directions[i][0] = directions[random][0];
        directions[i][1] = directions[random][1];
        directions[random][0] = tempx;
        directions[random][1] = tempy;
    }

    // 遍歷每個方向
    for (int i = 0; i < 4; i++) {
        int next_x = start_x + 2*directions[i][0];
        int next_y = start_y + 2*directions[i][1];

        // 檢查越界跟visited
        if (next_x >= 0 && next_x < WIDTH && next_y >= 0 && next_y < HEIGHT && !visited[next_y][next_x]) {
            // 移除牆
            maze_map[start_y + directions[i][1]][start_x + directions[i][0]] = 1;

            // 遞歸
            dfs(next_x, next_y);
        }
    }
}

void seed() {
    
    srand(time(NULL)); // 設置隨機種子

    // 初始化迷宮
    for (int i = 0; i <  HEIGHT ; i++) {
        for (int j = 0; j <  WIDTH ; j++) {
            if(i%2 == 1 && j%2 == 1){maze_map[i][j] = 1; } //通道
            else{maze_map[i][j] = 0; }// 牆壁
        }
    }
    
    int start_x = 1, start_y = 1;
    int end_x = 57, end_y = 17;
    maze_map[start_y][start_x] = 1;  // 起始點
    maze_map[end_y][end_x] = 1;     // 終點
    maze_map[end_y-1][end_x] = 1;     // 終點
    maze_map[end_y][end_x-1] = 1;     // 終點
    maze_map[end_y-1][end_x] = 1;     // 終點

}

//**********function of generating MAZE*************

void erase_area(WINDOW *win, int start_y, int start_x, int height, int width) {
    for (int i = 0; i < height; ++i) {
        mvwprintw(win, start_y + i, start_x, "%*s", width, ""); // Overwrite with spaces
    }
    wrefresh(win); // Refresh to show the changes
}

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

    mvwprintw(pause, 4, 46, "Score: ");
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
    int time_left = 99;
    pthread_t score_counting;

    if(time_in_maze != 0){  //Add the time passed in prev session
        elapsed_time = time_in_maze;
    }

    WINDOW *timer = newwin(3, 12, 1, 5);
    WINDOW *maze = newwin(20, 60, 5, 5);
    WINDOW *cog = newwin(3, 9, 1, 54);
    WINDOW *score = newwin(3, 15, 1, 17);

    nodelay(maze, TRUE);
    keypad(maze, TRUE);       // Enable keypad for the maze window

    // Draw the initial box around the maze
    box(maze, 0, 0);
    box(cog, 0, 0);
    box(score, 0, 0);

    mvwprintw(cog, 1, 1, "ESC   *");

    mvwprintw(score, 1, 1, "Score: ");
    wattron(score, COLOR_PAIR(1));
    mvwprintw(score, 1, 7, "%6d", point_counter);
    wattroff(score, COLOR_PAIR(1));

    wrefresh(score);
    wrefresh(cog);
    wrefresh(maze);           // Refresh the maze window to display the box

    // 生成迷宮
    seed();                    //生成迷宮
    dfs(1, 1);                  //生成迷宮
    for (int my = 1; my < HEIGHT; my++) {
        for (int mx = 1; mx < WIDTH; mx++) {
            mvwprintw(maze, my, mx, maze_map[my][mx] ? " " : "#"); // " "表示路徑，"#"表示牆
            visited[my][mx] = 0; //重制visited 才能重新生成地圖
        }
        printf("\n");
    }
    //////

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
                if (maze_map[y][x] == 0){//碰到 "#" 不移動 
                    x++;
                }
                else if(maze_map[y][x] == 2){
                    pthread_create(&score_counting, NULL, pickup_coin, score);
                }
                else if(maze_map[y][x] == 3){
                    pthread_join(score_counting, NULL);
                    level_advance(score, timer, maze);
                }
            }
            else if (key == KEY_RIGHT) {
                x++;
                if (x > 58) x = 58; // Keep within right bound
                if (maze_map[y][x] == 0){//碰到 "#" 不移動 
                    x--;
                }
                else if(maze_map[y][x] == 2){
                    pthread_create(&score_counting, NULL, pickup_coin, score);
                }
                else if(maze_map[y][x] == 3){
                    pthread_join(score_counting, NULL);
                    level_advance(score, timer, maze);
                }
            }
            else if (key == KEY_UP) {
                y--;
                if (y < 1) y = 1; // Keep within top bound
                if (maze_map[y][x] == 0){//碰到 "#" 不移動 
                    y++;
                }
                else if(maze_map[y][x] == 2){
                    pthread_create(&score_counting, NULL, pickup_coin, score);
                }
                else if(maze_map[y][x] == 3){
                    pthread_join(score_counting, NULL);
                    level_advance(score, timer, maze);
                }
            }
            else if (key == KEY_DOWN) {
                y++;
                if (y > 18) y = 18; // Keep within bottom bound
                if (maze_map[y][x] == 0){//碰到 "#" 不移動 
                    y--;
                }
                else if(maze_map[y][x] == 2){
                    pthread_create(&score_counting, NULL, pickup_coin, score);
                }
                else if(maze_map[y][x] == 3){
                    pthread_join(score_counting, NULL);
                    level_advance(score, timer, maze);
                }
            }
            else if (key == 27) {
                wclear(cog);
                wrefresh(cog);
                wclear(timer);
                wrefresh(timer);
                wclear(score);
                wrefresh(score);
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
        time_left = 10 - elapsed_time;
        if(time_left < 0){
            wclear(cog);
            wrefresh(cog);
            wclear(timer);
            wrefresh(timer);
            wclear(score);                
            wrefresh(score);
            time_out();
        }
        box(timer, 0, 0);

        mvwprintw(timer, 1, 1, "Time: ");
        wattron(timer, COLOR_PAIR(1));
        mvwprintw(timer, 1, 6, "%4d", time_left);
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
    
    tutor_animation(tutorial, page_on);

    while(1){
        mvwprintw(tutorial, 18, 27, "Page: %d", page_on);
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
    erase_area(tutorial, 2, 2, 14, 58);
    box(tutorial, 0, 0);
    wrefresh(tutorial);
    switch(page_on)
    {
        case 0:
            int tutor_x = 26;
            int tutor_y = 6;
            mvwprintw(tutorial, 14, 14, "This is simply a game of maze... .");
            mvwprintw(tutorial,  6, 26, "o ###    # ");
            mvwprintw(tutorial,  7, 26, "#   ###  ##");
            mvwprintw(tutorial,  8, 26, "###   #  # ");
            mvwprintw(tutorial,  9, 26, "# ### #    ");
            mvwprintw(tutorial, 10, 26, "  # # #####");
            mvwprintw(tutorial, 11, 26, "### #      ");
            wrefresh(tutorial);
            char ani_o_move_1[] = {'R','D','R','R','D','R','R','D','D','D','R','R','R','R','R'};
            for(int i = 0; i < 15; i++){
                switch(ani_o_move_1[i]){
                    case 'R':
                        mvwprintw(tutorial, tutor_y, tutor_x, " ");
                        tutor_x++;
                        mvwprintw(tutorial, tutor_y, tutor_x, "o");
                        break;
                    case 'D':
                        mvwprintw(tutorial, tutor_y, tutor_x, " ");
                        tutor_y++;
                        mvwprintw(tutorial, tutor_y, tutor_x, "o");
                        break;
                    default:
                        break;
                }
                napms(150);
                wrefresh(tutorial);
            }
            napms(150);
            mvwprintw(tutorial, tutor_y, tutor_x, " ");
            break;
        case 1:
            tutor_x = 26;
            tutor_y = 8;
            mvwprintw(tutorial, 14, 10, "Reach the end to advance to the next level.");
            mvwprintw(tutorial,  7, 26, "###########");
            mvwprintw(tutorial,  8, 26, "o         F");
            mvwprintw(tutorial,  9, 26, "###########");
            wrefresh(tutorial);
            char ani_o_move_2[] = {'R','R','R','R','R','R','R','R','R','R'};
            for(int i = 0; i < 10; i++){
                switch(ani_o_move_2[i]){
                    case 'R':
                        mvwprintw(tutorial, tutor_y, tutor_x, " ");
                        tutor_x++;
                        mvwprintw(tutorial, tutor_y, tutor_x, "o");
                        break;
                    default:
                        break;
                }
                napms(150);
                wrefresh(tutorial);
            }
            napms(150);
            erase_area(tutorial, 7, 26, 3, 12);
            mvwprintw(tutorial,  8, 25, "  Finish!  ");
            wrefresh(tutorial);
            break;
        case 2:
            tutor_x = 26;
            tutor_y = 8;
            mvwprintw(tutorial, 14, 8, "Remerber to pick up some coins for extra point!");
            mvwprintw(tutorial,  7, 26, "###########");
            mvwprintw(tutorial,  8, 26, "o         $");
            mvwprintw(tutorial,  9, 26, "###########");
            wrefresh(tutorial);
            char ani_o_move_3[] = {'R','R','R','R','R','R','R','R','R','R'};
            for(int i = 0; i < 10; i++){
                switch(ani_o_move_3[i]){
                    case 'R':
                        mvwprintw(tutorial, tutor_y, tutor_x, " ");
                        tutor_x++;
                        mvwprintw(tutorial, tutor_y, tutor_x, "o");
                        break;
                    default:
                        break;
                }
                napms(150);
                wrefresh(tutorial);
            }
            napms(150);
            erase_area(tutorial, 7, 26, 3, 12);
            mvwprintw(tutorial,  8, 25, "   +500    ");
            wrefresh(tutorial);
            break;
        case 3:
            tutor_x = 99;
            tutor_y = 0;
            mvwprintw(tutorial, 14, 9, "Time spent is also a good bonus to your score!");
            mvwprintw(tutorial, 8, 21, "Time: %2d Score: %4d", tutor_x, tutor_y);
            mvwprintw(tutorial, 9, 25, "TIME BONUS!!!");
            wrefresh(tutorial);
            for(int i = 0; i < 99; i++){
                tutor_x--;
                tutor_y = tutor_y +50;
                mvwprintw(tutorial, 8, 21, "Time: %2d Score: %4d", tutor_x, tutor_y);
                napms(50);
                wrefresh(tutorial);
                mvwprintw(tutorial, 8, 21, "Time:    Score:   ");
            }
            mvwprintw(tutorial, 8, 21, "Time: %2d Score: %4d", tutor_x, tutor_y);
            wrefresh(tutorial);
            break;
        case 4:
            mvwprintw(tutorial, 13, 19, "If the game is too easy,");
            mvwprintw(tutorial, 14, 2, "try to spice it up by changing the difficulty settings!!");
            wrefresh(tutorial);
            mvwprintw(tutorial, 8, 27, "Setting");
            wrefresh(tutorial);
            napms(1000);

            wattron(tutorial, A_REVERSE);
            mvwprintw(tutorial, 8, 27, "Setting");
            wattroff(tutorial, A_REVERSE);
            wrefresh(tutorial);
            napms(300);

            mvwprintw(tutorial, 8, 27, "       ");
            mvwprintw(tutorial, 7, 20, "Difficulty:  <  Easy  >");
            wattron(tutorial, A_REVERSE);
            mvwprintw(tutorial, 9, 27, "Return");
            wattroff(tutorial, A_REVERSE);
            wrefresh(tutorial);
            napms(1000);

            mvwprintw(tutorial, 9, 27, "Return");
            wattron(tutorial, A_REVERSE);
            mvwprintw(tutorial, 7, 20, "Difficulty");
            wattroff(tutorial, A_REVERSE);
            mvwprintw(tutorial, 7, 30, ":  <  Easy  >");
            wrefresh(tutorial);
            napms(300);

            mvwprintw(tutorial, 7, 20, "Difficulty:  ");
            wattron(tutorial, A_REVERSE);
            mvwprintw(tutorial, 7, 33, "<");
            wattroff(tutorial, A_REVERSE);
            mvwprintw(tutorial, 7, 34, "  Easy  >");
            wrefresh(tutorial);
            napms(1000);

            mvwprintw(tutorial, 7, 33, "<  Easy  ");
            wattron(tutorial, A_REVERSE);
            mvwprintw(tutorial, 7, 42, ">");
            wattroff(tutorial, A_REVERSE);
            wrefresh(tutorial);
            napms(300);

            mvwprintw(tutorial, 7, 33, "< Medium ");
            wrefresh(tutorial);
            napms(300);

            mvwprintw(tutorial, 7, 33, "<  Hard  ");
            wrefresh(tutorial);
            napms(300);

            break;
        case 5:
            mvwprintw(tutorial, 14, 12, "Beat the 10 levels to beat the game!!");
            wrefresh(tutorial);
            for(int i = 0; i < 3; i++){
                mvwprintw(tutorial, 8, 24, "GAME OVER!!!");
                napms(1000);
                wrefresh(tutorial);
                mvwprintw(tutorial, 8, 24, "            ");
                napms(500);
                wrefresh(tutorial);
            }
            mvwprintw(tutorial, 8, 24, "GAME OVER!!!");
            wrefresh(tutorial);
            break;
        default:
            break;
    }
}

void *pickup_coin(void *arg){
    WINDOW *score = (WINDOW *)arg;
    for(int i = 0; i < 10; i++){
        point_counter = point_counter + 50;
        mvwprintw(score, 1, 1, "Score: ");
        wattron(score, COLOR_PAIR(1));
        mvwprintw(score, 1, 7, "%6d", point_counter);
        wattroff(score, COLOR_PAIR(1));
        napms(50);
        wrefresh(score);
    }
}

void level_advance(WINDOW *score, WINDOW *timer, WINDOW *maze){
    do_nothing();
}

void time_out(){
    int back = 0;
    WINDOW *lose = newwin(20, 60, 5, 5);
    keypad(lose, TRUE);
    box(lose, 0, 0);
    mvwprintw(lose, 5, 20, "You run out of time...");
    mvwprintw(lose, 7, 20, "Your score is %6d", point_counter);

    wattron(lose, A_REVERSE);
    mvwprintw(lose, 14, 23, "return to menu");
    wattroff(lose, A_REVERSE);
    
    while(back = wgetch(lose)){
        if(back == '\n')
            main_menu(0);
    }
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

