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
void erase_area(WINDOW *win, int start_y, int start_x, int height, int width);
void *pickup_coin(void *arg);
void level_advance(WINDOW *score, int time_left, WINDOW *maze);
void time_out();
void end_game();

int x = 1, y = 1;
int level_counter = 1;
int point_counter = 0;
// 方向：上、下、左、右
int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};
int dfc_mode = 2;
int dif_setting = 0;
int num_ext = 2; //  3-lvl count

//**********function of generating MAZE*************

#define WIDTH  59 // 單邊網格數量
#define HEIGHT 19 // 單邊網格數量
int maze_map[HEIGHT][WIDTH]; 
int visited[HEIGHT][WIDTH]; 

void dfs(int start_x, int start_y) {
    visited[start_y][start_x] = 1;
    
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

//**********function of dfs*************
struct Queue{
    int front, rear;
    int y[HEIGHT*WIDTH];
    int x[HEIGHT*WIDTH];
};


void bfs(int x, int y) {
    struct Queue q;
    q.front = 0;
    q.rear = -1;

    visited[y][x] = 1;
    maze_map[y][x] = 1;

    q.y[++q.rear] = y;
    q.x[q.rear] = x;

    while (q.front <= q.rear) {
        int cur_y = q.y[q.front];
        int cur_x = q.x[q.front++];
        

        for (int i = 0; i < 4; i++) {

            //洗牌
            int random = rand() % 4;
            int tempx = directions[i][0], tempy = directions[i][1];
            directions[i][0] = directions[random][0];
            directions[i][1] = directions[random][1];
            directions[random][0] = tempx;
            directions[random][1] = tempy;
            

            int next_x = cur_x +  2* directions[i][0];
            int next_y = cur_y +  2* directions[i][1];

            if (next_x >= 0 && next_x < WIDTH && next_y >= 0 && next_y < HEIGHT && !visited[next_y][next_x]) {
                // 清除當前節點和下一節點之間的牆
                maze_map[cur_y + directions[i][1]][cur_x + directions[i][0]] = 1;

                // 標記下一節點為訪問過，並設置為路徑
                visited[next_y][next_x] = 1;
                maze_map[next_y][next_x] = 1;

                // 將下一節點加入隊列
                q.y[++q.rear] = next_y;
                q.x[q.rear] = next_x;
            }
        }
    }
}
/**********function of bfs*****************/

void bt() {// binary tree
    for (int i = 1; i < HEIGHT; i += 2) {
        for (int j = 1; j < WIDTH; j += 2) {
            // 如果在右下角，跳过
            if (i == HEIGHT - 2 && j == WIDTH - 2) continue;

            // 如果在最后一行，只能向右
            if (i == HEIGHT - 2) {
                maze_map[i][j + 1] = 1;
            }
            // 如果在最后一列，只能向下
            else if (j == WIDTH - 2) {
                maze_map[i + 1][j] = 1;
            }
            // 否则随机选择向右或向下
            else {
                if (rand() % 2) {
                    maze_map[i][j + 1] = 1; // 向右
                } else {
                    maze_map[i + 1][j] = 1; // 向下
                }
            }
        }
    }
}
/*************function of binary tree**************/

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
void gnr_exit()
{   
    int flag =0;    //判斷出口位置
    
    int end_x = 57, end_y = 18; //一定會生成右下角的出口
    maze_map[end_y][end_x] = 2;  
    
    if(level_counter == 2){
        num_ext = 1;
    }
    else if(level_counter == 3){
        num_ext = 0;
    }


    while(num_ext){
        end_x = rand()%(WIDTH/2) + 10*num_ext;
        end_y = rand()%HEIGHT;
        flag += maze_map[end_y + directions[0][1]][end_x + directions[0][0]];
        flag += maze_map[end_y + directions[1][1]][end_x + directions[1][0]];
        flag += maze_map[end_y + directions[2][1]][end_x + directions[2][0]];
        flag += maze_map[end_y + directions[3][1]][end_x + directions[3][0]];
        if(flag == 1 && maze_map[end_y][end_x] == 1) {
            maze_map[end_y][end_x] = 2;
            num_ext--;
        }
        flag = 0; //重置flag
    }   
}

void gnr_bns()
{
    int bns_x, bns_y;
    int num_bns = 3; //  
    while(num_bns){
        bns_x = rand()%(WIDTH/2) + 10*num_bns;
        bns_y = rand()%HEIGHT;
        if(maze_map[bns_y][bns_x] == 1) {
            maze_map[bns_y][bns_x] = 3;
            num_bns--;
        }
    }
}

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

    point_counter = 0; //reset
    level_counter = 1;

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
    int time_left;
    
    num_ext = 2;


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
    seed();                  

    //新增difficulty相關
    if(dfc_mode == 0){
        dfs(1, 1);
    }
    else if(dfc_mode == 1){
        bfs(1, 1);
    }
    else if(dfc_mode == 2){
        bt();
    }                
    gnr_exit();
    gnr_bns();

    for (int my = 1; my < HEIGHT; my++) {
        for (int mx = 1; mx < WIDTH; mx++) {
            if(maze_map[my][mx] == 1){mvwprintw(maze, my, mx, " ");} // " "表示路徑，"#"表示牆
            else if(maze_map[my][mx] == 2){
                wattron(maze, COLOR_PAIR(3));
                mvwprintw(maze, my, mx, "F");
                wattroff(maze, COLOR_PAIR(3));
            }
            else if(maze_map[my][mx] == 3){
                wattron(maze, COLOR_PAIR(1));
                mvwprintw(maze, my, mx, "$");
                wattroff(maze, COLOR_PAIR(1));
            }
            else if(maze_map[my][mx] == 0){mvwprintw(maze, my, mx, "#");}
            visited[my][mx] = 0; //重制visited 才能重新生成地圖
        }
        printf("\n");
    }
    //////

    while (key = wgetch(maze)) { // Read input from the maze window
        mvwprintw(cog, 1, 1, "ESC   *");

        mvwprintw(score, 1, 1, "Score: ");
        wattron(score, COLOR_PAIR(1));
        mvwprintw(score, 1, 7, "%6d", point_counter);
        wattroff(score, COLOR_PAIR(1));

        box(cog, 0, 0);
        box(score, 0, 0);
        box(maze, 0, 0);
        wattron(maze, COLOR_PAIR(4));
        mvwprintw(maze, y, x, "O");
        wattroff(maze, COLOR_PAIR(4));
        wrefresh(score);
        wrefresh(cog);
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
                else if(maze_map[y][x] == 3){
                    point_counter = point_counter + 500;
                    maze_map[y][x] = 1;
                    mvwprintw(score, 1, 1, "Score: ");
                    wattron(score, COLOR_PAIR(1));
                    mvwprintw(score, 1, 7, "%6d", point_counter);
                    wattroff(score, COLOR_PAIR(1));
                    wrefresh(score);
                }
                else if(maze_map[y][x] == 2){
                    maze_map[y][x] = 1;
                    wclear(cog);
                    wrefresh(cog);
                    wclear(timer);
                    wrefresh(timer);
                    wclear(score);
                    wrefresh(score);
                    level_advance(score, time_left, maze);
                    start_time = time(NULL);
                    time_in_maze = 0;
                }
            }
            else if (key == KEY_RIGHT) {
                x++;
                if (x > 58) x = 58; // Keep within right bound
                if (maze_map[y][x] == 0){//碰到 "#" 不移動 
                    x--;
                }
                else if(maze_map[y][x] == 3){
                    maze_map[y][x] = 1;
                    point_counter = point_counter + 500;
                    mvwprintw(score, 1, 1, "Score: ");
                    wattron(score, COLOR_PAIR(1));
                    mvwprintw(score, 1, 7, "%6d", point_counter);
                    wattroff(score, COLOR_PAIR(1));
                    wrefresh(score);
                }
                else if(maze_map[y][x] == 2){
                    maze_map[y][x] = 1;
                    wclear(cog);
                    wrefresh(cog);
                    wclear(timer);
                    wrefresh(timer);
                    wclear(score);
                    wrefresh(score);
                    level_advance(score, time_left, maze);
                    start_time = time(NULL);
                    time_in_maze = 0;
                }
            }
            else if (key == KEY_UP) {
                y--;
                if (y < 1) y = 1; // Keep within top bound
                if (maze_map[y][x] == 0){//碰到 "#" 不移動 
                    y++;
                }
                else if(maze_map[y][x] == 3){
                    maze_map[y][x] = 1;
                    point_counter = point_counter + 500;
                    mvwprintw(score, 1, 1, "Score: ");
                    wattron(score, COLOR_PAIR(1));
                    mvwprintw(score, 1, 7, "%6d", point_counter);
                    wattroff(score, COLOR_PAIR(1));
                    wrefresh(score);
                }
                else if(maze_map[y][x] == 2){
                    maze_map[y][x] = 1;
                    wclear(cog);
                    wrefresh(cog);
                    wclear(timer);
                    wrefresh(timer);
                    wclear(score);
                    wrefresh(score);
                    level_advance(score, time_left, maze);
                    start_time = time(NULL);
                    time_in_maze = 0;
                }
            }
            else if (key == KEY_DOWN) {
                y++;
                if (y > 18) y = 18; // Keep within bottom bound
                if (maze_map[y][x] == 0){//碰到 "#" 不移動 
                    y--;
                }
                else if(maze_map[y][x] == 3){
                    maze_map[y][x] = 1;
                    point_counter = point_counter + 500;
                    mvwprintw(score, 1, 1, "Score: ");
                    wattron(score, COLOR_PAIR(1));
                    mvwprintw(score, 1, 7, "%6d", point_counter);
                    wattroff(score, COLOR_PAIR(1));
                    wrefresh(score);
                }
                else if(maze_map[y][x] == 2){
                    maze_map[y][x] = 1;
                    wclear(cog);
                    wrefresh(cog);
                    wclear(timer);
                    wrefresh(timer);
                    wclear(score);
                    wrefresh(score);
                    level_advance(score, time_left, maze);
                    start_time = time(NULL);
                    time_in_maze = 0;
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
        if(time_in_maze != 0){  //Add the time passed in pause session
            time_left = 99 - elapsed_time - time_in_maze;
        }
        else{
            time_left = 99 - elapsed_time;
        }

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
    int highlight_dif = 0;
    const char* choices[] = {"Difficulty:", "Return"};
    const char* diff[] = {" Easy ", "Medium", " Hard "};
    const char* choices_dif[] = {"<", ">"};

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
        for(int i = 0; i < 2; i++){
            mvwprintw(setting, 5, 30+12*i, "%s", choices_dif[i]);
            mvwprintw(setting, 5, 34, "%s", diff[dif_setting]);
        }
        wrefresh(setting);
        choice = wgetch(setting);
        if(choice == '\n'){
            if(highlight == 0){
                while(1){
                    for(int i = 0; i < 2; i++){
                        if(i == highlight_dif)
                            wattron(setting, A_REVERSE);
                        mvwprintw(setting, 5, 30+12*i, "%s", choices_dif[i]);
                        wattroff(setting, A_REVERSE);
                        mvwprintw(setting, 5, 34, "%s", diff[dif_setting]);
                    }
                    choice = wgetch(setting);
                    if(choice == '\n'){
                        if(highlight_dif == 0){
                            dif_setting--;
                            if(dif_setting < 0)
                                dif_setting = 0;
                        }
                        else if(highlight_dif == 1){
                            dif_setting ++;
                            if(dif_setting > 2)
                                dif_setting = 2;
                        }
                    }
                    else if(choice == 27){
                        for(int i = 0; i < 2; i++){
                            mvwprintw(setting, 5, 30+12*i, "%s", choices_dif[i]);
                            mvwprintw(setting, 5, 34, "%s", diff[dif_setting]);
                        }
                        wrefresh(setting);
                        dfc_mode = 2 - dif_setting;
                        break;
                    }
                    switch(choice)
                    {
                        case KEY_LEFT:
                            highlight_dif--;
                            if( highlight_dif < 0)
                                 highlight_dif = 0;
                            break;
                        case KEY_RIGHT:
                             highlight_dif++;
                            if( highlight_dif > 1)
                                 highlight_dif = 1;
                            break;
                        default:
                            break;
                    }
                    wrefresh(setting);
                }
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
        mvwprintw(tutorial, 18, 27, "Page: %d", page_on+1);
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
                mvwprintw(tutorial, 18, 27, "Page: %d", page_on+1);
                tutor_animation(tutorial, page_on);
            }   
            else if(highlight == 1){
                wclear(tutorial);
                wrefresh(tutorial);
                main_menu(0);
            }
            else if(highlight == 2){
                page_on++;
                mvwprintw(tutorial, 18, 27, "Page: %d", page_on+1);
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
            mvwprintw(tutorial, 14, 13, "Beat the 3 levels to beat the game!!");
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

void level_advance(WINDOW *score, int time_left, WINDOW *maze){
    level_counter = level_counter + 1;
    WINDOW *level_up = newwin(20, 60, 5, 5);
    box(level_up,0,0);
    wrefresh(level_up);

    // 生成迷宮
    seed();                  

    //新增difficulty相關
    if(dfc_mode == 0){
        dfs(1, 1);
    }
    else if(dfc_mode == 1){
        bfs(1, 1);
    }
    else if(dfc_mode == 2){
        bt();
    }                
    gnr_exit();
    gnr_bns();

    for (int my = 1; my < HEIGHT; my++) {
        for (int mx = 1; mx < WIDTH; mx++) {
            if(maze_map[my][mx] == 1){mvwprintw(maze, my, mx, " ");} // " "表示路徑，"#"表示牆
            else if(maze_map[my][mx] == 2){
                wattron(maze, COLOR_PAIR(3));
                mvwprintw(maze, my, mx, "F");
                wattroff(maze, COLOR_PAIR(3));
            }
            else if(maze_map[my][mx] == 3){
                wattron(maze, COLOR_PAIR(1));
                mvwprintw(maze, my, mx, "$");
                wattroff(maze, COLOR_PAIR(1));
            }
            else if(maze_map[my][mx] == 0){mvwprintw(maze, my, mx, "#");}
            visited[my][mx] = 0; //重制visited 才能重新生成地圖
        }
        printf("\n");
    }
    mvwprintw(level_up, 12, 23, "TIME BONUS!!");
    mvwprintw(level_up, 8, 24, "LEVEL UP!!");
    while(time_left > 0){
        time_left--;
        point_counter = point_counter + 50;
        mvwprintw(level_up, 11, 19, "Time: %2d Score: %5d", time_left, point_counter);
        napms(50);
        wrefresh(level_up);
    }
    
    if(level_counter == 4){
        wclear(level_up);
        wrefresh(level_up);
        end_game();
    }

    for(int i = 0; i < 8; i++){
        if(i == 7){
            mvwprintw(level_up, 8, 24, "LEVEL     ");
            wrefresh(level_up);
        }
        mvwprintw(level_up, 1+i, 31, "%d", level_counter);
        mvwprintw(level_up, i, 31, " ");
        wrefresh(level_up);
        napms(100);
    }
    box(level_up, 0, 0);
    mvwprintw(level_up, 7, 30, "\\|/");
    mvwprintw(level_up, 8, 30, "-%d-", level_counter);
    mvwprintw(level_up, 9, 30, "/|\\");
    wrefresh(level_up);
    napms(100);
    mvwprintw(level_up, 7, 30, "   ");
    mvwprintw(level_up, 8, 30, " %d ", level_counter);
    mvwprintw(level_up, 9, 30, "   ");
    mvwprintw(level_up, 6, 29, "\\ | /");
    mvwprintw(level_up, 8, 29, "- %d -", level_counter);
    mvwprintw(level_up, 10, 29, "/ | \\");
    wrefresh(level_up);
    napms(100);
    mvwprintw(level_up, 6, 29, "     ");
    mvwprintw(level_up, 8, 29, "  %d  ", level_counter);
    mvwprintw(level_up, 10, 29, "     ");
    mvwprintw(level_up, 5, 28, "\\  |  /");
    mvwprintw(level_up, 8, 28, "-  %d  -", level_counter);
    mvwprintw(level_up, 11, 28, "/  |  \\");
    wrefresh(level_up);
    napms(100);
    mvwprintw(level_up, 5, 28, "       ");
    mvwprintw(level_up, 8, 28, "   %d   ", level_counter);
    mvwprintw(level_up, 11, 28, "       ");
    mvwprintw(level_up, 4, 27, "\\   |   /");
    mvwprintw(level_up, 8, 27, "-L  %d   -", level_counter);
    mvwprintw(level_up, 12, 27, "/   |   \\");
    wrefresh(level_up);
    napms(100);
    mvwprintw(level_up, 4, 27, "         ");
    mvwprintw(level_up, 8, 27, "    %d    ", level_counter);
    mvwprintw(level_up, 12, 27, "         ");
    mvwprintw(level_up, 3, 26, "\\    |    /");
    mvwprintw(level_up, 8, 26, "-EL  %d    -", level_counter);
    mvwprintw(level_up, 13, 26, "/    |    \\");
    wrefresh(level_up);
    napms(100);
    mvwprintw(level_up, 3, 26, "           ");
    mvwprintw(level_up, 8, 26, "VEL  %d     ", level_counter);
    mvwprintw(level_up, 13, 26, "           ");
    mvwprintw(level_up, 14, 15, "Press any key to continue...");
    wrefresh(level_up);
    mvwprintw(level_up, 12, 23, "TIME BONUS!!");
    mvwprintw(level_up, 11, 19, "Time: %2d Score: %5d", time_left, point_counter);
    wrefresh(level_up);
    wgetch(level_up);
    napms(300);
    wclear(level_up);
    wrefresh(level_up);

    x = 1; y = 1;
    wattron(maze, COLOR_PAIR(4));
    mvwprintw(maze, y, x, "O");
    wattroff(maze, COLOR_PAIR(4));
    wrefresh(maze);
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

void end_game(){
    int rate_counter = 0;
    WINDOW *end = newwin(20, 60, 5, 5);
    box(end,0,0);
    mvwprintw(end, 8, 24, "GAME OVER");
    wrefresh(end);
    napms(150);
    mvwprintw(end, 9, 18, "YOU BEAT THE MAZE GAME");
    wrefresh(end);
    napms(150);
    mvwprintw(end, 11, 18, "Your score is : %6d", rate_counter);
    wrefresh(end);
    napms(30);
    while(rate_counter < point_counter){
        rate_counter = rate_counter + 50;
        mvwprintw(end, 11, 18, "Your score is : %6d", rate_counter);
        wrefresh(end);
        napms(30);
    }
    napms(270);
    mvwprintw(end, 13, 18, "Your rate is ...");
    wrefresh(end);
    napms(150);
    if(point_counter > 14000){
        mvwprintw(end, 13, 18, "Your rate is ...   ");
        wattron(end, COLOR_PAIR(4));
        mvwprintw(end, 13, 33, "S");
        wattroff(end, COLOR_PAIR(4));
    }
    else if(point_counter > 12000){
        mvwprintw(end, 13, 18, "Your rate is ...   ");
        wattron(end, COLOR_PAIR(2));
        mvwprintw(end, 13, 33, "A");
        wattroff(end, COLOR_PAIR(2));
    }
    else if(point_counter > 10000){
        mvwprintw(end, 13, 18, "Your rate is ...   ");
        wattron(end, COLOR_PAIR(1));
        mvwprintw(end, 13, 33, "B");
        wattroff(end, COLOR_PAIR(1));
    }
    else if(point_counter > 6000){
        mvwprintw(end, 13, 18, "Your rate is ...   ");
        wattron(end, COLOR_PAIR(3));
        mvwprintw(end, 13, 33, "C");
        wattroff(end, COLOR_PAIR(3));
    }
    wrefresh(end);
    napms(100);
    mvwprintw(end, 16, 10, "Press any key to back to main menu...");
    wgetch(end);
    napms(100);
    delwin(end);
    main_menu(0);
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
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    main_menu(time_in_main);
    
    endwin();     // End ncurses mode
    return 0;
}

