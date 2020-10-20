#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct
{
    int n;
    int e;
    int s;
    int w;
    int visited;
} mazeBlock;

const int mazeRows = 8;
const int mazeCols = 8;

mazeBlock maze[8][8];

void printMaze()
{
    int i;
    int j;
    for (int j = 0; j<mazeCols; j++)
    {
        //printf("(%d %d %d %d) ", maze[i][j].n, maze[i][j].e, maze[i][j].s, maze[i][j].w);
        if (maze[0][j].n == 0)
        {
            printf("._");
        }
        else
        {
            printf("  ");
        }
    }
    printf("\n");
    for (int i = 0; i<mazeRows; i++)
    {
        for (int j = 0; j<mazeCols; j++)
        {
            //printf("(%d %d %d %d) ", maze[i][j].n, maze[i][j].e, maze[i][j].s, maze[i][j].w);
            if (maze[i][j].w == 0)
            {
                printf("|");
            }
            else
            {
                printf(" ");
            }
            if (maze[i][j].s == 0)
            {
                printf("_");
            }
            else
            {
                printf(" ");
            }
            
        }
        printf("|\n");
    }
    /*for (int i = 0; i<mazeRows; i++)
    {
        for (int j = 0; j<mazeCols; j++)
        {
            printf("(%d %d %d %d) ", maze[i][j].n, maze[i][j].e, maze[i][j].s, maze[i][j].w);
        }
        printf("\n");
    }*/
}

void recursiveMazeBuilder(int row, int col)
{
    int i = 0;
    int directions[4] = {0,1,2,3};

    for (i=0; i<4; i++)
    {
        int randomInt = rand() % (4-i);
        int choice = directions[randomInt];
        printMaze();
        if (choice == 0 && maze[row][col].n == 0 && row != 0 && maze[row-1][col].visited == 0) //Go up
        {
            maze[row][col].n = 1;
            maze[row-1][col].s = 1;
            maze[row-1][col].visited = 1;
            recursiveMazeBuilder(row-1, col);
        }
        else if (choice == 1 && maze[row][col].e == 0 && col != mazeCols-1 && maze[row][col+1].visited == 0) //Go right
        {
            maze[row][col].e = 1;
            maze[row][col+1].w = 1;
            maze[row][col+1].visited = 1;
            recursiveMazeBuilder(row, col+1);
        }
        else if (choice == 2 && maze[row][col].s == 0 && row != mazeRows-1 && maze[row+1][col].visited == 0)//Go down
        {
            maze[row][col].s = 1;
            maze[row+1][col].n = 1;
            maze[row+1][col].visited = 1;
            recursiveMazeBuilder(row+1, col);
        }
        else if (choice == 3 && maze[row][col].w == 0 && col != 0 && maze[row][col-1].visited == 0)//Go left
        {
            maze[row][col].w = 1;
            maze[row][col-1].e = 1;
            maze[row][col-1].visited = 1;
            recursiveMazeBuilder(row, col-1);
        }

        for (;randomInt<(4-i-1);randomInt++) //Shift array downwards.
        {
            directions[randomInt] = directions[randomInt+1];
        }
    }
}

int main()
{
    //Generate maze
    srand(time(NULL)); //Set the seed using time
    int startCol = rand() % 8;
    maze[0][startCol].n = 1;
    maze[0][startCol].visited = 1;
    recursiveMazeBuilder(0, startCol);
    startCol = rand() % 8;
    maze[7][startCol].s = 1;

    printMaze();
    //Solve Maze Backtracking

    //Solve using best path

    return 0;
}