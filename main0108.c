#include <ncurses.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define WIDTH  59 // 單邊網格數量
#define HEIGHT 19 // 單邊網格數量

//**********function of maze***********
int maze[HEIGHT][WIDTH]; // 迷宮大小
int visited[HEIGHT][WIDTH]; // 標記每個格點是否已訪問
int directions[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // 方向：上、下、左、右

void dfs(int x, int y) 
{
    visited[y][x] = 1;
    
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
        int next_x = x + 2*directions[i][0];
        int next_y = y + 2*directions[i][1];

        // 檢查是否越界或已訪問
        if (next_x >= 0 && next_x < WIDTH && next_y >= 0 && next_y < HEIGHT && !visited[next_y][next_x]) {
            // 移除牆
            maze[y + directions[i][1]][x + directions[i][0]] = 1;

            // 遞歸訪問鄰居
            dfs(next_x, next_y);
        }
    }
}
/**********function of bfs*****************/
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
    maze[y][x] = 1;

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
                maze[cur_y + directions[i][1]][cur_x + directions[i][0]] = 1;

                // 標記下一節點為訪問過，並設置為路徑
                visited[next_y][next_x] = 1;
                maze[next_y][next_x] = 1;

                // 將下一節點加入隊列
                q.y[++q.rear] = next_y;
                q.x[q.rear] = next_x;
            }
        }
    }
}
/**********function of bfs*****************/

void seed() {
    srand(time(NULL)); // 設置隨機種子

    // 初始化迷宮
    for (int i = 0; i <  HEIGHT ; i++) {
        for (int j = 0; j <  WIDTH ; j++) {
            if(i%2 == 1 && j%2 == 1){maze[i][j] = 1; } //通道
            else{maze[i][j] = 0; }// 牆壁
            visited[i][j] = 0;
        }
    }
    
    int start_x = 1, start_y = 1;
    int end_x = 57, end_y = 18;
    maze[start_y][start_x] = 1;  // 起始點
    maze[17][56] = 1;  // 修bfs bug
    maze[16][57] = 1;  // 修bfs bug

}

void print_maze() 
{
    for (int i = 1; i < HEIGHT; i++) {
        for (int j = 1; j < WIDTH; j++) {
            if(maze[i][j] == 1) {printf(" ");} //path
            else if(maze[i][j] == 2) {printf("F");} //exit
            else if(maze[i][j] == 3) {printf("$");} //bonus
            else {printf("#");} // wall
        }
        printf("\n");
    }
}

void gnr_exit()
{   
    int num_ext = 5; //  5 -level_cnt
    int flag =0;    //判斷出口位置
    
    int end_x = 57, end_y = 18; //一定會生成右下角的出口
    maze[end_y][end_x] = 2;  
    
    while(num_ext){
        end_x = rand()%(WIDTH/2) + 10*num_ext;
        end_y = rand()%HEIGHT;
        flag += maze[end_y + directions[0][1]][end_x + directions[0][0]];
        flag += maze[end_y + directions[1][1]][end_x + directions[1][0]];
        flag += maze[end_y + directions[2][1]][end_x + directions[2][0]];
        flag += maze[end_y + directions[3][1]][end_x + directions[3][0]];
        if(flag == 1 && maze[end_y][end_x] == 1) {
            maze[end_y][end_x] = 2;
            num_ext--;
        }
        flag = 0; //重置flag
    }   
}

void gnr_bonus()
{
    int bns_x, bns_y;
    int num_bns = 5; //  5 -level_cnt
    while(num_bns){
        bns_x = rand()%(WIDTH/2) + 10*num_bns;
        bns_y = rand()%HEIGHT;
        if(maze[bns_y][bns_x] == 1) {
            maze[bns_y][bns_x] = 3;
            num_bns--;
        }
    }
}

int main() {
    
    //生成全部牆壁('#') 
    seed();
    
    //打通道路生成迷宮
    bfs(1, 1);

    //生成出口/加分
    gnr_exit();
    gnr_bonus();
    
    // 打印生成的迷宮
    print_maze();

    printf("suc!\n");

    return 0;
}